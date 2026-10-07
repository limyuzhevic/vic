#include "../performance/simd_object_pool.hpp"
#include <thread>
#include <chrono>
#include <numa.h>

namespace nlm {

// SIMDObjectPool implementation
template<typename T>
T* SIMDObjectPool<T>::allocate() {
    std::lock_guard<std::mutex> lock(allocationMutex_);
    
    T* obj = nullptr;
    
    if (freeObjects_.load(std::memory_order_acquire) > 0) {
        // Get from free list
        obj = popFromFreeList();
    } else {
        // Need to allocate new memory block
        obj = allocateNewBlock();
    }
    
    if (obj) {
        allocationCount_.fetch_add(1, std::memory_order_relaxed);
        totalObjects_.fetch_add(1, std::memory_order_relaxed);
        
        // SIMD-optimized initialization
        if (config_.enableSIMD) {
            initializeSIMD(obj, 1);
        } else {
            initializeScalar(obj, 1);
        }
    }
    
    return obj;
}

template<typename T>
std::vector<T*> SIMDObjectPool<T>::allocateBatch(size_t count) {
    std::lock_guard<std::mutex> lock(allocationMutex_);
    
    std::vector<T*> result;
    result.reserve(count);
    
    size_t allocated = 0;
    while (allocated < count) {
        if (freeObjects_.load(std::memory_order_acquire) > 0) {
            // Batch pop from free list
            size_t batchSize = std::min(count - allocated, freeObjects_.load(std::memory_order_relaxed));
            std::vector<T*> batch = popBatchFromFreeList(batchSize);
            result.insert(result.end(), batch.begin(), batch.end());
            allocated += batch.size();
        } else {
            // Allocate new blocks until we have enough
            T* obj = allocateNewBlock();
            if (obj) {
                result.push_back(obj);
                allocated++;
            } else {
                break; // Out of memory
            }
        }
    }
    
    if (allocated > 0) {
        allocationCount_.fetch_add(allocated, std::memory_order_relaxed);
        totalObjects_.fetch_add(allocated, std::memory_order_relaxed);
        
        // SIMD-optimized batch initialization
        if (config_.enableSIMD) {
            initializeSIMD(&result[0], allocated);
        } else {
            initializeScalar(&result[0], allocated);
        }
    }
    
    return result;
}

template<typename T>
void SIMDObjectPool<T>::deallocate(T* obj) {
    if (obj == nullptr) return;
    
    std::lock_guard<std::mutex> lock(deallocationMutex_);
    
    // SIMD-optimized cleanup
    if (config_.enableSIMD) {
        cleanupSIMD(obj, 1);
    } else {
        cleanupScalar(obj, 1);
    }
    
    pushToFreeList(obj);
    deallocationCount_.fetch_add(1, std::memory_order_relaxed);
    totalObjects_.fetch_sub(1, std::memory_order_relaxed);
}

template<typename T>
void SIMDObjectPool<T>::deallocateBatch(const std::vector<T*>& objects) {
    if (objects.empty()) return;
    
    std::lock_guard<std::mutex> lock(deallocationMutex_);
    
    // SIMD-optimized batch cleanup
    if (config_.enableSIMD) {
        cleanupSIMD(&objects[0], objects.size());
    } else {
        cleanupScalar(&objects[0], objects.size());
    }
    
    // Batch push to free list
    for (T* obj : objects) {
        pushToFreeList(obj);
    }
    
    deallocationCount_.fetch_add(objects.size(), std::memory_order_relaxed);
    totalObjects_.fetch_sub(objects.size(), std::memory_order_relaxed);
}

template<typename T>
void SIMDObjectPool<T>::defragment() {
    if (!config_.enableFragmentationReduction) return;
    
    std::lock_guard<std::mutex> lock(allocationMutex_);
    std::lock_guard<std::mutex> lock2(deallocationMutex_);
    
    // Extract all objects from free list
    std::vector<T*> allObjects;
    while (!freeList_.empty()) {
        allObjects.push_back(freeList_.back());
        freeList_.pop_back();
    }
    
    // Clear memory blocks and reallocate
    clear();
    
    // Re-allocate all objects
    for (T* obj : allObjects) {
        T* newObj = allocateNewBlock();
        if (newObj) {
            // Copy object data (requires T to be copyable)
            if constexpr (std::is_copy_constructible<T>::value) {
                *newObj = *obj;
            }
            pushToFreeList(obj); // Return old object
        }
    }
    
    fragmentationCounter_.fetch_add(1, std::memory_order_relaxed);
}

template<typename T>
void SIMDObjectPool<T>::clear() {
    std::lock_guard<std::mutex> lock(allocationMutex_);
    std::lock_guard<std::mutex> lock2(deallocationMutex_);
    
    // Reset free list
    freeList_.clear();
    
    // Release memory blocks
    for (auto& block : memoryBlocks_) {
        free(block.data, block.alignment);
    }
    
    memoryBlocks_.clear();
    totalObjects_.store(0, std::memory_order_relaxed);
    freeObjects_.store(0, std::memory_order_relaxed);
}

// Template specializations for common neural types
template class SIMDObjectPool<NeuronState>;
template class SIMDObjectPool<SynapseRecord>;
template class SIMDObjectPool<QueuedSpikeEvent>;

// NeuralSIMDObjectPools implementation
template<typename T>
static size_t calculateOptimalBlockSize(size_t currentDemand) {
    // Simple heuristic for block sizing
    if (currentDemand < 1024) {
        return 1024;
    } else if (currentDemand < 10240) {
        return 4096;
    } else if (currentDemand < 102400) {
        return 8192;
    } else {
        return 16384;
    }
}

// Inline helper functions
template<typename T>
static T* allocateAlignedMemory(size_t count, size_t objectSize, size_t alignment) {
    size_t totalSize = count * objectSize;
    
    if (alignment <= 16) {
        // Use _mm_malloc for small alignments
        return static_cast<T*>(_mm_malloc(totalSize, alignment));
    } else {
        // Use std::aligned_alloc for larger alignments
        return static_cast<T*>(std::aligned_alloc(alignment, totalSize));
    }
}

} // namespace nlm
