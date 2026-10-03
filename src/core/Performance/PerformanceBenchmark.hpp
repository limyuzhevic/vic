// Performance optimization utilities and benchmarks for NLM
// Provides performance measurement, optimization tools, and benchmarking capabilities

#ifndef NLM_PERFORMANCE_HPP
#define NLM_PERFORMANCE_HPP

#include "core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include <numeric>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <atomic>
#include <future>

namespace nlm {

// Performance measurement utilities
class PerformanceTimer {
public:
    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<double>;
    
    PerformanceTimer() : startTime_(Clock::now()), pausedTime_(0.0), isRunning_(true) {}
    
    // Start or resume timer
    void start() {
        if (!isRunning_) {
            pausedTime_ = 0.0;
            isRunning_ = true;
        }
    }
    
    // Pause timer
    void pause() {
        if (isRunning_) {
            auto now = Clock::now();
            pausedTime_ += std::chrono::duration<double>(now - startTime_).count();
            isRunning_ = false;
        }
    }
    
    // Stop timer
    void stop() {
        if (isRunning_) {
            auto now = Clock::now();
            totalTime_ += std::chrono::duration<double>(now - startTime_).count() - pausedTime_;
            isRunning_ = false;
        }
    }
    
    // Reset timer
    void reset() {
        startTime_ = Clock::now();
        totalTime_ = 0.0;
        pausedTime_ = 0.0;
        isRunning_ = true;
    }
    
    // Get elapsed time
    double elapsed() const {
        if (isRunning_) {
            return totalTime_ + (std::chrono::duration<double>(Clock::now() - startTime_).count() - pausedTime_);
        } else {
            return totalTime_;
        }
    }
    
    // Get elapsed time in milliseconds
    double elapsedMs() const {
        return elapsed() * 1000.0;
    }
    
    // Get elapsed time in microseconds
    double elapsedUs() const {
        return elapsed() * 1000000.0;
    }
    
    // Check if timer is running
    bool isRunning() const {
        return isRunning_;
    }
    
    // Convert elapsed time to string
    std::string toString() const {
        double elapsed = this->elapsed();
        if (elapsed >= 60.0) {
            return std::to_string(elapsed / 60.0) + "min";
        } else if (elapsed >= 1.0) {
            return std::to_string(elapsed) + "s";
        } else if (elapsed >= 0.001) {
            return std::to_string(elapsed * 1000.0) + "ms";
        } else {
            return std::to_string(elapsed * 1000000.0) + "μs";
        }
    }
    
private:
    Clock::time_point startTime_;
    double totalTime_ = 0.0;
    double pausedTime_ = 0.0;
    bool isRunning_ = true;
};

// Benchmark results structure
struct BenchmarkResult {
    std::string name;
    std::string description;
    double timeSeconds;
    double timeMilliseconds;
    double timeMicroseconds;
    size_t iterations;
    double operationsPerSecond;
    double memoryUsageBytes;
    std::map<std::string, std::string> metadata;
    std::vector<double> rawTimes;
    
    BenchmarkResult(const std::string& name, const std::string& desc = "")
        : name(name), description(desc), timeSeconds(0.0), timeMilliseconds(0.0), 
          timeMicroseconds(0.0), iterations(0), operationsPerSecond(0.0), memoryUsageBytes(0.0) {}
    
    // Calculate statistics
    double getMeanTime() const {
        if (rawTimes.empty()) return 0.0;
        return std::accumulate(rawTimes.begin(), rawTimes.end(), 0.0) / rawTimes.size();
    }
    
    double getMinTime() const {
        if (rawTimes.empty()) return 0.0;
        return *std::min_element(rawTimes.begin(), rawTimes.end());
    }
    
    double getMaxTime() const {
        if (rawTimes.empty()) return 0.0;
        return *std::max_element(rawTimes.begin(), rawTimes.end());
    }
    
    double getStandardDeviation() const {
        if (rawTimes.size() <= 1) return 0.0;
        
        double mean = getMeanTime();
        double sumSquared = 0.0;
        for (double time : rawTimes) {
            double diff = time - mean;
            sumSquared += diff * diff;
        }
        
        return std::sqrt(sumSquared / (rawTimes.size() - 1));
    }
    
    // Format for display
    std::string toString() const {
        std::ostringstream oss;
        oss << "Benchmark: " << name << "\n";
        oss << "Description: " << description << "\n";
        oss << "Iterations: " << iterations << "\n";
        oss << "Total time: " << std::fixed << std::setprecision(6) << timeSeconds << "s\n";
        oss << "Mean time: " << std::fixed << std::setprecision(6) << getMeanTime() << "s\n";
        oss << "Min time: " << std::fixed << std::setprecision(6) << getMinTime() << "s\n";
        oss << "Max time: " << std::fixed << std::setprecision(6) << getMaxTime() << "s\n";
        oss << "Std dev: " << std::fixed << std::setprecision(6) << getStandardDeviation() << "s\n";
        oss << "Operations/sec: " << std::fixed << std::setprecision(2) << operationsPerSecond << "\n";
        oss << "Memory usage: " << memoryUsageBytes << " bytes\n";
        
        return oss.str();
    }
    
    // Format for CSV
    std::string toCSV() const {
        std::ostringstream oss;
        oss << name << "," << description << "," << iterations << ","
            << timeSeconds << "," << getMeanTime() << "," << getMinTime() << "," << getMaxTime() << "," << getStandardDeviation() << ","
            << operationsPerSecond << "," << memoryUsageBytes;
        return oss.str();
    }
};

// Benchmark suite for running multiple benchmarks
class BenchmarkSuite {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    BenchmarkSuite();
    ~BenchmarkSuite();
    
    // Add benchmark
    void addBenchmark(const std::string& name, const std::string& description,
                     std::function<void()> benchmark,
                     size_t iterations = 100);
    
    // Run all benchmarks
    std::vector<BenchmarkResult> runAll();
    
    // Run specific benchmark
    BenchmarkResult runBenchmark(const std::string& name);
    
    // Get benchmark names
    std::vector<std::string> getBenchmarkNames() const;
    
    // Get benchmark count
    size_t getBenchmarkCount() const;
    
    // Clear all benchmarks
    void clear();
    
    // Set number of iterations for all benchmarks
    void setIterations(size_t iterations);
    
    // Set number of iterations for specific benchmark
    void setIterations(const std::string& name, size_t iterations);
    
    // Generate report
    std::string generateReport(const std::vector<BenchmarkResult>& results) const;
    
    // Save results to file
    bool saveResults(const std::vector<BenchmarkResult>& results, const std::string& filepath) const;
    
    // Load results from file
    bool loadResults(std::vector<BenchmarkResult>& results, const std::string& filepath) const;
    
    // Compare with baseline
    struct ComparisonResult {
        std::string benchmarkName;
        double improvementFactor;
        double timeDifference;
        bool isFaster;
        std::string status;
    };
    
    ComparisonResult compareWithBaseline(const BenchmarkResult& baseline, const BenchmarkResult& current) const;
    
    // Profile memory usage
    static size_t getMemoryUsage();
    
    // Get system information
    static std::string getSystemInfo();
};n

// Optimization analysis and recommendations
class OptimizationAnalyzer {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    OptimizationAnalyzer();
    ~OptimizationAnalyzer();
    
    // Analyze performance data
    std::string analyzePerformance(const std::vector<BenchmarkResult>& results) const;
    
    // Generate optimization recommendations
    std::vector<std::string> generateRecommendations(const std::vector<BenchmarkResult>& results) const;
    
    // Identify bottlenecks
    std::vector<std::string> identifyBottlenecks(const std::vector<BenchmarkResult>& results) const;
    
    // Calculate performance ratios
    double calculateSpeedup(const BenchmarkResult& baseline, const BenchmarkResult& optimized) const;
    
    // Analyze memory usage
    std::string analyzeMemoryUsage(const std::vector<BenchmarkResult>& results) const;
    
    // Profile cache performance (simulated)
    struct CacheMetrics {
        double l1CacheMisses = 0.0;
        double l2CacheMisses = 0.0;
        double l3CacheMisses = 0.0;
        double memoryBandwidth = 0.0;
        double cacheEfficiency = 0.0;
    };
    
    CacheMetrics profileCachePerformance() const;
    
    // Analyze CPU utilization
    struct CPUMetrics {
        double userTime = 0.0;
        double systemTime = 0.0;
        double idleTime = 0.0;
        double utilization = 0.0;
    };
    
    CPUMetrics analyzeCPUUtilization() const;
    
    // Generate optimization report
    std::string generateOptimizationReport(const std::vector<BenchmarkResult>& results) const;
};n

// Thread pool for parallel execution
class ThreadPool {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    ThreadPool(size_t threadCount = std::thread::hardware_concurrency());
    ~ThreadPool();
    
    // Submit task
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;
    
    // Wait for all tasks to complete
    void wait() const;
    
    // Stop thread pool
    void stop();
    
    // Get thread count
    size_t getThreadCount() const;
    
    // Check if thread pool is running
    bool isRunning() const;
};n

// SIMD optimization utilities
class SIMDUtils {
public:
    // Check SIMD support
    static bool hasSSE2();
    static bool hasSSE3();
    static bool hasSSSE3();
    static bool hasSSE4();
    static bool hasAVX();
    static bool hasAVX2();
    static bool hasAVX512();
    static bool hasFMA();
    
    // Optimized vector operations (examples)
    static void vectorAddSSE2(const float* a, const float* b, float* c, size_t n);
    static void vectorAddAVX(const float* a, const float* b, float* c, size_t n);
    static void vectorMultiplySSE2(const float* a, const float* b, float* c, size_t n);
    static void vectorMultiplyAVX(const float* a, const float* b, float* c, size_t n);
    
    // Get best available SIMD level
    static std::string getBestSIMDLevel();
    
    // Performance test for SIMD operations
    static BenchmarkResult benchmarkSIMDAdd(size_t size = 1000000);
    
    // Auto-dispatch based on available SIMD
    template<typename Func>
    static void autoDispatch(Func&& func, size_t size);
};n

// Memory optimization utilities
class MemoryOptimizer {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    MemoryOptimizer();
    ~MemoryOptimizer();
    
    // Allocate aligned memory
    static void* allocateAligned(size_t size, size_t alignment = 64);
    
    // Free aligned memory
    static void freeAligned(void* ptr);
    
    // Allocate memory pool
    static void* allocatePool(size_t poolSize, size_t elementSize);
    
    // Free memory pool
    static void freePool(void* pool);
    
    // Get optimal cache line size
    static size_t getCacheLineSize();
    
    // Get optimal memory alignment
    static size_t getOptimalAlignment();
    
    // Prefetch data into cache
    static void prefetch(const void* ptr, size_t offset = 0);
    
    // Memory bandwidth test
    static BenchmarkResult benchmarkMemoryBandwidth(size_t size = 100 * 1024 * 1024); // 100MB
    
    // Cache performance test
    static BenchmarkResult benchmarkCachePerformance();
};n

// Branch prediction utilities
class BranchPredictor {
public:
    // Branch prediction benchmark
    static BenchmarkResult benchmarkBranchPrediction(size_t iterations = 100000000);
    
    // Conditional branch benchmark
    static BenchmarkResult benchmarkConditionalBranch(size_t iterations = 100000000);
    
    // Loop branch benchmark
    static BenchmarkResult benchmarkLoopBranch(size_t iterations = 100000000);
    
    // Predictable vs unpredictable branch benchmark
    static BenchmarkResult benchmarkBranchPredictability(size_t iterations = 100000000);
};n

// Floating-point optimization utilities
class FPOptimizer {
public:
    // Floating-point addition benchmark
    static BenchmarkResult benchmarkFPAddition(size_t iterations = 10000000);
    
    // Floating-point multiplication benchmark
    static BenchmarkResult benchmarkFPMultiplication(size_t iterations = 10000000);
    
    // Floating-point division benchmark
    static BenchmarkResult benchmarkFPDivision(size_t iterations = 10000000);
    
    // Trigonometric functions benchmark
    static BenchmarkResult benchmarkTrigonometric(size_t iterations = 1000000);
    
    // Square root benchmark
    static BenchmarkResult benchmarkSquareRoot(size_t iterations = 10000000);
};n

// Concurrent execution utilities
class ConcurrentUtils {
public:
    // Parallel reduce benchmark
    template<typename Iterator, typename T>
    static BenchmarkResult benchmarkParallelReduce(Iterator begin, Iterator end, T init);
    
    // Parallel sort benchmark
    template<typename Iterator>
    static BenchmarkResult benchmarkParallelSort(Iterator begin, Iterator end);
    
    // Parallel map benchmark
    template<typename Iterator, typename Function>
    static BenchmarkResult benchmarkParallelMap(Iterator begin, Iterator end, Function func);
    
    // Concurrent container benchmark
    static BenchmarkResult benchmarkConcurrentContainer(size_t threadCount = 8, size_t operations = 100000);
};n

// Comprehensive benchmarking system
class BenchmarkManager {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    BenchmarkManager();
    ~BenchmarkManager();
    
    // Initialize benchmark manager
    void initialize();
    
    // Add benchmark suite
    void addSuite(const std::string& name, const BenchmarkSuite& suite);
    
    // Get benchmark suites
    std::vector<std::string> getSuiteNames() const;
    
    // Run all benchmarks
    std::vector<BenchmarkResult> runAllBenchmarks();
    
    // Run specific suite
    std::vector<BenchmarkResult> runSuite(const std::string& name);
    
    // Generate comprehensive report
    std::string generateComprehensiveReport(const std::vector<BenchmarkResult>& allResults) const;
    
    // Save results to file
    bool saveResults(const std::vector<BenchmarkResult>& results, const std::string& filepath) const;
    
    // Load results from file
    bool loadResults(std::vector<BenchmarkResult>& results, const std::string& filepath) const;
    
    // Compare with historical results
    std::string compareWithHistory(const std::vector<BenchmarkResult>& currentResults, 
                                  const std::string& historyFile) const;
    
    // Get performance statistics
    struct PerformanceStats {
        double minTime = 0.0;
        double maxTime = 0.0;
        double meanTime = 0.0;
        double medianTime = 0.0;
        double stddevTime = 0.0;
        size_t totalIterations = 0;
        double throughput = 0.0;
        double efficiency = 0.0;
    };
    
    PerformanceStats calculatePerformanceStats(const std::vector<BenchmarkResult>& results) const;
    
    // Generate optimization recommendations
    std::vector<std::string> generateOptimizationRecommendations(const std::vector<BenchmarkResult>& results) const;
    
    // Get system performance profile
    std::string getSystemPerformanceProfile() const;
};

// Convenience functions
namespace Performance {
    // Simple timer
    class ScopedTimer {
    public:
        ScopedTimer(const std::string& name, bool record = true) : name_(name), record_(record) {
            if (record_) {
                start_ = std::chrono::high_resolution_clock::now();
            }
        }
        
        ~ScopedTimer() {
            if (record_) {
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start_).count();
                NLM_LOG_DEBUG("Timer '" + name_ + "': " + std::to_string(duration) + "s");
            }
        }
        
    private:
        std::string name_;
        bool record_;
        std::chrono::high_resolution_clock::time_point start_;
    };
    
    // Timing macros
    #define PERF_TIMER(name) nlm::Performance::ScopedTimer timer_##__LINE__(name)
    #define PERF_TIMER_START(name) nlm::Performance::ScopedTimer timer_##__LINE__(name, false)
    
    // Benchmark functions
    BenchmarkResult benchmarkOperation(const std::string& name, std::function<void()> operation,
                                      size_t iterations = 100);
    
    // Performance assertion
    bool assertPerformance(const std::string& testName, double maxTimeSeconds,
                          std::function<void()> testFunc);
    
    // Memory benchmark
    BenchmarkResult benchmarkMemory(size_t size = 1024 * 1024); // 1MB
    
    // CPU benchmark
    BenchmarkResult benchmarkCPU(size_t iterations = 10000000);
    
    // System benchmark
    std::map<std::string, BenchmarkResult> benchmarkSystem();
    
    // Generate performance report
    std::string generatePerformanceReport();
    
    // Get performance statistics
    std::string getPerformanceStats();
}

} // namespace nlm

#endif // NLM_PERFORMANCE_HPP