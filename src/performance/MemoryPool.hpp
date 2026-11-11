#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include <atomic>
#include <algorithm>
#include <unordered_map>
#include <string>

namespace nlm {

/**
 * Memory pool for efficient allocation of small objects
 * 
 * Memory pools are particularly useful in neural simulations where many
 * small objects (like spikes, synapses, neurons) are frequently allocated
 * and deallocated. By reusing pre-allocated memory blocks, pools can
 * significantly reduce memory fragmentation and improve cache locality.
 */
class MemoryPool {
public:
    // Pool statistics
    struct Statistics {
        uint64_t allocations = 0;
        uint64_t deallocations = 0;
        uint64_t currentSize = 0;
        uint64_t peakSize = 0;
        uint64_t bytesAllocated = 0;
        double fragmentation = 0.0;
    };

    MemoryPool();
    ~MemoryPool();

    // Disable copying and moving
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&&) = delete;
    MemoryPool& operator=(MemoryPool&&) = delete;

    /**
     * Allocate memory for an object
     * 
     * @param size Number of bytes to allocate
     * @param alignment Alignment requirement (default: 8 bytes)
     * @return Pointer to allocated memory
     */
    void* allocate(size_t size, size_t alignment = 8);

    /**
     * Deallocate memory
     * 
     * @param ptr Pointer to memory to deallocate
     * @param size Size of the memory block
     */
    void deallocate(void* ptr, size_t size);

    /**
     * Allocate and construct an object
     * 
     * @tparam T Type of object to allocate
     * @tparam Args Constructor arguments
     * @param args Arguments to pass to constructor
     * @return Pointer to constructed object
     */
    template<typename T, typename... Args>
    T* makeObject(Args&&... args) {
        void* memory = allocate(sizeof(T), alignof(T));
        if (!memory) return nullptr;
        try {
            T* obj = new (memory) T(std::forward<Args>(args)...);
            return obj;
        } catch (...) {
            deallocate(memory, sizeof(T));
            throw;
        }
    }

    /**
     * Destroy and deallocate an object
     * 
     * @tparam T Type of object
     * @param ptr Pointer to object to destroy
     */
    template<typename T>
    void destroyObject(T* ptr) {
        if (!ptr) return;
        ptr->~T();
        deallocate(ptr, sizeof(T));
    }

    /**
     * Allocate memory for a vector
     * 
     * @tparam T Element type
     * @param count Number of elements
     * @return Pointer to allocated vector memory
     */
    template<typename T>
    T* allocateVector(size_t count) {
        return static_cast<T*>(allocate(count * sizeof(T), alignof(T)));
    }

    /**
     * Deallocate vector memory
     * 
     * @tparam T Element type
     * @param ptr Pointer to vector memory
     * @param count Number of elements
     */
    template<typename T>
    void deallocateVector(T* ptr, size_t count) {
        deallocate(ptr, count * sizeof(T));
    }

    /**
     * Get memory usage statistics
     * 
     * @return Pool statistics
     */
    Statistics getStatistics() const;

    /**
     * Reset pool to free all allocated memory
     */
    void reset();

    /**
     * Set growth factor
     * 
     * @param factor Growth factor (default: 2.0)
     */
    void setGrowthFactor(double factor);

private:
    struct Block {
        void* ptr;
        size_t size;
        Block* next;
    };

    // Memory blocks for different sizes
    std::vector<Block*> blocks;
    std::unordered_map<size_t, std::vector<void*>> freeBlocks;
    std::atomic<size_t> currentSize;
    double growthFactor;
    size_t alignment;

    void* allocateBlock(size_t size);
    void deallocateBlock(void* ptr);
    Block* createBlock(size_t size);
    void destroyBlock(Block* block);
};

/**
 * Typed memory pool for specific types
 * 
 * This is more efficient than the generic MemoryPool when allocating
 * many objects of the same type.
 */
template<typename T>
class TypedMemoryPool {
public:
    TypedMemoryPool() = default;
    ~TypedMemoryPool() { clear(); }

    // Disable copying and moving
    TypedMemoryPool(const TypedMemoryPool&) = delete;
    TypedMemoryPool& operator=(const TypedMemoryPool&) = delete;

    /**
     * Allocate an object
     * 
     * @tparam Args Constructor arguments
     * @param args Arguments to pass to constructor
     * @return Pointer to allocated object
     */
    template<typename... Args>
    T* allocate(Args&&... args) {
        if (!freeList.empty()) {
            T* obj = freeList.back();
            freeList.pop_back();
            new (obj) T(std::forward<Args>(args)...);
            ++statistics.allocations;
            statistics.currentSize++;
            if (statistics.currentSize > statistics.peakSize) {
                statistics.peakSize = statistics.currentSize;
            }
            return obj;
        }

        // Allocate new block
        void* memory = allocator.allocate(sizeof(T), alignof(T));
        T* obj = new (memory) T(std::forward<Args>(args)...);
        ++statistics.allocations;
        statistics.currentSize++;
        if (statistics.currentSize > statistics.peakSize) {
            statistics.peakSize = statistics.currentSize;
        }
        return obj;
    }

    /**
     * Deallocate an object
     * 
     * @param ptr Pointer to object to deallocate
     */
    void deallocate(T* ptr) {
        if (!ptr) return;
        ptr->~T();
        freeList.push_back(ptr);
        ++statistics.deallocations;
        statistics.currentSize--;
    }

    /**
     * Get statistics
     * 
     * @return Pool statistics
     */
    auto getStatistics() const { return statistics; }

    /**
     * Clear all allocated objects
     * 
     * Note: This does not return memory to the system, just marks
     * all objects as free for reuse.
     */
    void clear() {
        // Destroy all objects in free list
        for (T* obj : freeList) {
            obj->~T();
            allocator.deallocate(obj, sizeof(T));
        }
        freeList.clear();
        statistics.currentSize = 0;
    }

    /**
     * Check if pool is empty
     * 
     * @return true if pool is empty
     */
    bool empty() const { return freeList.empty(); }

    /**
     * Get number of free objects
     * 
     * @return Number of free objects
     */
    size_t size() const { return freeList.size(); }

private:
    std::vector<T*> freeList;
    MemoryPool allocator;
    Statistics statistics;
};

/**
 * Memory allocator for performance-critical systems
 * 
 * This allocator uses memory pools to reduce memory allocation overhead
 * and improve cache locality. It's particularly useful for real-time
 * neural simulations where predictable memory access patterns are important.
 */
class PerformanceMemoryAllocator {
public:
    PerformanceMemoryAllocator() = default;
    ~PerformanceMemoryAllocator() = default;

    /**
     * Allocate memory
     * 
     * @param size Number of bytes to allocate
     * @return Pointer to allocated memory
     */
    void* allocate(size_t size) {
        return memoryPool.allocate(size);
    }

    /**
     * Deallocate memory
     * 
     * @param ptr Pointer to memory to deallocate
     * @param size Size of memory block
     */
    void deallocate(void* ptr, size_t size) {
        memoryPool.deallocate(ptr, size);
    }

    /**
     * Get memory pool statistics
     * 
     * @return Pool statistics
     */
    auto getStatistics() const { return memoryPool.getStatistics(); }

    /**
     * Reset memory pool
     */
    void reset() {
        memoryPool.reset();
    }

private:
    MemoryPool memoryPool;
};

/**
 * SIMD-optimized math functions for neural computation
 * 
 * These functions use SIMD instructions to accelerate neural computation
 * operations like matrix multiplication, vector operations, and neural
 * network forward passes.
 */
namespace simd {

/**
 * SIMD-optimized vector addition
 * 
 * @param a First vector
 * @param b Second vector
 * @param result Output vector
 * @param size Number of elements
 */
void add(const float* a, const float* b, float* result, size_t size);

/**
 * SIMD-optimized vector multiplication
 * 
 * @param a First vector
 * @param b Second vector
 * @param result Output vector
 * @param size Number of elements
 */
void multiply(const float* a, const float* b, float* result, size_t size);

/**
 * SIMD-optimized vector sigmoid function
 * 
 * @param input Input vector
 * @param output Output vector
 * @param size Number of elements
 */
void sigmoid(const float* input, float* output, size_t size);

/**
 * SIMD-optimized vector ReLU function
 * 
 * @param input Input vector
 * @param output Output vector
 * @param size Number of elements
 */
void relu(const float* input, float* output, size_t size);

/**
 * SIMD-optimized vector softmax function
 * 
 * @param input Input vector
 * @param output Output vector
 * @param size Number of elements
 */
void softmax(const float* input, float* output, size_t size);

/**
 * SIMD-optimized matrix multiplication
 * 
 * @param a First matrix (m x n)
 * @param b Second matrix (n x p)
 * @param result Output matrix (m x p)
 * @param m Number of rows in matrix A
 * @param n Number of columns in matrix A / rows in matrix B
 * @param p Number of columns in matrix B
 */
void matrixMultiply(const float* a, const float* b, float* result,
                   size_t m, size_t n, size_t p);

/**
 * SIMD-optimized element-wise vector operation
 * 
 * @param a First vector
 * @param b Second vector
 * @param result Output vector
 * @param size Number of elements
 * @param op Operation to perform (0: add, 1: multiply, 2: max, 3: min)
 */
void elementWise(const float* a, const float* b, float* result,
                size_t size, int op);

} // namespace simd

/**
 * Performance monitor for tracking system performance
 * 
 * This class tracks key performance metrics like memory usage,
 * CPU utilization, and computational throughput to help optimize
 * neural simulation performance.
 */
class PerformanceMonitor {
public:
    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;

    /**
     * Start performance monitoring
     * 
     * @param name Name of the performance measurement
     */
    void start(const std::string& name);

    /**
     * Stop performance monitoring
     * 
     * @param name Name of the performance measurement
     * @return Elapsed time in milliseconds
     */
    double stop(const std::string& name);

    /**
     * Record a memory allocation
     * 
     * @param size Number of bytes allocated
     */
    void recordAllocation(size_t size);

    /**
     * Record a memory deallocation
     * 
     * @param size Number of bytes deallocated
     */
    void recordDeallocation(size_t size);

    /**
     * Get performance statistics
     * 
     * @return Performance statistics
     */
    struct Statistics {
        std::unordered_map<std::string, std::vector<double>> timings;
        uint64_t totalAllocations = 0;
        uint64_t totalDeallocations = 0;
        uint64_t currentMemoryUsage = 0;
        uint64_t peakMemoryUsage = 0;
        uint64_t totalOperations = 0;
    };

    Statistics getStatistics() const;

    /**
     * Generate performance report
     * 
     * @return Performance report as string
     */
    std::string generateReport() const;

    /**
     * Clear performance statistics
     */
    void clear();

private:
    struct Measurement {
        std::string name;
        double startTime;
        double endTime;
        bool active;
    };

    std::vector<Measurement> measurements;
    Statistics stats;
};

} // namespace nlm