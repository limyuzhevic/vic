// Performance Optimization Header
// Common optimization macros and utilities for NLM

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

// Performance timing utilities
class PerformanceTimer {
public:
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    
    PerformanceTimer() {
        start = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceTimer() {
        end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "  Performance: " << duration.count() << "ms" << std::endl;
    }
    
    double elapsed() const {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() / 1000.0;
    }
};

// Optimized vector operations
namespace VectorOps {
    // Parallel-safe version of accumulate (placeholder for future SIMD)
    template<typename T>
    T safe_accumulate(const std::vector<T>& v) {
        return std::accumulate(v.begin(), v.end(), T());
    }
    
    // Cache-friendly access for large vectors
    template<typename T>
    void cache_friendly_sort(std::vector<T>& v) {
        // Simple std::sort - cache friendly
        std::sort(v.begin(), v.end());
    }
    
    // Memory efficient range operations
    template<typename T>
    size_t count_if(const std::vector<T>& v, const std::function<bool(const T&)>& pred) {
        size_t count = 0;
        for (const auto& item : v) {
            if (pred(item)) count++;
        }
        return count;
    }
}

// Memory pool utilities for small objects
class ObjectPool {
public:
    std::vector<void*> pool;
    size_t capacity;
    
    ObjectPool(size_t initialCapacity = 1024) : capacity(initialCapacity) {
        pool.reserve(capacity);
    }
    
    void* allocate() {
        if (pool.empty()) {
            // Grow pool if needed
            pool.resize(pool.size() + capacity);
        }
        return pool.back();
    }
    
    void deallocate(void* ptr) {
        // In real implementation, would track allocated pointers
        // For this demo, just clear
        pool.clear();
    }
};

// Optimized logging utilities
namespace LogOps {
    // Level-based logging with minimal overhead
    enum class LogLevel {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };
    
    // Thread-safe logging (placeholder)
    void log(LogLevel level, const std::string& message);
    
    // Performance logging
    class PerfLog {
    public:
        std::string name;
        std::chrono::high_resolution_clock::time_point start;
        
        PerfLog(const std::string& n) : name(n) {
            start = std::chrono::high_resolution_clock::now();
        }
        
        ~PerfLog() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "[PERF] " << name << ": " << duration.count() << "μs" << std::endl;
        }
    };
}

// Memory management optimizations
namespace MemoryOps {
    // Alignment-aware allocation
    template<typename T>
    T* aligned_allocate(size_t count, size_t alignment = 64) {
        void* ptr = std::aligned_alloc(alignment, sizeof(T) * count);
        return static_cast<T*>(ptr);
    }
    
    template<typename T>
    void aligned_deallocate(T* ptr) {
        std::free(ptr);
    }
    
    // Efficient clearing for large containers
    template<typename Container>
    void fast_clear(Container& c) {
        typename Container::iterator it = c.begin();
        typename Container::iterator end = c.end();
        for (; it != end; ++it) {
            it->~decltype(*it)();
        }
        c.clear();
    }
}

// SIMD utilities (placeholders for future implementation)
namespace SIMD {
    // Placeholder for SIMD optimizations
    // Will be implemented as needed for specific optimizations
    
    template<typename T>
    void copy_simd(const T* src, T* dst, size_t n) {
        // Standard copy for now
        std::copy(src, src + n, dst);
    }
    
    template<typename T>
    void add_simd(const T* a, const T* b, T* c, size_t n) {
        // Standard add for now
        for (size_t i = 0; i < n; ++i) {
            c[i] = a[i] + b[i];
        }
    }
}

// Parallel processing utilities (placeholders)
namespace Parallel {
    // Placeholder for parallel processing
    // Will be implemented with OpenMP or std::thread as needed
    
    template<typename F>
    void parallel_for(size_t start, size_t end, size_t numThreads, F func) {
        // Single-threaded implementation for now
        for (size_t i = start; i < end; ++i) {
            func(i);
        }
    }
    
    template<typename Iterator, typename F>
    void parallel_for_each(Iterator begin, Iterator end, F func) {
        // Single-threaded implementation for now
        for (auto it = begin; it != end; ++it) {
            func(*it);
        }
    }
}

} // namespace nlm
