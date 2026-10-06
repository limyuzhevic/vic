#pragma once

// Vector memory pool system for NLM brain simulation
// Provides optimized allocation and deallocation for std::vector containers
// Reduces memory fragmentation and improves performance for temporary data structures

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include <cassert>
#include <cstdlib>

namespace nlm {

// ==================== VECTOR MEMORY POOL CONFIGURATION ====================

namespace VectorPoolConfig {
    // Pool sizes for different vector types
    constexpr size_t NEURON_VECTOR_POOL_SIZE = 1000;
    constexpr size_t SYNAPSE_VECTOR_POOL_SIZE = 1000;
    constexpr size_t FLOAT_VECTOR_POOL_SIZE = 2000;
    constexpr size_t DOUBLE_VECTOR_POOL_SIZE = 2000;
    constexpr size_t STRING_VECTOR_POOL_SIZE = 500;
    constexpr size_t TIMESTAMP_VECTOR_POOL_SIZE = 1000;
    constexpr size_t SIZE_T_VECTOR_POOL_SIZE = 1000;
    constexpr size_t INT_VECTOR_POOL_SIZE = 1000;
    
    // Block sizes for different vector types
    constexpr size_t VECTOR_BLOCK_SIZE = 1024;      // bytes per block
    constexpr size_t SMALL_VECTOR_BLOCK_SIZE = 256; // bytes per block for small vectors
    constexpr size_t LARGE_VECTOR_BLOCK_SIZE = 4096; // bytes per block for large vectors
    
    // Growth strategy
    constexpr size_t VECTOR_POOL_GROWTH_FACTOR = 2;
    constexpr size_t MAX_VECTOR_POOL_MULTIPLIER = 4;
    
    // Performance tuning
    constexpr bool ENABLE_VECTOR_ALIGNMENT = true;
    constexpr size_t MEMORY_ALIGNMENT = 16;         // bytes
    constexpr bool TRACK_VECTOR_USAGE = true;
    constexpr size_t MAX_VECTOR_MEMORY_MB = 128;    // Limit vector memory usage
}

// ==================== BASE VECTOR MEMORY POOL ====================

template<typename T>
class VectorMemoryPoolBase {
public:
    VectorMemoryPoolBase(size_t initialCapacity = 0)
        : currentCapacity_(0), maxCapacity_(0), freeList_(), statistics_() {
        if (initialCapacity > 0) {
            grow(initialCapacity);
        }
    }
    
    virtual ~VectorMemoryPoolBase() {
        // Clean up all blocks
        for (auto& block : blocks_) {
            delete[] block;
        }
    }
    
    // Allocate a vector from the pool
    std::vector<T>* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (freeList_.empty()) {
            // Need to grow the pool
            grow(blocks_.size() * VectorPoolConfig::VECTOR_POOL_GROWTH_FACTOR);
        }
        
        if (freeList_.empty()) {
            throw std::bad_alloc(); // Out of memory
        }
        
        std::vector<T>* vec = freeList_.back();
        freeList_.pop_back();
        statistics_.allocatedCount++;
        
        return vec;
    }
    
    // Deallocate a vector back to the pool
    void deallocate(std::vector<T>* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find which block this pointer belongs to
        size_t blockIndex = findBlockIndex(ptr);
        if (blockIndex != static_cast<size_t>(-1)) {
            // Add to free list
            freeList_.push_back(ptr);
            statistics_.deallocatedCount++;
        }
    }
    
    // Get current capacity
    size_t capacity() const { return currentCapacity_; }
    
    // Get number of free vectors
    size_t freeCount() const { return freeList_.size(); }
    
    // Get statistics
    const auto& getStatistics() const { return statistics_; }
    
    // Clear all free vectors (for reset operations)
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        freeList_.clear();
        // Clear all vectors in blocks
        for (auto& block : blocks_) {
            for (size_t i = 0; i < block.capacity; ++i) {
                auto& vec = reinterpret_cast<std::vector<T>*>(block.data)[i];
                vec.clear();
                vec.shrink_to_fit();
            }
        }
    }
    
    // Check if pool is empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return freeList_.empty();
    }
    
    // Check if pool has enough capacity for count vectors
    bool hasCapacity(size_t count) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count <= freeList_.size();
    }
    
protected:
    struct Block {
        void* data;
        size_t capacity;
        
        Block() : data(nullptr), capacity(0) {}
        Block(void* d, size_t c) : data(d), capacity(c) {}
    };
    
    // Find which block contains a pointer
    size_t findBlockIndex(std::vector<T>* ptr) const {
        for (size_t i = 0; i < blocks_.size(); ++i) {
            if (blocks_[i].data != nullptr) {
                auto* blockStart = static_cast<std::vector<T>*>(blocks_[i].data);
                auto* ptrChar = static_cast<std::vector<T>*>(ptr);
                if (ptrChar >= blockStart && 
                    ptrChar < blockStart + blocks_[i].capacity) {
                    return i;
                }
            }
        }
        return static_cast<size_t>(-1);
    }
    
    // Grow the pool by adding more blocks
    void grow(size_t targetCapacity) {
        while (currentCapacity_ < targetCapacity) {
            // Calculate how many vectors we can add to this block
            size_t objectsPerBlock = std::min(
                VectorPoolConfig::NEURON_VECTOR_POOL_SIZE,
                (targetCapacity - currentCapacity_) / (blocks_.size() + 1)
            );
            
            // Allocate block
            void* blockData = ::operator new(objectsPerBlock * sizeof(std::vector<T>));
            if (!blockData) {
                throw std::bad_alloc(); // Cannot allocate memory
            }
            
            // Initialize all vectors in the block to be empty (which puts them in free list)
            for (size_t i = 0; i < objectsPerBlock; ++i) {
                auto* vecPtr = static_cast<std::vector<T>*>(blockData) + i;
                freeList_.push_back(vecPtr);
            }
            
            blocks_.emplace_back(blockData, objectsPerBlock);
            maxCapacity_ += objectsPerBlock;
        }
    }
    
private:
    std::vector<Block> blocks_;
    std::vector<std::vector<T>*> freeList_;
    size_t currentCapacity_;
    size_t maxCapacity_;
    mutable std::mutex mutex_;
    
    struct PoolStatistics {
        size_t allocatedCount = 0;
        size_t deallocatedCount = 0;
        size_t allocatedBytes = 0;
        size_t peakAllocated = 0;
    };
    
    PoolStatistics statistics_;
};

// ==================== SPECIALIZED VECTOR MEMORY POOLS ====================

// Neuron pointer vector memory pool
class NeuronVectorMemoryPool : public VectorMemoryPoolBase<Neuron*> {
public:
    NeuronVectorMemoryPool(size_t capacity = VectorPoolConfig::NEURON_VECTOR_POOL_SIZE)
        : VectorMemoryPoolBase<Neuron*>(capacity) {}
};

// Synapse pointer vector memory pool
class SynapseVectorMemoryPool : public VectorMemoryPoolBase<Synapse*> {
public:
    SynapseVectorMemoryPool(size_t capacity = VectorPoolConfig::SYNAPSE_VECTOR_POOL_SIZE)
        : VectorMemoryPoolBase<Synapse*>(capacity) {}
};

// Float vector memory pool
class FloatVectorMemoryPool : public VectorMemoryPoolBase<float> {
public:
    FloatVectorMemoryPool(size_t capacity = VectorPoolConfig::FLOAT_VECTOR_POOL_SIZE)
        : VectorMemoryPoolBase<float>(capacity) {}
};

// Double vector memory pool
class DoubleVectorMemoryPool : public VectorMemoryPoolBase<double> {
public:
    DoubleVectorMemoryPool(size_t capacity = VectorPoolConfig::DOUBLE_VECTOR_POOL_SIZE)
        : VectorMemoryPoolBase<double>(capacity) {}
};

// String vector memory pool
class StringVectorMemoryPool : public VectorMemoryPoolBase<std::string> {
public:
    StringVectorMemoryPool(size_t capacity = VectorPoolConfig::STRING_VECTOR_POOL_SIZE)
        : VectorMemoryPoolBase<std::string>(capacity) {}
};

// Timestamp vector memory pool
class TimestampVectorMemoryPool : public VectorMemoryPoolBase<Timestamp> {
public:
    TimestampVectorMemoryPool(size_t capacity = VectorPoolConfig::TIMESTAMP_VECTOR_POOL_SIZE)
        : VectorMemoryPoolBase<Timestamp>(capacity) {}
};

// Size_t vector memory pool
class SizeTVectorMemoryPool : public VectorMemoryPoolBase<size_t> {
public:
    SizeTVectorMemoryPool(size_t capacity = VectorPoolConfig::SIZE_T_VECTOR_POOL_SIZE)
        : VectorMemoryPoolBase<size_t>(capacity) {}
};

// Int vector memory pool
class IntVectorMemoryPool : public VectorMemoryPoolBase<int> {
public:
    IntVectorMemoryPool(size_t capacity = VectorPoolConfig::INT_VECTOR_POOL_SIZE)
        : VectorMemoryPoolBase<int>(capacity) {}
};

// ==================== VECTOR MEMORY MANAGER ====================

class VectorMemoryManager {
public:
    VectorMemoryManager() {
        initializePools();
    }
    
    ~VectorMemoryManager() {
        // Clean up all pools
        for (auto& pair : pools_) {
            delete pair.second;
        }
    }
    
    // Get or create a vector memory pool
    template<typename PoolType>
    PoolType* getPool() {
        std::string poolName = getPoolName<PoolType>();
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            // Create new pool
            PoolType* pool = new PoolType();
            pools_[poolName] = pool;
            return pool;
        }
        return static_cast<PoolType*>(it->second);
    }
    
    // Initialize all default pools
    void initializePools() {
        // Neuron pointer vector pool
        getPool<NeuronVectorMemoryPool>();
        
        // Synapse pointer vector pool
        getPool<SynapseVectorMemoryPool>();
        
        // Float vector pool
        getPool<FloatVectorMemoryPool>();
        
        // Double vector pool
        getPool<DoubleVectorMemoryPool>();
        
        // String vector pool
        getPool<StringVectorMemoryPool>();
        
        // Timestamp vector pool
        getPool<TimestampVectorMemoryPool>();
        
        // Size_t vector pool
        getPool<SizeTVectorMemoryPool>();
        
        // Int vector pool
        getPool<IntVectorMemoryPool>();
    }
    
    // Get memory pools
    NeuronVectorMemoryPool& getNeuronVectorPool() { return *getPool<NeuronVectorMemoryPool>(); }
    SynapseVectorMemoryPool& getSynapseVectorPool() { return *getPool<SynapseVectorMemoryPool>(); }
    FloatVectorMemoryPool& getFloatVectorPool() { return *getPool<FloatVectorMemoryPool>(); }
    DoubleVectorMemoryPool& getDoubleVectorPool() { return *getPool<DoubleVectorMemoryPool>(); }
    StringVectorMemoryPool& getStringVectorPool() { return *getPool<StringVectorMemoryPool>(); }
    TimestampVectorMemoryPool& getTimestampVectorPool() { return *getPool<TimestampVectorMemoryPool>(); }
    SizeTVectorMemoryPool& getSizeTVectorPool() { return *getPool<SizeTVectorMemoryPool>(); }
    IntVectorMemoryPool& getIntVectorPool() { return *getPool<IntVectorMemoryPool>(); }
    
    // Clear all pools
    void clearAllPools() {
        for (auto& pair : pools_) {
            delete pair.second;
            pools_.erase(pair.first);
        }
        initializePools();
    }
    
    // Get memory usage report
    std::string getMemoryReport() {
        std::ostringstream oss;
        oss << "Vector Memory Pool Report:" << std::endl;
        
        for (const auto& pair : pools_) {
            oss << "  " << pair.first << ": "
                << pair.second->getStatistics().allocatedCount << " allocated" << std::endl;
        }
        
        return oss.str();
    }
    
    // Check if memory limit is exceeded
    bool isMemoryLimitExceeded() {
        size_t totalMemory = 0;
        for (const auto& pair : pools_) {
            totalMemory += pair.second->getStatistics().allocatedBytes;
        }
        
        return totalMemory > VectorPoolConfig::MAX_VECTOR_MEMORY_MB * 1024 * 1024;
    }
    
protected:
    template<typename PoolType>
    std::string getPoolName() {
        std::string name = std::string(typeid(PoolType).name());
        // Extract just the class name without template parameters
        size_t pos = name.find("class ");
        if (pos != std::string::npos) {
            name = name.substr(pos + 6);
        }
        return name;
    }
    
private:
    std::unordered_map<std::string, VectorMemoryPoolBase<void>*> pools_;
    mutable std::mutex mutex_;
};

// ==================== GLOBAL VECTOR MEMORY MANAGER INSTANCE ====================

inline VectorMemoryManager& getGlobalVectorMemoryManager() {
    static VectorMemoryManager manager;
    return manager;
}

// ==================== CONVENIENCE FUNCTIONS ====================

// Get vector memory manager
inline VectorMemoryManager& getVectorMemoryManager() {
    return getGlobalVectorMemoryManager();
}

// Get neuron vector pool (convenience function)
inline NeuronVectorMemoryPool& getNeuronVectorPool() {
    return getVectorMemoryManager().getNeuronVectorPool();
}

// Get synapse vector pool (convenience function)
inline SynapseVectorMemoryPool& getSynapseVectorPool() {
    return getVectorMemoryManager().getSynapseVectorPool();
}

// Get float vector pool (convenience function)
inline FloatVectorMemoryPool& getFloatVectorPool() {
    return getVectorMemoryManager().getFloatVectorPool();
}

// Get double vector pool (convenience function)
inline DoubleVectorMemoryPool& getDoubleVectorPool() {
    return getVectorMemoryManager().getDoubleVectorPool();
}

// Get string vector pool (convenience function)
inline StringVectorMemoryPool& getStringVectorPool() {
    return getVectorMemoryManager().getStringVectorPool();
}

// Get timestamp vector pool (convenience function)
inline TimestampVectorMemoryPool& getTimestampVectorPool() {
    return getVectorMemoryManager().getTimestampVectorPool();
}

// Get size_t vector pool (convenience function)
inline SizeTVectorMemoryPool& getSizeTVectorPool() {
    return getVectorMemoryManager().getSizeTVectorPool();
}

// Get int vector pool (convenience function)
inline IntVectorMemoryPool& getIntVectorPool() {
    return getVectorMemoryManager().getIntVectorPool();
}

// Allocate vector from memory pool
template<typename T>
inline std::vector<T>* allocateVector() {
    return getVectorMemoryManager().getPool<VectorMemoryPoolBase<T>>()->allocate();
}

// Deallocate vector back to memory pool
template<typename T>
inline void deallocateVector(std::vector<T>* vector) {
    getVectorMemoryManager().getPool<VectorMemoryPoolBase<T>>()->deallocate(vector);
}

// Clear all vector pools (for reset operations)
inline void clearVectorPools() {
    getVectorMemoryManager().clearAllPools();
}

// Get vector memory usage report
inline std::string getVectorMemoryReport() {
    return getVectorMemoryManager().getMemoryReport();
}

// Check if vector memory limit is exceeded
inline bool isVectorMemoryLimitExceeded() {
    return getVectorMemoryManager().isMemoryLimitExceeded();
}

// ==================== CUSTOM ALLOCATOR DELEGATES FOR VECTORS ====================

// Custom allocator for vectors using memory pool
template<typename T>
struct VectorAllocator {
    typedef T value_type;
    
    std::vector<T>* allocate(std::size_t count) {
        if (count == 1) {
            return getVectorMemoryManager().getPool<VectorMemoryPoolBase<T>>()->allocate();
        } else {
            // For multiple elements, allocate from system memory
            auto* vec = new std::vector<T>(count);
            return vec;
        }
    }
    
    void deallocate(std::vector<T>* ptr, std::size_t count) {
        if (count == 1) {
            getVectorMemoryManager().getPool<VectorMemoryPoolBase<T>>()->deallocate(ptr);
        } else {
            delete ptr;
        }
    }
    
    template<typename U>
    struct rebind {
        typedef VectorAllocator<U> other;
    };
};

// Specialized allocator for different types
struct NeuronVectorAllocator {
    typedef Neuron* value_type;
    
    std::vector<Neuron*>* allocate(std::size_t count) {
        if (count == 1) {
            return getNeuronVectorPool().allocate();
        } else {
            auto* vec = new std::vector<Neuron*>(count);
            return vec;
        }
    }
    
    void deallocate(std::vector<Neuron*>* ptr, std::size_t count) {
        if (count == 1) {
            getNeuronVectorPool().deallocate(ptr);
        } else {
            delete ptr;
        }
    }
};

struct SynapseVectorAllocator {
    typedef Synapse* value_type;
    
    std::vector<Synapse*>* allocate(std::size_t count) {
        if (count == 1) {
            return getSynapseVectorPool().allocate();
        } else {
            auto* vec = new std::vector<Synapse*>(count);
            return vec;
        }
    }
    
    void deallocate(std::vector<Synapse*>* ptr, std::size_t count) {
        if (count == 1) {
            getSynapseVectorPool().deallocate(ptr);
        } else {
            delete ptr;
        }
    }
};

struct FloatVectorAllocator {
    typedef float value_type;
    
    std::vector<float>* allocate(std::size_t count) {
        if (count == 1) {
            return getFloatVectorPool().allocate();
        } else {
            auto* vec = new std::vector<float>(count);
            return vec;
        }
    }
    
    void deallocate(std::vector<float>* ptr, std::size_t count) {
        if (count == 1) {
            getFloatVectorPool().deallocate(ptr);
        } else {
            delete ptr;
        }
    }
};

} // namespace nlm

// Include the original vector pool headers for backward compatibility
#include "core/VectorPool.hpp"