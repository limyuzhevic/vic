// Memory pool implementation for NLM
// Reduces memory allocation/deallocation overhead for frequently created objects

#pragma once

#include <vector>
#include <mutex>
#include <algorithm>

namespace nlm {
    
    // Generic memory pool for a specific type
    template<typename T>
    class MemoryPool {
    public:
        explicit MemoryPool(size_t initialCapacity = 1024, size_t maxCapacity = 65536) 
            : initialCapacity_(initialCapacity)
            , maxCapacity_(maxCapacity)
            , currentSize_(0)
            , pool_(new T[initialCapacity]) {}
        
        ~MemoryPool() {
            delete[] pool_;
        }
        
        // Allocate memory
        T* allocate() {
            std::lock_guard<std::mutex> lock(mutex_);
            
            if (currentSize_ >= initialCapacity_) {
                // Grow pool if needed
                size_t newCapacity = std::min(initialCapacity_ * 2, maxCapacity_);
                if (newCapacity > initialCapacity_) {
                    T* newPool = new T[newCapacity];
                    
                    // Move existing objects to new pool
                    std::copy(pool_, pool_ + currentSize_, newPool);
                    
                    delete[] pool_;
                    pool_ = newPool;
                    initialCapacity_ = newCapacity;
                }
            }
            
            return &pool_[currentSize_++];
        }
        
        // Deallocate memory (returns object to pool)
        void deallocate(T* ptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            
            // Find the pointer in our pool
            if (ptr >= pool_ && ptr < pool_ + currentSize_) {
                // Mark as free (in a simple pool, we don't actually free memory)
                // The memory remains allocated for reuse
            }
        }
        
        // Clear all allocated objects
        void clear() {
            std::lock_guard<std::mutex> lock(mutex_);
            currentSize_ = 0;
        }
        
        // Get current size
        size_t size() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return currentSize_;
        }
        
        // Check if pool is empty
        bool empty() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return currentSize_ == 0;
        }
        
        // Get current capacity
        size_t capacity() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return initialCapacity_;
        }
        
        // Iterator support for accessing allocated objects
        T* begin() { return pool_; }
        const T* begin() const { return pool_; }
        T* end() { return pool_ + currentSize_; }
        const T* end() const { return pool_ + currentSize_; }
        
    private:
        T* pool_;
        size_t initialCapacity_;
        size_t maxCapacity_;
        size_t currentSize_;
        mutable std::mutex mutex_;
    };
    
    // Specialized memory pools for common NLM types
    using SpikeEventPool = MemoryPool<DetailedSpikeEvent>;
    using DelayedSpikeEventPool = MemoryPool<DelayedSpikeEvent>;
    
} // namespace nlm