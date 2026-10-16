#include "MemoryPool.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../memory/WorkingMemory.hpp"
#include "../memory/Memory.hpp"
#include <thread>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include <chrono>

namespace nlm {
namespace memory {

// Memory pool implementation

// Global pool instances
std::unique_ptr<NeuronPool> g_neuronPool;
std::unique_ptr<SynapsePool> g_synapsePool;
std::unique_ptr<MemorySystemPool> g_memorySystemPool;

// Global statistics
MemoryPoolStatistics g_stats;

// Thread-local storage for pool instances
thread_local ThreadLocalPoolStorage threadLocalStorage;

// Helper functions
namespace {
    
    Timestamp getCurrentTime() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return static_cast<Timestamp>(duration.count() / 1000000.0);  // Convert to milliseconds
    }
    
    float calculateRelevance(const std::vector<NeuronId>& neurons, const std::vector<float>& activations) {
        if (neurons.empty()) return 0.0f;
        
        float sumActivations = 0.0f;
        float maxActivation = 0.0f;
        
        for (size_t i = 0; i < activations.size(); ++i) {
            sumActivations += activations[i];
            if (activations[i] > maxActivation) {
                maxActivation = activations[i];
            }
        }
        
        // Relevance combines average activation with max activation
        float avgActivation = sumActivations / activations.size();
        return (avgActivation + maxActivation) * 0.5f;
    }
    
} // namespace

// MemoryBlock implementation

MemoryBlock::MemoryBlock(size_t objectSize, size_t objectCount)
    : objectSize(objectSize)
    , objectCount(objectCount)
    , allocations(0)
    , deallocations(0)
    , currentUsage(0)
    , active(true) {
    
    size_t totalSize = objectSize * objectCount;
    memoryStart = ::operator new(totalSize);
    
    // Initialize free list
    freeListHead = memoryStart;
    
    // Calculate byte offset for free list pointer
    freeListPtr = static_cast<void**>(memoryStart);
    
    // Initialize all objects as free
    for (size_t i = 0; i < objectCount - 1; ++i) {
        void* current = static_cast<void*>(static_cast<char*>(memoryStart) + i * objectSize);
        void* next = static_cast<void*>(static_cast<char*>(memoryStart) + (i + 1) * objectSize);
        *static_cast<void**>(current) = next;
    }
    *static_cast<void**>(static_cast<char*>(memoryStart) + (objectCount - 1) * objectSize) = nullptr;
    
    // Lock the block for thread safety
    // mutex is automatically constructed
}

MemoryBlock::~MemoryBlock() {
    if (memoryStart) {
        ::operator delete(memoryStart);
        memoryStart = nullptr;
        freeListHead = nullptr;
        freeListPtr = nullptr;
    }
}

bool MemoryBlock::allocate(void** objPtr) {
    if (!active || currentUsage >= objectCount) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex);
    
    if (freeListHead == nullptr) {
        return false;
    }
    
    *objPtr = freeListHead;
    freeListHead = *static_cast<void**>(*objPtr);
    currentUsage++;
    allocations++;
    
    return true;
}

void MemoryBlock::deallocate(void* objPtr) {
    if (!active || objPtr == nullptr) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex);
    
    if (currentUsage <= 0) {
        return;
    }
    
    // Add to free list
    *static_cast<void**>(objPtr) = freeListHead;
    freeListHead = objPtr;
    currentUsage--;
    deallocations++;
}

void MemoryBlock::reset() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Rebuild free list
    freeListHead = memoryStart;
    currentUsage = 0;
}

bool MemoryBlock::isFull() const {
    return currentUsage >= objectCount;
}

bool MemoryBlock::isEmpty() const {
    return currentUsage == 0;
}

PoolStatistics MemoryBlock::getStatistics() const {
    PoolStatistics stats;
    stats.allocationCount = allocations;
    stats.deallocationCount = deallocations;
    stats.currentSize = currentUsage;
    stats.blockCount = 1;
    stats.totalMemoryUsage = objectSize * objectCount;
    
    if (stats.totalMemoryUsage > 0) {
        stats.utilizationRate = static_cast<float>(stats.currentSize) / objectCount;
    } else {
        stats.utilizationRate = 0.0f;
    }
    
    return stats;
}

// ObjectPool implementation

template<typename T, size_t BlockSize, bool ThreadLocal>
ObjectPool<T, BlockSize, ThreadLocal>::ObjectPool()
    : autoInit_(true)
    , maxBlocks_(1024) {
    
    // Initialize global instance if not thread-local
    if (!ThreadLocal) {
        globalInstance_ = this;
    }
}

template<typename T, size_t BlockSize, bool ThreadLocal>
ObjectPool<T, BlockSize, ThreadLocal>::~ObjectPool() {
    // Clear all blocks
    for (auto& block : blocks_) {
        delete block;
    }
    blocks_.clear();
    
    if (!ThreadLocal) {
        globalInstance_ = nullptr;
    }
}

template<typename T, size_t BlockSize, bool ThreadLocal>
T* ObjectPool<T, BlockSize, ThreadLocal>::allocate() {
    if (ThreadLocal) {
        return threadLocalInstance->allocateInternal();
    } else {
        return globalInstance_->allocateInternal();
    }
}

template<typename T, size_t BlockSize, bool ThreadLocal>
void ObjectPool<T, BlockSize, ThreadLocal>::deallocate(T* obj) {
    if (obj == nullptr) return;
    
    if (ThreadLocal) {
        threadLocalInstance->deallocateInternal(obj);
    } else {
        globalInstance_->deallocateInternal(obj);
    }
}

template<typename T, size_t BlockSize, bool ThreadLocal>
void ObjectPool<T, BlockSize, ThreadLocal>::clear() {
    if (ThreadLocal) {
        threadLocalInstance->clearInternal();
    } else {
        globalInstance_->clearInternal();
    }
}

template<typename T, size_t BlockSize, bool ThreadLocal>
T* ObjectPool<T, BlockSize, ThreadLocal>::allocateInternal() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Try to allocate from existing blocks
    for (auto& block : blocks_) {
        void* objPtr = nullptr;
        if (block->allocate(&objPtr)) {
            allocationCount_++;
            
            if (autoInit_) {
                // Default initialization
                T* obj = static_cast<T*>(objPtr);
                new (obj) T();
            }
            
            return static_cast<T*>(objPtr);
        }
    }
    
    // Create new block if we haven't reached max
    if (blocks_.size() < maxBlocks_) {
        MemoryBlock* newBlock = createNewBlock();
        if (newBlock) {
            blocks_.push_back(newBlock);
            
            void* objPtr = nullptr;
            if (newBlock->allocate(&objPtr)) {
                allocationCount_++;
                
                if (autoInit_) {
                    T* obj = static_cast<T*>(objPtr);
                    new (obj) T();
                }
                
                return static_cast<T*>(objPtr);
            }
        }
    }
    
    return nullptr;
}

template<typename T, size_t BlockSize, bool ThreadLocal>
void ObjectPool<T, BlockSize, ThreadLocal>::deallocateInternal(T* obj) {
    if (obj == nullptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the block containing this object
    for (auto& block : blocks_) {
        // Check if object is within this block's memory range
        char* memoryStart = static_cast<char*>(block->memoryStart);
        size_t objectSize = sizeof(T);
        size_t objectCount = block->objectCount;
        
        // Calculate offset of object within block
        intptr_t objPtr = reinterpret_cast<intptr_t>(obj);
        intptr_t blockPtr = reinterpret_cast<intptr_t>(memoryStart);
        
        if (objPtr >= blockPtr && objPtr < blockPtr + objectSize * objectCount) {
            // Object is in this block
            size_t offset = (objPtr - blockPtr) / objectSize;
            
            // Get the free list pointer for this position
            void** freeListPtr = reinterpret_cast<void**>(
                static_cast<char*>(memoryStart) + offset * sizeof(void*));
            
            // Deallocate by adding to free list
            block->deallocate(obj);
            deallocationCount_++;
            
            // Destruct the object
            obj->~T();
            
            return;
        }
    }
}

template<typename T, size_t BlockSize, bool ThreadLocal>
void ObjectPool<T, BlockSize, ThreadLocal>::clearInternal() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& block : blocks_) {
        block->reset();
    }
    
    allocationCount_ = 0;
    deallocationCount_ = 0;
}

template<typename T, size_t BlockSize, bool ThreadLocal>
MemoryBlock* ObjectPool<T, BlockSize, ThreadLocal>::createNewBlock() {
    size_t objectSize = sizeof(T);
    size_t objectsPerBlock = BlockSize / objectSize;
    
    if (objectsPerBlock < 1) {
        objectsPerBlock = 1;
    }
    
    MemoryBlock* block = new MemoryBlock(objectSize, objectsPerBlock);
    return block;
}

template<typename T, size_t BlockSize, bool ThreadLocal>
size_t ObjectPool<T, BlockSize, ThreadLocal>::getAllocationCount() const {
    return allocationCount_.load();
}

template<typename T, size_t BlockSize, bool ThreadLocal>
size_t ObjectPool<T, BlockSize, ThreadLocal>::getDeallocationCount() const {
    return deallocationCount_.load();
}

template<typename T, size_t BlockSize, bool ThreadLocal>
size_t ObjectPool<T, BlockSize, ThreadLocal>::getCurrentSize() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->currentUsage;
    }
    return total;
}

template<typename T, size_t BlockSize, bool ThreadLocal>
size_t ObjectPool<T, BlockSize, ThreadLocal>::getBlockCount() const {
    return blocks_.size();
}

template<typename T, size_t BlockSize, bool ThreadLocal>
void ObjectPool<T, BlockSize, ThreadLocal>::setAutoInit(bool autoInit) {
    autoInit_ = autoInit;
}

template<typename T, size_t BlockSize, bool ThreadLocal>
void ObjectPool<T, BlockSize, ThreadLocal>::setMaxBlocks(size_t maxBlocks) {
    maxBlocks_ = maxBlocks;
}

// ThreadLocalObjectPool implementation

template<typename T, size_t BlockSize>
T* ThreadLocalObjectPool<T, BlockSize>::allocateInternal() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Try to allocate from existing blocks
    for (auto& block : blocks_) {
        void* objPtr = nullptr;
        if (block->allocate(&objPtr)) {
            allocationCount_++;
            
            if (autoInit_) {
                T* obj = static_cast<T*>(objPtr);
                new (obj) T();
            }
            
            return static_cast<T*>(objPtr);
        }
    }
    
    // Create new block if we haven't reached max
    if (blocks_.size() < maxBlocks_) {
        MemoryBlock* newBlock = createNewBlock();
        if (newBlock) {
            blocks_.push_back(newBlock);
            
            void* objPtr = nullptr;
            if (newBlock->allocate(&objPtr)) {
                allocationCount_++;
                
                if (autoInit_) {
                    T* obj = static_cast<T*>(objPtr);
                    new (obj) T();
                }
                
                return static_cast<T*>(objPtr);
            }
        }
    }
    
    return nullptr;
}

// NeuronPool implementation

NeuronPool::NeuronPool() {
    // Configure for typical neuron objects (approx. 64 bytes per neuron)
    // Block size: 512 bytes -> 8 neurons per block
}

SynapsePool::SynapsePool() {
    // Configure for typical synapse objects (approx. 48 bytes per synapse)
    // Block size: 2048 bytes -> 42 synapses per block
}

MemorySystemPool::MemorySystemPool() {
    // Initialize child pools
    workingMemoryPool_ = std::make_unique<ObjectPool<WorkingMemoryTrace, 256, true>>();
    episodicMemoryPool_ = std::make_unique<ObjectPool<EpisodicMemoryItem, 128, true>>();
}

// Global pool management

void initializePools() {
    g_neuronPool = std::make_unique<NeuronPool>();
    g_synapsePool = std::make_unique<SynapsePool>();
    g_memorySystemPool = std::make_unique<MemorySystemPool>();
}

void shutdownPools() {
    g_neuronPool.reset();
    g_synapsePool.reset();
    g_memorySystemPool.reset();
}

NeuronPool* getNeuronPool() {
    return g_neuronPool.get();
}

SynapsePool* getSynapsePool() {
    return g_synapsePool.get();
}

MemorySystemPool* getMemorySystemPool() {
    return g_memorySystemPool.get();
}

// Statistics implementation

PoolStatistics MemoryPoolStatistics::getNeuronPoolStats() const {
    PoolStatistics stats;
    stats.allocationCount = g_stats.neuronStats.allocationCount;
    stats.deallocationCount = g_stats.neuronStats.deallocationCount;
    stats.currentSize = g_stats.neuronStats.currentSize;
    stats.blockCount = g_stats.neuronStats.blockCount;
    stats.totalMemoryUsage = g_stats.neuronStats.totalMemoryUsage;
    stats.utilizationRate = g_stats.neuronStats.utilizationRate;
    return stats;
}

PoolStatistics MemoryPoolStatistics::getSynapsePoolStats() const {
    PoolStatistics stats;
    stats.allocationCount = g_stats.synapseStats.allocationCount;
    stats.deallocationCount = g_stats.synapseStats.deallocationCount;
    stats.currentSize = g_stats.synapseStats.currentSize;
    stats.blockCount = g_stats.synapseStats.blockCount;
    stats.totalMemoryUsage = g_stats.synapseStats.totalMemoryUsage;
    stats.utilizationRate = g_stats.synapseStats.utilizationRate;
    return stats;
}

PoolStatistics MemoryPoolStatistics::getMemoryPoolStats() const {
    PoolStatistics stats;
    stats.allocationCount = g_stats.memoryStats.allocationCount;
    stats.deallocationCount = g_stats.memoryStats.deallocationCount;
    stats.currentSize = g_stats.memoryStats.currentSize;
    stats.blockCount = g_stats.memoryStats.blockCount;
    stats.totalMemoryUsage = g_stats.memoryStats.totalMemoryUsage;
    stats.utilizationRate = g_stats.memoryStats.utilizationRate;
    return stats;
}

void MemoryPoolStatistics::printStatistics() const {
    std::cout << "=== NLM Memory Pool Statistics ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Neuron Pool:" << std::endl;
    std::cout << "  Allocations: " << g_stats.neuronStats.allocationCount << std::endl;
    std::cout << "  Deallocations: " << g_stats.neuronStats.deallocationCount << std::endl;
    std::cout << "  Current Size: " << g_stats.neuronStats.currentSize << std::endl;
    std::cout << "  Block Count: " << g_stats.neuronStats.blockCount << std::endl;
    std::cout << "  Total Memory: " << g_stats.neuronStats.totalMemoryUsage << " bytes" << std::endl;
    std::cout << "  Utilization Rate: " << (g_stats.neuronStats.utilizationRate * 100) << "%" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Synapse Pool:" << std::endl;
    std::cout << "  Allocations: " << g_stats.synapseStats.allocationCount << std::endl;
    std::cout << "  Deallocations: " << g_stats.synapseStats.deallocationCount << std::endl;
    std::cout << "  Current Size: " << g_stats.synapseStats.currentSize << std::endl;
    std::cout << "  Block Count: " << g_stats.synapseStats.blockCount << std::endl;
    std::cout << "  Total Memory: " << g_stats.synapseStats.totalMemoryUsage << " bytes" << std::endl;
    std::cout << "  Utilization Rate: " << (g_stats.synapseStats.utilizationRate * 100) << "%" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Memory System Pool:" << std::endl;
    std::cout << "  Allocations: " << g_stats.memoryStats.allocationCount << std::endl;
    std::cout << "  Deallocations: " << g_stats.memoryStats.deallocationCount << std::endl;
    std::cout << "  Current Size: " << g_stats.memoryStats.currentSize << std::endl;
    std::cout << "  Block Count: " << g_stats.memoryStats.blockCount << std::endl;
    std::cout << "  Total Memory: " << g_stats.memoryStats.totalMemoryUsage << " bytes" << std::endl;
    std::cout << "  Utilization Rate: " << (g_stats.memoryStats.utilizationRate * 100) << "%" << std::endl;
    std::cout << std::endl;
}

void MemoryPoolStatistics::exportStatistics(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "NLM Memory Pool Statistics" << std::endl;
    file << "Generated at: " << getCurrentTimeString() << std::endl;
    file << std::endl;
    
    file << "Neuron Pool:" << std::endl;
    file << "  Allocations: " << g_stats.neuronStats.allocationCount << std::endl;
    file << "  Deallocations: " << g_stats.neuronStats.deallocationCount << std::endl;
    file << "  Current Size: " << g_stats.neuronStats.currentSize << std::endl;
    file << "  Block Count: " << g_stats.neuronStats.blockCount << std::endl;
    file << "  Total Memory: " << g_stats.neuronStats.totalMemoryUsage << " bytes" << std::endl;
    file << "  Utilization Rate: " << g_stats.neuronStats.utilizationRate << std::endl;
    file << std::endl;
    
    file << "Synapse Pool:" << std::endl;
    file << "  Allocations: " << g_stats.synapseStats.allocationCount << std::endl;
    file << "  Deallocations: " << g_stats.synapseStats.deallocationCount << std::endl;
    file << "  Current Size: " << g_stats.synapseStats.currentSize << std::endl;
    file << "  Block Count: " << g_stats.synapseStats.blockCount << std::endl;
    file << "  Total Memory: " << g_stats.synapseStats.totalMemoryUsage << " bytes" << std::endl;
    file << "  Utilization Rate: " << g_stats.synapseStats.utilizationRate << std::endl;
    file << std::endl;
    
    file << "Memory System Pool:" << std::endl;
    file << "  Allocations: " << g_stats.memoryStats.allocationCount << std::endl;
    file << "  Deallocations: " << g_stats.memoryStats.deallocationCount << std::endl;
    file << "  Current Size: " << g_stats.memoryStats.currentSize << std::endl;
    file << "  Block Count: " << g_stats.memoryStats.blockCount << std::endl;
    file << "  Total Memory: " << g_stats.memoryStats.totalMemoryUsage << " bytes" << std::endl;
    file << "  Utilization Rate: " << g_stats.memoryStats.utilizationRate << std::endl;
    
    file.close();
}

std::string MemoryPoolStatistics::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace memory
} // namespace nlm