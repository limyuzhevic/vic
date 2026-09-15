#ifndef NLM_PERFORMANCE_HPP
#define NLM_PERFORMANCE_HPP

#include <chrono>

namespace nlm {

// Performance monitoring and optimization utilities
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    // Enable moving
    PerformanceMonitor(PerformanceMonitor&&) noexcept;
    PerformanceMonitor& operator=(PerformanceMonitor&&) noexcept;
    
    // Start/stop timing for specific operations
    void startTiming(const std::string& operation);
    double stopTiming(const std::string& operation);
    
    // Get average time for operation
    double getAverageTime(const std::string& operation) const;
    
    // Get number of times operation was timed
    size_t getOperationCount(const std::string& operation) const;
    
    // Reset all statistics
    void reset();
    
    // Print performance report
    void printReport() const;
    
    // Check if performance monitoring is enabled
    bool isEnabled() const;
    
    // Enable/disable performance monitoring
    void setEnabled(bool enabled);
    
private:
    struct Impl;
    Impl* pImpl;
};

// SIMD optimization utilities
class SIMDVectorizer {
public:
    SIMDVectorizer();
    ~SIMDVectorizer();
    
    // Disable copying
    SIMDVectorizer(const SIMDVectorizer&) = delete;
    SIMDVectorizer& operator=(const SIMDVectorizer&) = delete;
    
    // Enable moving
    SIMDVectorizer(SIMDVectorizer&&) noexcept;
    SIMDVectorizer& operator=(SIMDVectorizer&&) noexcept;
    
    // Check if SIMD is available
    bool isAvailable() const;
    
    // Get SIMD capabilities
    std::string getCapabilities() const;
    
    // Vector operations (when SIMD available)
    template<typename T>
    void vectorAdd(const T* a, const T* b, T* result, size_t size);
    
    template<typename T>
    void vectorMultiply(const T* a, const T* b, T* result, size_t size);
    
    // Memory operations
    void prefetch(const void* ptr, int hint);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Parallel processing utilities
class ParallelProcessor {
public:
    ParallelProcessor();
    ~ParallelProcessor();
    
    // Disable copying
    ParallelProcessor(const ParallelProcessor&) = delete;
    ParallelProcessor& operator=(const ParallelProcessor&) = delete;
    
    // Enable moving
    ParallelProcessor(ParallelProcessor&&) noexcept;
    ParallelProcessor& operator=(ParallelProcessor&&) noexcept;
    
    // Get number of available threads
    static size_t getThreadCount();
    
    // Set number of threads to use
    static void setThreadCount(size_t count);
    
    // Parallel loop execution
    template<typename Func>
    void parallelFor(size_t start, size_t end, size_t grainSize, const Func& func);
    
    // Parallel reduction
    template<typename T, typename BinaryOp>
    T parallelReduce(const std::vector<T>& values, const BinaryOp& op);
    
    // Check if parallel processing is available
    bool isAvailable() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_HPP