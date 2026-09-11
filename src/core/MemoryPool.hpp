#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <algorithm>

namespace nlm {

// Legacy memory pool for backward compatibility
template<typename T, size_t PoolSize = 1024>
class MemoryPool {
public:
    MemoryPool() : freeIndex(0) {}
    
    ~MemoryPool() {
        for (auto& block : blocks) {
            delete[] block;
        }
    }
    
    T* allocate() {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (freeIndex < static_cast<ptrdiff_t>(blocks.empty() ? 0 : blocks.back().size())) {
            return &blocks.back()[freeIndex++];
        }
        
        // Allocate new block
        T* block = new T[PoolSize];
        blocks.push_back(std::vector<T>(block, block + PoolSize));
        freeIndex = 1;
        return block;
    }
    
    void deallocate(T* ptr) {
        if (!ptr) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        // Find block and mark as free (simplified - in practice we'd track exact positions)
        freeIndex = std::min(freeIndex + 1, static_cast<ptrdiff_t>(blocks.back().size()));
    }
    
    static void* operator new(size_t size) {
        void* ptr = std::malloc(size);
        return ptr;
    }
    
    static void operator delete(void* ptr) noexcept {
        std::free(ptr);
    }
    
private:
    std::vector<std::vector<T>> blocks;
    std::mutex mutex;
    ptrdiff_t freeIndex;
};

// Specialized pools for common objects
class ObjectPools {
public:
    static MemoryPool<Neuron, 2048>* getNeuronPool() {
        static MemoryPool<Neuron, 2048> pool;
        return &pool;
    }
    
    static MemoryPool<Synapse, 4096>* getSynapsePool() {
        static MemoryPool<Synapse, 4096> pool;
        return &pool;
    }
    
    static MemoryPool<float, 65536>* getFloatPool() {
        static MemoryPool<float, 65536> pool;
        return &pool;
    }
    
    static std::vector<float>* getTempVector(size_t capacity = 0) {
        static thread_local std::vector<float> tempVec;
        if (capacity > tempVec.capacity()) {
            tempVec.clear();
            tempVec.reserve(capacity);
        }
        tempVec.resize(capacity);
        return &tempVec;
    }
    
    static void resetTempVectors() {
        // Clear thread-local temp vectors
    }
};

// Forward declaration for AdvancedMemoryPool
class AdvancedMemoryPoolManager;

// Global memory pool manager function
AdvancedMemoryPoolManager& getGlobalMemoryPoolManager();

// Convenience function to check if advanced pools are available
inline bool useAdvancedMemoryPools() {
    return true; // Always use advanced pools in Phase 6
}

} // namespace nlm