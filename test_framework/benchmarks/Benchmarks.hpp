// Performance Benchmark Framework for NLM Testing
// Provides comprehensive benchmark capabilities for performance measurement

#pragma once

#include "types/TestTypes.hpp"
#include <chrono>
#include <vector>
#include <string>
#include <functional>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <thread>
#include <future>
#include <memory>
#include <unordered_map>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace test {

// Forward declarations
class BenchmarkSuite;
class PerformanceAnalyzer;

// Benchmark metadata structure
struct BenchmarkMetadata {
    std::string name;
    std::string description;
    std::string category;
    std::string tags;
    std::string unit;
    BenchmarkClass performanceClass;
    std::string author;
    std::string date;
    std::unordered_map<std::string, std::string> properties;
    
    BenchmarkMetadata()
        : name("Unknown")
        , description("No description")
        , category("General")
        , tags("")
        , unit("us")
        , performanceClass(BenchmarkClass::Acceptable)
        , author("Unknown")
        , date("") {}
    
    BenchmarkMetadata(const std::string& name, const std::string& description = "")
        : name(name)
        , description(description)
        , category("General")
        , tags("")
        , unit("us")
        , performanceClass(BenchmarkClass::Acceptable)
        , author("Unknown")
        , date("") {}
};

// Performance measurement configuration
struct BenchmarkConfig {
    size_t warmupIterations = 100;
    size_t benchmarkIterations = 1000;
    size_t minIterations = 100;
    size_t maxIterations = 10000;
    double timeout = 30.0; // seconds
    bool repeat = true;
    bool parallel = false;
    size_t parallelThreads = std::thread::hardware_concurrency();
    double confidenceInterval = 0.95;
    std::string outputFormat = "console";
    bool saveResults = false;
    std::string resultsFile;
    bool statisticalAnalysis = true;
    bool regressionDetection = true;
    double regressionThreshold = 0.05; // 5% regression
    std::unordered_map<std::string, std::string> customOptions;
    
    BenchmarkConfig() = default;
    BenchmarkConfig(size_t warmup, size_t benchmark, size_t min, size_t max)
        : warmupIterations(warmup)
        , benchmarkIterations(benchmark)
        , minIterations(min)
        , maxIterations(max) {}
};

// High-resolution timer for benchmark measurements
class BenchmarkTimer {
public:
    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<double, std::microseconds>;
    
    BenchmarkTimer() : startTime_(Clock::now()), isRunning_(true) {}
    ~BenchmarkTimer() = default;
    
    void start() {
        if (!isRunning_) {
            startTime_ = Clock::now();
            isRunning_ = true;
        }
    }
    
    void stop() {
        if (isRunning_) {
            auto endTime = Clock::now();
            duration_ += endTime - startTime_;
            isRunning_ = false;
        }
    }
    
    void reset() {
        duration_ = std::chrono::microseconds(0);
        isRunning_ = false;
    }
    
    double elapsedMicroseconds() const {
        auto current = Clock::now();
        auto elapsed = duration_;
        if (isRunning_) {
            elapsed += (current - startTime_);
        }
        return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    }
    
    double elapsedMilliseconds() const {
        return elapsedMicroseconds() / 1000.0;
    }
    
    double elapsedSeconds() const {
        return elapsedMicroseconds() / 1000000.0;
    }
    
    bool isRunning() const { return isRunning_; }
    std::chrono::microseconds duration() const { return std::chrono::duration_cast<std::chrono::microseconds>(duration_); }
    
private:
    Clock::time_point startTime_;
    std::chrono::microseconds duration_;
    bool isRunning_;
};

// Basic benchmark implementation
class Benchmark {
public:
    Benchmark(const BenchmarkMetadata& metadata = BenchmarkMetadata())
        : metadata_(metadata), config_(), warmupTimer_(), benchmarkTimer_() {}
    
    virtual ~Benchmark() = default;
    
    // Getters
    const BenchmarkMetadata& getMetadata() const { return metadata_; }
    const BenchmarkConfig& getConfig() const { return config_; }
    
    // Setters
    void setMetadata(const BenchmarkMetadata& metadata) { metadata_ = metadata; }
    void setConfig(const BenchmarkConfig& config) { config_ = config; }
    
    // Benchmark configuration
    void setIterations(size_t warmup, size_t benchmark, size_t min, size_t max) {
        config_.warmupIterations = warmup;
        config_.benchmarkIterations = benchmark;
        config_.minIterations = min;
        config_.maxIterations = max;
    }
    
    void setTimeout(double timeout) { config_.timeout = timeout; }
    void setParallel(bool parallel, size_t threads = 0) {
        config_.parallel = parallel;
        if (threads > 0) config_.parallelThreads = threads;
    }
    
    void setConfidenceInterval(double interval) { config_.confidenceInterval = interval; }
    void setOutputFormat(const std::string& format) { config_.outputFormat = format; }
    
    // Benchmark execution interface
    virtual BenchmarkResult run(size_t iterations) = 0;
    
    // Setup/teardown hooks
    virtual void setup() {}
    virtual void teardown() {}
    
    // Convenience methods for common benchmark patterns
    template<typename Func>
    BenchmarkResult timeFunction(const std::string& name, Func&& func, size_t iterations) {
        std::vector<double> timings;
        timings.reserve(iterations);
        
        // Warmup phase
        for (size_t i = 0; i < config_.warmupIterations; ++i) {
            func();
        }
        
        // Benchmark phase
        for (size_t i = 0; i < iterations; ++i) {
            warmupTimer_.start();
            func();
            warmupTimer_.stop();
            timings.push_back(warmupTimer_.elapsedMicroseconds());
        }
        
        return createBenchmarkResult(name, timings, "us");
    }
    
    // Statistical analysis
    static BenchmarkResult calculateStatistics(const std::vector<double>& timings,
                                              const std::string& unit = "us");
    
    // Performance classification
    static PerformanceClass classifyPerformance(double meanDuration,
                                                const BenchmarkConfig& config);
    
    // Benchmark result creation helpers
    static BenchmarkResult createBenchmarkResult(const std::string& name,
                                                const std::vector<double>& timings,
                                                const std::string& unit = "us");
    static BenchmarkResult createBenchmarkResult(const BenchmarkMetadata& metadata,
                                                const std::vector<double>& timings,
                                                const std::string& unit = "us");
    
    // Configuration helpers
    static BenchmarkConfig getDefaultConfig(size_t benchmarkIterations = 1000);
    static BenchmarkConfig getQuickConfig();
    static BenchmarkConfig getComprehensiveConfig();
    
    // Benchmark discovery and registration
    static std::vector<Benchmark*> discoverBenchmarks();
    static bool registerBenchmark(Benchmark* benchmark);
    static bool unregisterBenchmark(Benchmark* benchmark);
    static std::vector<Benchmark*> getAllBenchmarks();
    static std::vector<Benchmark*> getBenchmarksByCategory(const std::string& category);
    static Benchmark* findBenchmarkByName(const std::string& name);
    
    // Batch benchmark execution
    static std::vector<BenchmarkResult> runBenchmarks(const std::vector<Benchmark*>& benchmarks);
    static std::vector<BenchmarkResult> runBenchmarkSuite(const std::vector<Benchmark*>& benchmarks,
                                                          const BenchmarkConfig& config);
    
protected:
    BenchmarkMetadata metadata_;
    BenchmarkConfig config_;
    BenchmarkTimer warmupTimer_;
    BenchmarkTimer benchmarkTimer_;
    
private:
    static std::vector<Benchmark*> registeredBenchmarks_;
    static std::mutex registrationMutex_;
};

// Benchmark suite for organizing related benchmarks
class BenchmarkSuite {
public:
    BenchmarkSuite(const std::string& name, const std::string& description = "")
        : name_(name), description_(description) {}
    
    ~BenchmarkSuite() = default;
    
    // Getters
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    
    // Benchmark management
    void addBenchmark(Benchmark* benchmark) {
        benchmarks_.push_back(benchmark);
    }
    
    void removeBenchmark(Benchmark* benchmark) {
        auto it = std::find(benchmarks_.begin(), benchmarks_.end(), benchmark);
        if (it != benchmarks_.end()) {
            benchmarks_.erase(it);
        }
    }
    
    void clearBenchmarks() {
        benchmarks_.clear();
    }
    
    size_t getBenchmarkCount() const { return benchmarks_.size(); }
    
    // Benchmark access
    Benchmark* getBenchmark(size_t index) {
        return (index < benchmarks_.size()) ? benchmarks_[index] : nullptr;
    }
    
    const Benchmark* getBenchmark(size_t index) const {
        return (index < benchmarks_.size()) ? benchmarks_[index] : nullptr;
    }
    
    std::vector<Benchmark*> getAllBenchmarks() const {
        return benchmarks_;
    }
    
    // Benchmark filtering
    std::vector<Benchmark*> getBenchmarksByCategory(const std::string& category) const {
        std::vector<Benchmark*> filtered;
        for (auto* benchmark : benchmarks_) {
            if (benchmark->getMetadata().category == category) {
                filtered.push_back(benchmark);
            }
        }
        return filtered;
    }
    
    std::vector<Benchmark*> getBenchmarksByTag(const std::string& tag) const {
        std::vector<Benchmark*> filtered;
        for (auto* benchmark : benchmarks_) {
            if (benchmark->getMetadata().tags.find(tag) != std::string::npos) {
                filtered.push_back(benchmark);
            }
        }
        return filtered;
    }
    
    // Benchmark execution
    std::vector<BenchmarkResult> runAllBenchmarks() {
        std::vector<BenchmarkResult> results;
        for (auto* benchmark : benchmarks_) {
            try {
                benchmark->setup();
                auto result = benchmark->run(config_.benchmarkIterations);
                benchmark->teardown();
                results.push_back(result);
            } catch (const std::exception& e) {
                // Create error result
                BenchmarkResult errorResult;
                errorResult.name = benchmark->getMetadata().name;
                errorResult.status = BenchmarkStatus::Error;
                errorResult.errorMessage = e.what();
                errorResult.mean = 0.0;
                errorResult.stddev = 0.0;
                errorResult.min = 0.0;
                errorResult.max = 0.0;
                errorResult.count = 0;
                results.push_back(errorResult);
            }
        }
        return results;
    }
    
    // Configuration
    void setConfig(const BenchmarkConfig& config) { config_ = config; }
    const BenchmarkConfig& getConfig() const { return config_; }
    
    // Suite information
    std::string getInfo() const {
        std::stringstream ss;
        ss << "Benchmark Suite: " << name_ << std::endl;
        ss << "Description: " << description_ << std::endl;
        ss << "Benchmark Count: " << benchmarks_.size() << std::endl;
        return ss.str();
    }
    
private:
    std::string name_;
    std::string description_;
    std::vector<Benchmark*> benchmarks_;
    BenchmarkConfig config_;
};

// Performance analyzer for statistical analysis and comparison
class PerformanceAnalyzer {
public:
    // Statistical analysis methods
    static double calculateMean(const std::vector<double>& values);
    static double calculateStandardDeviation(const std::vector<double>& values, double mean = 0.0);
    static double calculateMedian(const std::vector<double>& values);
    static double calculatePercentile(const std::vector<double>& values, double percentile);
    static double calculateQuartiles(const std::vector<double>& values, std::vector<double>& quartiles);
    static double calculateSkewness(const std::vector<double>& values, double mean, double stddev);
    static double calculateKurtosis(const std::vector<double>& values, double mean, double stddev);
    
    // Statistical tests
    static double tTest(const std::vector<double>& baseline,
                       const std::vector<double>& current);
    static double chiSquareTest(const std::vector<size_t>& observed,
                               const std::vector<size_t>& expected);
    static double anovaTest(const std::vector<std::vector<double>>& groups);
    
    // Performance comparison
    static double comparePerformance(const BenchmarkResult& baseline,
                                   const BenchmarkResult& current);
    static PerformanceClass classifyPerformanceLevel(double meanDuration,
                                                     const BenchmarkConfig& config);
    static std::string getPerformanceLevelDescription(PerformanceClass level);
    
    // Regression detection
    static bool detectRegression(const BenchmarkResult& baseline,
                               const BenchmarkResult& current,
                               double threshold = 0.05);
    static double calculateRegressionPercentage(const BenchmarkResult& baseline,
                                               const BenchmarkResult& current);
    
    // Performance normalization
    static double normalizePerformance(const BenchmarkResult& result,
                                       double referenceValue = 1.0);
    static std::vector<double> normalizePerformance(const std::vector<BenchmarkResult>& results,
                                                    const std::string& referenceName = "");
    
    // Performance aggregation
    static BenchmarkResult aggregateResults(const std::vector<BenchmarkResult>& results,
                                           const std::string& aggregationMethod = "mean");
    static std::vector<BenchmarkResult> filterResultsByPerformance(const std::vector<BenchmarkResult>& results,
                                                                   PerformanceClass minLevel);
    
    // Performance visualization data
    static std::string generatePerformanceReport(const std::vector<BenchmarkResult>& results,
                                                 const std::string& format = "console");
    static std::string generatePerformanceComparison(const std::vector<BenchmarkResult>& baseline,
                                                     const std::vector<BenchmarkResult>& current,
                                                     const std::string& format = "console");
    
    // Benchmark suite analysis
    static BenchmarkSuite analyzeBenchmarkSuite(const BenchmarkSuite& suite);
    static std::vector<BenchmarkResult> runBenchmarkSuiteWithAnalysis(BenchmarkSuite& suite);
    
    // Memory and resource analysis
    static double estimateMemoryUsage(const BenchmarkResult& result);
    static std::chrono::microseconds estimateExecutionTime(const BenchmarkResult& result);
    static double calculateMemoryBandwidth(const BenchmarkResult& result, double dataSize);
    static double calculateFLOPS(const BenchmarkResult& result, double operations);
    
    // Performance trends
    static std::vector<BenchmarkResult> aggregateOverTime(const std::vector<BenchmarkResult>& results,
                                                         const std::string& timeColumn = "timestamp");
    static std::vector<double> calculatePerformanceTrend(const std::vector<BenchmarkResult>& results,
                                                        const std::string& trendType = "linear");
    static bool detectPerformanceRegression(const std::vector<BenchmarkResult>& results,
                                           double threshold = 0.05);
    
    // Benchmark optimization
    static BenchmarkResult optimizeBenchmark(const BenchmarkResult& result,
                                            const std::vector<double>& parameters);
    static std::vector<BenchmarkResult> optimizeBenchmarks(const std::vector<BenchmarkResult>& results,
                                                           const std::vector<std::vector<double>>& parameters);
    
    // Performance benchmarking
    static BenchmarkResult benchmarkFunction(const std::string& name,
                                            std::function<void()> func,
                                            size_t iterations = 1000,
                                            const BenchmarkConfig& config = BenchmarkConfig());
    static BenchmarkResult benchmarkMemoryAccess(const std::string& name,
                                                 std::function<void*()> allocFunc,
                                                 std::function<void(void*)> freeFunc,
                                                 size_t size, size_t iterations = 1000);
    static BenchmarkResult benchmarkCPUUtilization(const std::string& name,
                                                   std::function<void()> workload,
                                                   size_t threads = std::thread::hardware_concurrency());
    
private:
    static std::vector<double> extractValues(const std::vector<BenchmarkResult>& results,
                                            const std::string& metric = "mean");
    static std::string formatBenchmarkResult(const BenchmarkResult& result,
                                            const std::string& format = "console");
};

// Benchmark registry for global benchmark management
class BenchmarkRegistry {
public:
    static BenchmarkRegistry& getInstance() {
        static BenchmarkRegistry instance;
        return instance;
    }
    
    // Benchmark registration
    void registerBenchmark(Benchmark* benchmark) {
        std::lock_guard<std::mutex> lock(mutex_);
        benchmarks_.push_back(benchmark);
    }
    
    void unregisterBenchmark(Benchmark* benchmark) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = std::find(benchmarks_.begin(), benchmarks_.end(), benchmark);
        if (it != benchmarks_.end()) {
            benchmarks_.erase(it);
        }
    }
    
    // Benchmark discovery
    std::vector<Benchmark*> getAllBenchmarks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return benchmarks_;
    }
    
    std::vector<Benchmark*> getBenchmarksByCategory(const std::string& category) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Benchmark*> filtered;
        for (auto* benchmark : benchmarks_) {
            if (benchmark->getMetadata().category == category) {
                filtered.push_back(benchmark);
            }
        }
        return filtered;
    }
    
    // Benchmark execution
    std::vector<BenchmarkResult> runBenchmarks(const BenchmarkConfig& config = BenchmarkConfig()) {
        std::vector<BenchmarkResult> results;
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (auto* benchmark : benchmarks_) {
            try {
                benchmark->setup();
                auto result = benchmark->run(config.benchmarkIterations);
                benchmark->teardown();
                results.push_back(result);
            } catch (const std::exception& e) {
                BenchmarkResult errorResult;
                errorResult.name = benchmark->getMetadata().name;
                errorResult.status = BenchmarkStatus::Error;
                errorResult.errorMessage = e.what();
                errorResult.mean = 0.0;
                errorResult.stddev = 0.0;
                errorResult.min = 0.0;
                errorResult.max = 0.0;
                errorResult.count = 0;
                results.push_back(errorResult);
            }
        }
        return results;
    }
    
    // Benchmark suite management
    BenchmarkSuite createBenchmarkSuite(const std::string& name,
                                        const std::string& description = "",
                                        const BenchmarkConfig& config = BenchmarkConfig());
    
    // Configuration
    void setGlobalConfig(const BenchmarkConfig& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        globalConfig_ = config;
    }
    
    const BenchmarkConfig& getGlobalConfig() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return globalConfig_;
    }
    
    // Registry management
    void clearAllBenchmarks() {
        std::lock_guard<std::mutex> lock(mutex_);
        benchmarks_.clear();
    }
    
    size_t getBenchmarkCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return benchmarks_.size();
    }
    
    // Information
    std::string getRegistryInfo() const {
        std::stringstream ss;
        std::lock_guard<std::mutex> lock(mutex_);
        
        ss << "Benchmark Registry" << std::endl;
        ss << "===================" << std::endl;
        ss << "Total Benchmarks: " << benchmarks_.size() << std::endl;
        ss << std::endl;
        
        std::unordered_map<std::string, size_t> categoryCount;
        for (auto* benchmark : benchmarks_) {
            categoryCount[benchmark->getMetadata().category]++;
        }
        
        ss << "Benchmarks by Category:" << std::endl;
        for (const auto& pair : categoryCount) {
            ss << "  " << pair.first << ": " << pair.second << std::endl;
        }
        
        return ss.str();
    }
    
private:
    BenchmarkRegistry() = default;
    ~BenchmarkRegistry() = default;
    BenchmarkRegistry(const BenchmarkRegistry&) = delete;
    BenchmarkRegistry& operator=(const BenchmarkRegistry&) = delete;
    
    std::vector<Benchmark*> benchmarks_;
    BenchmarkConfig globalConfig_;
    mutable std::mutex mutex_;
};

// Convenience functions for common benchmark scenarios
namespace BenchmarkFactory {
    // Common benchmark patterns
    Benchmark* createMemoryBenchmark(const std::string& name,
                                     size_t size,
                                     const std::string& unit = "MB");
    
    Benchmark* createCPUBenchmark(const std::string& name,
                                  std::function<void()> workload);
    
    Benchmark* createIOBenchmark(const std::string& name,
                                 std::function<void()> ioOperation);
    
    Benchmark* createNetworkBenchmark(const std::string& name,
                                      std::function<void()> networkOperation);
    
    // Benchmark creators for specific components
    Benchmark* createNeuronBenchmark(const std::string& name);
    Benchmark* createSynapseBenchmark(const std::string& name);
    Benchmark* createMemoryBenchmark(const std::string& name);
    Benchmark* createLearningBenchmark(const std::string& name);
    Benchmark* createPlasticityBenchmark(const std::string& name);
    
    // Performance benchmark creators
    Benchmark* createPerformanceBenchmark(const std::string& name,
                                         const BenchmarkConfig& config = BenchmarkConfig());
    
    // Stress test benchmark creators
    Benchmark* createStressTestBenchmark(const std::string& name,
                                         size_t stressLevel = 1);
    
    // Comparison benchmark creators
    Benchmark* createComparisonBenchmark(const std::string& name,
                                         const std::vector<Benchmark*>& referenceBenchmarks);
}

} // namespace test
} // namespace nlm
