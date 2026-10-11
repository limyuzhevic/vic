#pragma once

#include "TestFramework.hpp"
#include "TestReporting.hpp"
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <concurrentqueue.h>

namespace nlm {

class PerformanceBenchmark {
public:
    enum class BenchmarkType {
        SPATIAL,
        TEMPORAL,
        MEMORY,
        COMPUTATION,
        IO,
        NETWORK
    };
    
    struct BenchmarkMetrics {
        std::string benchmarkName;
        BenchmarkType type;
        double executionTime;
        double memoryUsage;
        double cpuUsage;
        int64_t operationsPerSecond;
        int64_t dataThroughput; // bytes per second
        std::chrono::duration<double> duration;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, double> customMetrics;
        
        BenchmarkMetrics(const std::string& name, BenchmarkType t)
            : benchmarkName(name), type(t), executionTime(0.0), memoryUsage(0.0),
              cpuUsage(0.0), operationsPerSecond(0), dataThroughput(0),
              timestamp(std::chrono::system_clock::now()) {}
    };
    
    using BenchmarkResult = BenchmarkMetrics;
    
    PerformanceBenchmark();
    ~PerformanceBenchmark() = default;
    
    static PerformanceBenchmark& getInstance() {
        static PerformanceBenchmark instance;
        return instance;
    }
    
    void addBenchmark(const std::string& name, BenchmarkType type, std::function<void()> benchmarkFunc);
    void removeBenchmark(const std::string& name);
    
    BenchmarkResult runBenchmark(const std::string& name);
    std::vector<BenchmarkResult> runAllBenchmarks();
    
    double calculatePerformanceScore(const BenchmarkResult& benchmark) const;
    double calculatePerformanceScore(const std::vector<BenchmarkResult>& benchmarks) const;
    
    std::string getPerformanceReport(const std::vector<BenchmarkResult>& results) const;
    
    // Performance target checking
    bool checkPerformanceTarget(const BenchmarkResult& benchmark, double targetTime, double tolerance = 0.1) const;
    bool checkMemoryTarget(const BenchmarkResult& benchmark, double targetMemoryMB, double tolerance = 0.2) const;
    
    // Performance comparison
    double comparePerformance(const BenchmarkResult& benchmark1, const BenchmarkResult& benchmark2, 
                            const std::string& metric = "execution_time") const;
    
    // Performance trend analysis
    std::vector<double> calculatePerformanceTrend(const std::vector<BenchmarkResult>& results) const;
    double calculatePerformanceRegressionRate(const std::vector<BenchmarkResult>& results) const;
    
private:
    std::mutex benchmarksMutex;
    std::unordered_map<std::string, std::pair<BenchmarkType, std::function<void()>>> benchmarks;
    std::vector<BenchmarkResult> benchmarkResults;
};

class MemoryAllocationTracker {
public:
    struct AllocationInfo {
        std::chrono::system_clock::time_point timestamp;
        size_t allocatedBytes;
        size_t currentBytes;
        size_t peakBytes;
        std::string allocationType;
        std::string stackTrace;
        
        AllocationInfo(size_t bytes, const std::string& type)
            : timestamp(std::chrono::system_clock::now()), allocatedBytes(bytes),
              currentBytes(bytes), peakBytes(bytes), allocationType(type) {}
    };
    
    MemoryAllocationTracker();
    ~MemoryAllocationTracker() = default;
    
    static MemoryAllocationTracker& getInstance() {
        static MemoryAllocationTracker instance;
        return instance;
    }
    
    void trackAllocation(size_t bytes, const std::string& type = "unknown");
    void trackDeallocation(size_t bytes, const std::string& type = "unknown");
    
    std::vector<AllocationInfo> getAllocationHistory() const { return allocationHistory; }
    size_t getCurrentMemoryUsage() const { return currentMemoryUsage; }
    size_t getPeakMemoryUsage() const { return peakMemoryUsage; }
    double getMemoryGrowthRate() const;
    
    bool detectMemoryLeak(size_t thresholdBytes = 1024) const; // 1KB threshold
    std::vector<size_t> detectLeakPoints(size_t thresholdBytes = 1024) const;
    
    std::string getMemoryUsageReport() const;
    
    void reset();
    
    // Integration with TestFramework
    void setupTrackingForTest(TestFramework::TestCase& testCase);
    void cleanupTrackingForTest(TestFramework::TestCase& testCase);
    
    // Memory pressure simulation
    void simulateMemoryPressure(size_t pressureBytes);
    void releaseMemoryPressure();
    
private:
    std::mutex memoryMutex;
    std::vector<AllocationInfo> allocationHistory;
    size_t currentMemoryUsage;
    size_t peakMemoryUsage;
    std::chrono::system_clock::time_point lastAllocationTime;
    std::map<std::string, size_t> allocationCounts;
    std::map<std::string, size_t> deallocationCounts;
};

class PerformanceProfiler {
public:
    struct ProfileData {
        std::chrono::system_clock::time_point timestamp;
        std::string functionName;
        std::string fileName;
        int lineNumber;
        std::chrono::duration<double> executionTime;
        double cpuTime;
        double memoryTime;
        int instructionCount;
        int cacheMisses;
        int branchMispredictions;
        
        ProfileData(const std::string& func, const std::string& file, int line)
            : timestamp(std::chrono::system_clock::now()), functionName(func),
              fileName(file), lineNumber(line), executionTime(0.0), cpuTime(0.0),
              memoryTime(0.0), instructionCount(0), cacheMisses(0),
              branchMispredictions(0) {}
    };
    
    PerformanceProfiler();
    ~PerformanceProfiler() = default;
    
    static PerformanceProfiler& getInstance() {
        static PerformanceProfiler instance;
        return instance;
    }
    
    void startProfiling(const std::string& functionName, const std::string& fileName, int lineNumber);
    void stopProfiling();
    void pauseProfiling();
    void resumeProfiling();
    
    std::vector<ProfileData> getProfileData() const { return profileData; }
    double getTotalProfilingTime() const;
    std::string getProfileReport() const;
    
    void generateFlameGraph(const std::vector<ProfileData>& data, const std::string& outputFile) const;
    void generateCallGraph(const std::vector<ProfileData>& data, const std::string& outputFile) const;
    
    // Performance analysis
    std::string identifyBottlenecks(const std::vector<ProfileData>& data, double thresholdPercent = 0.1) const;
    double calculateFunctionHotness(const ProfileData& data, const std::vector<ProfileData>& allData) const;
    
    void clearProfileData();
    
private:
    std::mutex profilingMutex;
    std::vector<ProfileData> profileData;
    std::chrono::system_clock::time_point profilingStartTime;
    std::chrono::duration<double> totalPausedTime;
    ProfileData* currentProfile;
};

class PerformanceTestSuite {
public:
    enum class TestType {
        UNIT,
        INTEGRATION,
        PERFORMANCE,
        STRESS,
        MEMORY,
        REGRESSION
    };
    
    struct PerformanceTest {
        std::string name;
        TestType type;
        std::string description;
        std::chrono::duration<double> maxExecutionTime;
        std::chrono::duration<double> maxMemoryUsage;
        double performanceTarget; // For performance tests
        bool enabled;
        
        PerformanceTest(const std::string& n, TestType t, const std::string& desc)
            : name(n), type(t), description(desc), maxExecutionTime(10.0), 
              maxMemoryUsage(1024.0), performanceTarget(1.0), enabled(true) {}
    };
    
    PerformanceTestSuite();
    ~PerformanceTestSuite() = default;
    
    static PerformanceTestSuite& getInstance() {
        static PerformanceTestSuite instance;
        return instance;
    }
    
    void addPerformanceTest(const PerformanceTest& test);
    void removePerformanceTest(const std::string& name);
    PerformanceTest& getPerformanceTest(const std::string& name);
    
    void addBenchmark(const std::string& benchmarkName, PerformanceBenchmark::BenchmarkType type, 
                     std::function<void()> benchmarkFunc);
    void addMemoryTest(const std::string& testName, std::function<void()> memoryTestFunc);
    void addStressTest(const std::string& testName, std::function<void()> stressTestFunc);
    
    std::vector<PerformanceBenchmark::BenchmarkResult> runBenchmarks();
    std::vector<TestResult> runPerformanceTests();
    std::vector<TestResult> runMemoryTests();
    std::vector<TestResult> runStressTests();
    
    void runAllPerformanceTests();
    void generatePerformanceReport(const std::string& title = "Performance Test Report");
    
    std::string getPerformanceSummary(const std::vector<PerformanceBenchmark::BenchmarkResult>& benchmarks) const;
    std::string getMemoryReport() const;
    
    // Performance regression detection
    bool detectPerformanceRegression(const std::vector<PerformanceBenchmark::BenchmarkResult>& baseline,
                                    const std::vector<PerformanceBenchmark::BenchmarkResult>& current) const;
    std::vector<std::string> getRegressedBenchmarks(const std::vector<PerformanceBenchmark::BenchmarkResult>& baseline,
                                                    const std::vector<PerformanceBenchmark::BenchmarkResult>& current) const;
    
    // Performance benchmarking
    double benchmarkSpatialOperations(size_t arraySize, int iterations);
    double benchmarkTemporalProcessing(size_t sequenceLength, int iterations);
    double benchmarkMemoryAllocation(int allocations, size_t allocationSize);
    double benchmarkComputationComplexity(int operations, int dataSize);
    
private:
    std::mutex suiteMutex;
    std::vector<PerformanceTest> performanceTests;
    PerformanceBenchmark benchmarkManager;
    MemoryAllocationTracker memoryTracker;
    PerformanceProfiler profiler;
    
    std::vector<TestResult> runSinglePerformanceTest(const PerformanceTest& test);
    std::vector<TestResult> runSingleMemoryTest(const std::string& name, std::function<void()> testFunc);
    std::vector<TestResult> runSingleStressTest(const std::string& name, std::function<void()> testFunc);
    
    std::string formatBenchmarkResult(const PerformanceBenchmark::BenchmarkResult& result) const;
    std::string formatMemoryReport(const MemoryAllocationTracker& tracker) const;
    std::string formatProfilerReport(const PerformanceProfiler& profiler) const;
};

// Performance test utilities
namespace perf {
    
    // Time measurement utilities
    class Timer {
    public:
        Timer();
        ~Timer();
        
        void start();
        double stop();
        double getElapsedTime() const;
        
        static double measure(std::function<void()> func);
        
    private:
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point stopTime;
    };
    
    // Memory measurement utilities
    class MemoryProfiler {
    public:
        static size_t getCurrentMemoryUsage();
        static size_t getPeakMemoryUsage();
        static double getMemoryUsagePercent();
        static void resetPeak();
        
        static size_t measureMemory(std::function<void()> func);
    };
    
    // CPU measurement utilities
    class CpuProfiler {
    public:
        static double getCurrentCpuUsage();
        static double measureCpu(std::function<void()> func);
    };
    
    // Data structures for performance testing
    template<typename T>
    class PerformanceArray {
    public:
        PerformanceArray(size_t size);
        ~PerformanceArray();
        
        void fill(const T& value);
        void randomize();
        double sum() const;
        double mean() const;
        double variance() const;
        double stddev() const;
        
        const T& operator[](size_t index) const { return data[index]; }
        T& operator[](size_t index) { return data[index]; }
        
        size_t size() const { return size_; }
        
        // Performance testing methods
        double benchmarkAccessTime(int iterations);
        double benchmarkFillTime(int iterations);
        double benchmarkSortTime(int iterations);
        
    private:
        T* data;
        size_t size_;
    };
    
    // Neural network performance testing utilities
    class NeuralPerfTest {
    public:
        struct NetworkMetrics {
            double forwardPassTime;
            double backwardPassTime;
            double updateTime;
            double memoryFootprint;
            double accuracy;
            int totalParameters;
        };
        
        static NetworkMetrics benchmarkForwardPass(int inputSize, int hiddenSize, int outputSize, int iterations);
        static NetworkMetrics benchmarkTraining(int inputSize, int hiddenSize, int outputSize, int trainingSamples, int iterations);
        static NetworkMetrics benchmarkInference(int inputSize, int hiddenSize, int outputSize, int inferenceCount);
    };
    
} // namespace perf

} // namespace nlm
