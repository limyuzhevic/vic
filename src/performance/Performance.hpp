#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <atomic>
#include <chrono>

namespace nlm {
namespace performance {

// Memory pool for efficient allocation
class MemoryPool {
public:
    template<typename T>
    class PoolAllocator {
    public:
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        
        PoolAllocator() = default;
        
        template<typename U>
        PoolAllocator(const PoolAllocator<U>&) noexcept {}
        
        size_type max_size() const noexcept {
            return std::numeric_limits<size_type>::max() / sizeof(T);
        }
        
        pointer allocate(size_type n) {
            if (n == 0) return nullptr;
            return static_cast<pointer>(malloc(n * sizeof(T)));
        }
        
        void deallocate(pointer p, size_type n) noexcept {
            free(p);
        }
        
        template<typename U, typename... Args>
        void construct(U* p, Args&&... args) {
            ::new((void*)p) U(std::forward<Args>(args)...);
        }
        
        template<typename U>
        void destroy(U* p) {
            p->~U();
        }
    };
    
    static constexpr size_t DEFAULT_POOL_SIZE = 1024;
    static constexpr size_t SMALL_OBJECT_POOL_SIZE = 256;
    static constexpr size_t LARGE_OBJECT_POOL_SIZE = 64;
    
    MemoryPool();
    ~MemoryPool();
    
    // Get or create a pool for specific object size
    static std::shared_ptr<MemoryPool> getPool(size_t objectSize);
    
    // Allocate from pool
    void* allocate(size_t size);
    
    // Deallocate to pool
    void deallocate(void* ptr, size_t size);
    
    // Get pool statistics
    size_t getAllocationCount() const { return totalAllocations; }
    size_t getDeallocationCount() const { return totalDeallocations; }
    size_t getCurrentUsage() const { return currentUsage; }
    
private:
    struct PoolBlock {
        void* startPtr;
        size_t capacity;
        size_t used;
        PoolBlock* next;
        
        PoolBlock(size_t cap) : startPtr(nullptr), capacity(cap), used(0), next(nullptr) {
            startPtr = malloc(cap);
        }
        
        ~PoolBlock() {
            free(startPtr);
        }
    };
    
    std::vector<std::unique_ptr<PoolBlock>> blocks;
    std::atomic<size_t> totalAllocations{0};
    std::atomic<size_t> totalDeallocations{0};
    std::atomic<size_t> currentUsage{0};
    std::mutex mutex;
};

// Smart pointer with custom deallocation
template<typename T>
using PoolPtr = std::unique_ptr<T, std::function<void(T*)>>;

// Cache-friendly data structures
class CacheFriendlyArray {
public:
    CacheFriendlyArray(size_t capacity = 0);
    ~CacheFriendlyArray();
    
    // Reserve space (with cache-line padding)
    void reserve(size_t capacity);
    
    // Add element
    void push_back(const T& value);
    void push_back(T&& value);
    
    // Access elements
    T& operator[](size_t index);
    const T& operator[](size_t index) const;
    
    // Iteration
    T* begin();
    const T* begin() const;
    T* end();
    const T* end() const;
    
    // Size
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    void clear();
    
    // Performance optimized methods
    void bulk_insert(const std::vector<T>& values);
    void compact(); // Remove unused capacity
    
private:
    struct AlignedBlock {
        alignas(64) char data[64];
        AlignedBlock* next;
        
        AlignedBlock(size_t elementCount) {
            next = nullptr;
        }
    };
    
    std::vector<AlignedBlock*> blocks;
    size_t size_;
    size_t capacity_;
    size_t elementSize_;
};

// Object pool for frequently created objects
template<typename T>
class ObjectPool {
public:
    ObjectPool(size_t initialSize = 100);
    ~ObjectPool();
    
    // Get object from pool
    T* acquire();
    
    // Return object to pool
    void release(T* obj);
    
    // Get pool statistics
    size_t getAvailableCount() const { return availableObjects.size(); }
    size_t getTotalAllocated() const { return totalAllocated; }
    
private:
    std::vector<std::unique_ptr<T>> pool;
    std::vector<T*> availableObjects;
    std::mutex mutex;
    size_t totalAllocated{0};
};

// Allocator that tracks memory usage
class TrackingAllocator {
public:
    TrackingAllocator();
    ~TrackingAllocator();
    
    void* allocate(size_t size);
    void deallocate(void* ptr, size_t size);
    
    // Get statistics
    size_t getTotalAllocated() const { return totalAllocated; }
    size_t getPeakAllocated() const { return peakAllocated; }
    size_t getCurrentUsage() const { return currentUsage; }
    double getFragmentationRatio() const;
    
    // Reset statistics
    void resetStats();
    
    // Set allocation limits
    void setMaxAllocations(size_t max) { maxAllocations = max; }
    
private:
    struct AllocationInfo {
        void* ptr;
        size_t size;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    std::vector<AllocationInfo> allocations;
    std::mutex mutex;
    size_t totalAllocated{0};
    size_t peakAllocated{0};
    size_t currentUsage{0};
    size_t maxAllocations{std::numeric_limits<size_t>::max()};
    size_t totalDeallocations{0};
};

// Performance timer
class PerformanceTimer {
public:
    PerformanceTimer();
    ~PerformanceTimer();
    
    void start();
    void stop();
    
    double getElapsedTime() const; // microseconds
    double getElapsedTimeMs() const { return getElapsedTime() / 1000.0; }
    
    // Reset timer
    void reset();
    
    // Check if timer is running
    bool isRunning() const { return running; }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::duration<double> elapsedTime;
    bool running;
};

// Performance profiler
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Start profiling a function
    void startProfile(const std::string& name);
    
    // Stop profiling and record result
    void stopProfile(const std::string& name);
    
    // Display profile results
    void displayProfile() const;
    
    // Clear profile results
    void clearProfile();
    
    // Get profile for specific function
    const std::vector<double>& getProfile(const std::string& name) const;
    
    // Calculate average and std dev
    double getAverageTime(const std::string& name) const;
    double getStdDevTime(const std::string& name) const;
    
private:
    struct ProfileData {
        std::vector<double> times;
        std::chrono::high_resolution_clock::time_point startTime;
        double totalTime{0.0};
        size_t callCount{0};
    };
    
    std::unordered_map<std::string, ProfileData> profiles;
    std::mutex mutex;
};

// Lock-free queue for high-performance communication
template<typename T>
class LockFreeQueue {
public:
    LockFreeQueue(size_t capacity = 1024);
    ~LockFreeQueue();
    
    // Push item to queue
    bool push(const T& item);
    
    // Pop item from queue
    bool pop(T& item);
    
    // Try push (non-blocking)
    bool tryPush(const T& item);
    
    // Try pop (non-blocking)
    bool tryPop(T& item);
    
    // Get current size
    size_t size() const;
    
    // Check if queue is empty
    bool empty() const;
    
    // Clear queue
    void clear();
    
private:
    struct Node {
        T data;
        std::atomic<Node*> next;
        
        Node(const T& item) : data(item), next(nullptr) {}
    };
    
    std::atomic<Node*> head;
    std::atomic<Node*> tail;
    size_t capacity;
    std::atomic<size_t> count{0};
    std::mutex mutex; // For clear operation
};

// SIMD-optimized operations
namespace simd {
    // Vectorized sum
    float vectorSum(const float* data, size_t count);
    
    // Vectorized dot product
    float vectorDotProduct(const float* a, const float* b, size_t count);
    
    // Vectorized multiplication
    void vectorMultiply(float* result, const float* a, const float* b, size_t count);
    
    // Vectorized addition
    void vectorAdd(float* result, const float* a, const float* b, size_t count);
    
    // Check SIMD availability
    bool hasSSE() noexcept;
    bool hasAVX() noexcept;
    bool hasAVX512() noexcept;
}

} // namespace performance

// Convenience types and aliases
using PerformanceMemoryPool = performance::MemoryPool;
using PerformanceArray = performance::CacheFriendlyArray<float>;
using PerformanceProfiler = performance::PerformanceProfiler;

// Global performance utilities
namespace perf {
    // Get global memory pool
    PerformanceMemoryPool& getMemoryPool();
    
    // Get global performance profiler
    PerformanceProfiler& getProfiler();
    
    // Start/stop global profiling
    void startProfile(const std::string& name);
    void stopProfile(const std::string& name);
    
    // Display performance summary
    void displayPerformanceSummary();
    
    // Reset all performance data
    void resetPerformance();
}

} // namespace nlm
