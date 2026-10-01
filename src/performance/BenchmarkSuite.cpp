#include "BenchmarkSuite.hpp"
#include "PerformanceProfiler.hpp"
#include "../../core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <future>
#include <filesystem>

namespace nlm {

struct BenchmarkSuite::Impl {
    std::shared_ptr<Config> config;
    std::unordered_map<std::string, BenchmarkConfig> benchmarkConfigs;
    std::unordered_map<std::string, std::function<void(BenchmarkResult&)>> benchmarkFunctions;
    std::vector<std::string> runningBenchmarks;
    std::vector<BenchmarkResult> completedResults;
    std::queue<std::string> pendingBenchmarks;
    mutable std::mutex resultsMutex;
    mutable std::mutex runningMutex;
    std::condition_variable resultCondition;
    std::atomic<bool> suiteRunning;
    std::atomic<bool> suitePaused;
    size_t maxConcurrentBenchmarks;
    std::chrono::milliseconds defaultTimeout;
    bool autoCleanupEnabled;
    std::string outputDirectory;
    std::thread workerThread;
    bool stopWorker;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg), suiteRunning(false), suitePaused(false),
                                       maxConcurrentBenchmarks(4), defaultTimeout(std::chrono::milliseconds(5000)),
                                       autoCleanupEnabled(true), stopWorker(false) {
        outputDirectory = "./benchmark_results";
        std::filesystem::create_directories(outputDirectory);
    }
};

BenchmarkSuite::BenchmarkSuite(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)) {}

BenchmarkSuite::~BenchmarkSuite() {
    shutdown();
}

bool BenchmarkSuite::initialize() {
    if (pImpl->suiteRunning) return false;
    
    pImpl->suiteRunning = true;
    pImpl->suitePaused = false;
    pImpl->stopWorker = false;
    
    // Start worker thread
    pImpl->workerThread = std::thread(&BenchmarkSuite::processBenchmarkResults, this);
    
    NLM_LOG_INFO("BenchmarkSuite: Initialized successfully");
    return true;
}

void BenchmarkSuite::shutdown() {
    if (!pImpl->suiteRunning) return;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->runningMutex);
        pImpl->suiteRunning = false;
        pImpl->suitePaused = false;
        pImpl->stopWorker = true;
    }
    
    {
        std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
        pImpl->resultCondition.notify_all();
    }
    
    if (pImpl->workerThread.joinable()) {
        pImpl->workerThread.join();
    }
    
    NLM_LOG_INFO("BenchmarkSuite: Shutdown completed");
}

void BenchmarkSuite::addBenchmark(const BenchmarkConfig& config,
                                 std::function<void(BenchmarkResult&)> benchmarkFunc) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    if (pImpl->validateBenchmarkConfig(config)) {
        pImpl->benchmarkConfigs[config.name] = config;
        pImpl->benchmarkFunctions[config.name] = benchmarkFunc;
        NLM_LOG_INFO("BenchmarkSuite: Added benchmark - " + config.name + " (" + config.description + ")");
    }
}

void BenchmarkSuite::addTimedBenchmark(const std::string& name,
                                      const std::string& description,
                                      std::function<void()> benchmarkFunc,
                                      size_t iterations) {
    BenchmarkConfig config(name, description);
    config.iterations = iterations;
    
    addBenchmark(config, [benchmarkFunc, iterations](BenchmarkResult& result) {
        auto profiler = std::make_unique<PerformanceProfiler>();
        profiler->initialize();
        
        profiler->startProfile("total_time");
        
        for (size_t i = 0; i < iterations; ++i) {
            profiler->startProfile("iteration_time");
            benchmarkFunc();
            profiler->stopProfile("iteration_time");
        }
        
        profiler->stopProfile("total_time");
        
        auto stats = profiler->getProfileStats();
        result.executionTime = stats.totalExecutionTime.count();
        result.memoryUsage = stats.totalMemoryUsage;
        result.cpuUsage = stats.cpuUsage;
        result.iterations = iterations;
        
        // Store detailed times
        auto timerData = profiler->getTimerData("total_time");
        result.detailedTimes.reserve(timerData.samples.size());
        for (const auto& sample : timerData.samples) {
            result.detailedTimes.push_back(sample.count());
        }
        
        result.status = "PASSED";
    });
}

void BenchmarkSuite::addMemoryBenchmark(const std::string& name,
                                       const std::string& description,
                                       std::function<void*(size_t)> memoryAllocator,
                                       std::function<void(void*)> memoryDeallocator,
                                       size_t allocationSize) {
    BenchmarkConfig config(name, description);
    
    addBenchmark(config, [memoryAllocator, memoryDeallocator, allocationSize](BenchmarkResult& result) {
        auto profiler = std::make_unique<PerformanceProfiler>();
        profiler->initialize();
        
        profiler->startProfile("allocation");
        void* ptr = memoryAllocator(allocationSize);
        profiler->stopProfile("allocation");
        
        profiler->startProfile("deallocation");
        memoryDeallocator(ptr);
        profiler->stopProfile("deallocation");
        
        auto stats = profiler->getProfileStats();
        result.executionTime = stats.totalExecutionTime.count();
        result.memoryUsage = stats.totalMemoryUsage;
        result.iterations = 1;
        result.status = "PASSED";
    });
}

void BenchmarkSuite::addCPUBenchmark(const std::string& name,
                                   const std::string& description,
                                   std::function<void(size_t)> cpuWork,
                                   size_t threadCount) {
    BenchmarkConfig config(name, description);
    
    addBenchmark(config, [cpuWork, threadCount](BenchmarkResult& result) {
        auto profiler = std::make_unique<PerformanceProfiler>();
        profiler->initialize();
        
        profiler->startProfile("cpu_work");
        
        std::vector<std::thread> threads;
        size_t workPerThread = 1000000 / threadCount;
        
        for (size_t i = 0; i < threadCount; ++i) {
            threads.emplace_back([cpuWork, workPerThread, i]() {
                cpuWork(workPerThread + i * 100);
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        profiler->stopProfile("cpu_work");
        
        auto stats = profiler->getProfileStats();
        result.executionTime = stats.totalExecutionTime.count();
        result.cpuUsage = stats.cpuUsage;
        result.throughput = static_cast<uint64_t>(threadCount * workPerThread);
        result.iterations = threadCount;
        result.status = "PASSED";
    });
}

void BenchmarkSuite::addConfigurationBenchmark(const std::string& brainName,
                                               const std::string& configName,
                                               std::shared_ptr<Config> config) {
    BenchmarkConfig benchmarkConfig("config_" + brainName + "_" + configName,
                                   "Test " + brainName + " with " + configName + " configuration");
    benchmarkConfig.parameters["brain"] = brainName;
    benchmarkConfig.parameters["config"] = configName;
    
    addBenchmark(benchmarkConfig, [brainName, configName, config](BenchmarkResult& result) {
        // Simulate brain configuration test
        auto profiler = std::make_unique<PerformanceProfiler>();
        profiler->initialize(config);
        
        profiler->startProfile("config_test");
        
        // Simulate configuration operations
        for (int i = 0; i < 100; ++i) {
            profiler->startProfile("operation");
            // Simulate some work
            volatile double x = 3.14159;
            for (int j = 0; j < 1000; ++j) {
                x = x * 1.1;
            }
            profiler->stopProfile("operation");
        }
        
        profiler->stopProfile("config_test");
        
        auto stats = profiler->getProfileStats();
        result.executionTime = stats.totalExecutionTime.count();
        result.memoryUsage = stats.totalMemoryUsage;
        result.iterations = 100;
        result.status = "PASSED";
    });
}

void BenchmarkSuite::runConfigurationBenchmarks() {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    
    for (const auto& pair : pImpl->benchmarkConfigs) {
        if (pair.second.enabled && pair.second.parameters.find("brain") != pair.second.parameters.end()) {
            pImpl->pendingBenchmarks.push(pair.first);
        }
    }
}

void BenchmarkSuite::addPerformanceBenchmark(const std::string& testName,
                                            std::function<void()> performanceTest,
                                            size_t loadLevel) {
    BenchmarkConfig benchmarkConfig("perf_" + testName, "Performance test: " + testName);
    benchmarkConfig.parameters["load_level"] = std::to_string(loadLevel);
    
    addBenchmark(benchmarkConfig, [performanceTest, loadLevel](BenchmarkResult& result) {
        auto profiler = std::make_unique<PerformanceProfiler>();
        profiler->initialize();
        
        profiler->startProfile("performance_test");
        
        // Simulate varying load
        for (size_t i = 0; i < loadLevel * 10; ++i) {
            performanceTest();
        }
        
        profiler->stopProfile("performance_test");
        
        auto stats = profiler->getProfileStats();
        result.executionTime = stats.totalExecutionTime.count();
        result.memoryUsage = stats.totalMemoryUsage;
        result.cpuUsage = stats.cpuUsage;
        result.throughput = static_cast<uint64_t>(loadLevel * 10);
        result.iterations = loadLevel * 10;
        result.status = "PASSED";
    });
}

void BenchmarkSuite::runPerformanceBenchmarks() {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    
    for (const auto& pair : pImpl->benchmarkConfigs) {
        if (pair.second.enabled && 
            (pair.first.find("perf_") == 0 || pair.first.find("load_") == 0)) {
            pImpl->pendingBenchmarks.push(pair.first);
        }
    }
}

void BenchmarkSuite::addStressTest(const std::string& testName,
                                 std::function<void()> stressTest,
                                 size_t durationMs,
                                 double cpuTarget) {
    BenchmarkConfig benchmarkConfig("stress_" + testName, "Stress test: " + testName);
    benchmarkConfig.timeout = std::chrono::milliseconds(durationMs);
    benchmarkConfig.parameters["cpu_target"] = std::to_string(cpuTarget);
    
    addBenchmark(benchmarkConfig, [stressTest, durationMs, cpuTarget](BenchmarkResult& result) {
        auto profiler = std::make_unique<PerformanceProfiler>();
        profiler->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto endTime = startTime + std::chrono::milliseconds(durationMs);
        
        profiler->startProfile("stress_test");
        
        while (std::chrono::high_resolution_clock::now() < endTime) {
            stressTest();
        }
        
        profiler->stopProfile("stress_test");
        
        auto stats = profiler->getProfileStats();
        result.executionTime = stats.totalExecutionTime.count();
        result.memoryUsage = stats.totalMemoryUsage;
        result.cpuUsage = stats.cpuUsage;
        result.iterations = static_cast<size_t>(durationMs / 10); // Rough estimate
        result.status = "PASSED";
    });
}

void BenchmarkSuite::runStressTests() {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    
    for (const auto& pair : pImpl->benchmarkConfigs) {
        if (pair.second.enabled && pair.first.find("stress_") == 0) {
            pImpl->pendingBenchmarks.push(pair.first);
        }
    }
}

void BenchmarkSuite::addRegressionTest(const std::string& testName,
                                     std::function<void()> regressionTest,
                                     const std::string& baselineBenchmark) {
    BenchmarkConfig benchmarkConfig("reg_" + testName, "Regression test: " + testName);
    benchmarkConfig.parameters["baseline"] = baselineBenchmark;
    
    addBenchmark(benchmarkConfig, [regressionTest, baselineBenchmark](BenchmarkResult& result) {
        // Run regression test
        auto profiler = std::make_unique<PerformanceProfiler>();
        profiler->initialize();
        
        profiler->startProfile("regression_test");
        regressionTest();
        profiler->stopProfile("regression_test");
        
        auto stats = profiler->getProfileStats();
        result.executionTime = stats.totalExecutionTime.count();
        result.memoryUsage = stats.totalMemoryUsage;
        result.iterations = 1;
        result.status = "PASSED";
        
        // In real implementation, compare with baseline
    });
}

void BenchmarkSuite::runRegressionTests() {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    
    for (const auto& pair : pImpl->benchmarkConfigs) {
        if (pair.second.enabled && pair.first.find("reg_") == 0) {
            pImpl->pendingBenchmarks.push(pair.first);
        }
    }
}

void BenchmarkSuite::addComparisonTest(const std::string& testName,
                                      const std::vector<std::string>& benchmarks,
                                      const std::string& metric) {
    BenchmarkConfig benchmarkConfig("comp_" + testName, "Comparison test: " + testName);
    benchmarkConfig.parameters["metric"] = metric;
    
    std::vector<std::string> benchmarkNames = benchmarks;
    
    addBenchmark(benchmarkConfig, [benchmarkNames, metric](BenchmarkResult& result) {
        // In real implementation, run multiple benchmarks and compare
        result.status = "PASSED";
        
        for (const auto& name : benchmarkNames) {
            // Simulate benchmark execution
            result.executionTime += 1.0; // Dummy time
            result.metrics[name] = result.executionTime / benchmarkNames.size();
        }
    });
}

void BenchmarkSuite::runComparisonTests() {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    
    for (const auto& pair : pImpl->benchmarkConfigs) {
        if (pair.second.enabled && pair.first.find("comp_") == 0) {
            pImpl->pendingBenchmarks.push(pair.first);
        }
    }
}

void BenchmarkSuite::addLoadTest(const std::string& testName,
                               std::function<void(size_t)> loadTest,
                               size_t concurrentUsers,
                               size_t durationMs) {
    BenchmarkConfig benchmarkConfig("load_" + testName, "Load test: " + testName);
    benchmarkConfig.timeout = std::chrono::milliseconds(durationMs);
    benchmarkConfig.parameters["concurrent_users"] = std::to_string(concurrentUsers);
    
    addBenchmark(benchmarkConfig, [loadTest, concurrentUsers, durationMs](BenchmarkResult& result) {
        auto profiler = std::make_unique<PerformanceProfiler>();
        profiler->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto endTime = startTime + std::chrono::milliseconds(durationMs);
        
        profiler->startProfile("load_test");
        
        std::vector<std::thread> workers;
        std::atomic<size_t> completedUsers{0};
        
        for (size_t i = 0; i < concurrentUsers; ++i) {
            workers.emplace_back([&, i]() {
                loadTest(i);
                completedUsers++;
            });
        }
        
        while (completedUsers < concurrentUsers && 
               std::chrono::high_resolution_clock::now() < endTime) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        for (auto& worker : workers) {
            if (worker.joinable()) worker.join();
        }
        
        profiler->stopProfile("load_test");
        
        auto stats = profiler->getProfileStats();
        result.executionTime = stats.totalExecutionTime.count();
        result.memoryUsage = stats.totalMemoryUsage;
        result.throughput = concurrentUsers;
        result.iterations = concurrentUsers;
        result.status = "PASSED";
    });
}

void BenchmarkSuite::runLoadTests() {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    
    for (const auto& pair : pImpl->benchmarkConfigs) {
        if (pair.second.enabled && pair.first.find("load_") == 0) {
            pImpl->pendingBenchmarks.push(pair.first);
        }
    }
}

void BenchmarkSuite::scheduleBenchmark(const std::string& benchmarkName,
                                       std::chrono::system_clock::time_point when) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    // In real implementation, this would schedule the benchmark for a future time
    NLM_LOG_INFO("BenchmarkSuite: Scheduling benchmark " + benchmarkName + " for future execution");
}

void BenchmarkSuite::scheduleRecurringBenchmark(const std::string& benchmarkName,
                                               const std::string& cronExpression) {
    // In real implementation, this would schedule recurring benchmarks
    NLM_LOG_INFO("BenchmarkSuite: Scheduling recurring benchmark " + benchmarkName + 
                 " with cron: " + cronExpression);
}

void BenchmarkSuite::cancelBenchmarkSchedule(const std::string& benchmarkName) {
    // In real implementation, this would cancel scheduled benchmarks
    NLM_LOG_INFO("BenchmarkSuite: Canceling benchmark schedule for " + benchmarkName);
}

const BenchmarkSuite::BenchmarkResult& BenchmarkSuite::getBenchmarkResult(const std::string& benchmarkName, 
                                                                         size_t iteration) const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    static const BenchmarkResult emptyResult("", "");
    
    // Find the benchmark result (in real implementation, would search by iteration)
    for (const auto& result : pImpl->completedResults) {
        if (result.benchmarkName == benchmarkName) {
            return result;
        }
    }
    
    return emptyResult;
}

std::vector<BenchmarkSuite::BenchmarkResult> BenchmarkSuite::getBenchmarkHistory(const std::string& benchmarkName) const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    std::vector<BenchmarkResult> history;
    for (const auto& result : pImpl->completedResults) {
        if (result.benchmarkName == benchmarkName) {
            history.push_back(result);
        }
    }
    
    return history;
}

std::vector<BenchmarkSuite::BenchmarkResult> BenchmarkSuite::getAllResults() const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    return pImpl->completedResults;
}

void BenchmarkSuite::clearResults() {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->completedResults.clear();
    NLM_LOG_INFO("BenchmarkSuite: Results cleared");
}

void BenchmarkSuite::saveResults(const std::string& filename, const std::string& format) const {
    if (format == "json") {
        std::ofstream file(filename);
        if (!file.is_open()) return;
        
        std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
        
        file << "{\n";
        file << "  \"benchmarks\": [\n";
        
        for (size_t i = 0; i < pImpl->completedResults.size(); ++i) {
            const auto& result = pImpl->completedResults[i];
            file << "    {\n";
            file << "      \"name\": \"" << result.benchmarkName << "\",\n";
            file << "      \"config\": \"" << result.configuration << "\",\n";
            file << "      \"execution_time\": " << result.executionTime << ",\n";
            file << "      \"memory_usage\": " << result.memoryUsage << ",\n";
            file << "      \"cpu_usage\": " << result.cpuUsage << ",\n";
            file << "      \"throughput\": " << result.throughput << ",\n";
            file << "      \"iterations\": " << result.iterations << ",\n";
            file << "      \"status\": \"" << result.status << "\"\n";
            file << "    }";
            if (i < pImpl->completedResults.size() - 1) file << ",\n";
        }
        
        file << "\n  ]\n";
        file << "}\n";
        file.close();
    }
    
    NLM_LOG_INFO("BenchmarkSuite: Results saved to " + filename);
}

void BenchmarkSuite::loadResults(const std::string& filename, const std::string& format) {
    // In real implementation, would load benchmark results from file
    NLM_LOG_INFO("BenchmarkSuite: Loading results from " + filename);
}

BenchmarkSuite::SuiteStats BenchmarkSuite::getSuiteStats() const {
    SuiteStats stats;
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    stats.totalBenchmarks = pImpl->benchmarkConfigs.size();
    stats.completedBenchmarks = pImpl->completedResults.size();
    
    for (const auto& result : pImpl->completedResults) {
        if (result.status == "PASSED") stats.passedBenchmarks++;
        else if (result.status == "FAILED") stats.failedBenchmarks++;
        else if (result.status == "PENDING") stats.skippedBenchmarks++;
        
        stats.benchmarkCounts[result.benchmarkName]++;
        stats.totalTime += result.executionTime;
        
        if (pImpl->completedResults.size() > 0) {
            stats.minTime = (stats.minTime == 0) ? result.executionTime : 
                           std::min(stats.minTime, result.executionTime);
            stats.maxTime = std::max(stats.maxTime, result.executionTime);
        }
    }
    
    if (stats.completedBenchmarks > 0) {
        stats.averageTime = stats.totalTime / stats.completedBenchmarks;
    }
    
    return stats;
}

double BenchmarkSuite::getAverageExecutionTime(const std::string& benchmarkName) const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    double sum = 0.0;
    size_t count = 0;
    
    for (const auto& result : pImpl->completedResults) {
        if (result.benchmarkName == benchmarkName) {
            sum += result.executionTime;
            count++;
        }
    }
    
    return (count > 0) ? sum / count : 0.0;
}

double BenchmarkSuite::getMedianExecutionTime(const std::string& benchmarkName) const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    std::vector<double> times;
    for (const auto& result : pImpl->completedResults) {
        if (result.benchmarkName == benchmarkName) {
            times.push_back(result.executionTime);
        }
    }
    
    if (times.empty()) return 0.0;
    
    size_t size = times.size();
    std::sort(times.begin(), times.end());
    
    if (size % 2 == 0) {
        return (times[size/2 - 1] + times[size/2]) / 2.0;
    } else {
        return times[size/2];
    }
}

double BenchmarkSuite::getStandardDeviation(const std::string& benchmarkName) const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    std::vector<double> times;
    for (const auto& result : pImpl->completedResults) {
        if (result.benchmarkName == benchmarkName) {
            times.push_back(result.executionTime);
        }
    }
    
    if (times.size() <= 1) return 0.0;
    
    double mean = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    double sumSquaredDiff = 0.0;
    
    for (double time : times) {
        sumSquaredDiff += (time - mean) * (time - mean);
    }
    
    return std::sqrt(sumSquaredDiff / (times.size() - 1));
}

std::string BenchmarkSuite::getPerformanceTrend(const std::string& benchmarkName, size_t windowSize) const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    std::vector<double> recentTimes;
    
    for (auto it = pImpl->completedResults.rbegin(); 
         it != pImpl->completedResults.rend() && recentTimes.size() < windowSize; 
         ++it) {
        if (it->benchmarkName == benchmarkName) {
            recentTimes.push_back(it->executionTime);
        }
    }
    
    if (recentTimes.size() < 2) return "INSUFFICIENT_DATA";
    
    // Simple linear regression to determine trend
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    for (size_t i = 0; i < recentTimes.size(); ++i) {
        double x = static_cast<double>(i);
        double y = recentTimes[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double n = static_cast<double>(recentTimes.size());
    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    
    if (slope > 0.01) return "DEGRADING";
    else if (slope < -0.01) return "IMPROVING";
    else return "STABLE";
}

void BenchmarkSuite::setMaxConcurrentBenchmarks(size_t count) {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    pImpl->maxConcurrentBenchmarks = count;
}

void BenchmarkSuite::setDefaultTimeout(std::chrono::milliseconds timeout) {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    pImpl->defaultTimeout = timeout;
}

void BenchmarkSuite::enableAutoCleanup(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    pImpl->autoCleanupEnabled = enable;
}

void BenchmarkSuite::setOutputDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    pImpl->outputDirectory = directory;
    std::filesystem::create_directories(pImpl->outputDirectory);
}

bool BenchmarkSuite::isBenchmarkRunning(const std::string& benchmarkName) const {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    return std::find(pImpl->runningBenchmarks.begin(), pImpl->runningBenchmarks.end(), 
                     benchmarkName) != pImpl->runningBenchmarks.end();
}

bool BenchmarkSuite::isBenchmarkSuiteRunning() const {
    return pImpl->suiteRunning && !pImpl->stopWorker;
}

void BenchmarkSuite::stopAllBenchmarks() {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    pImpl->suiteRunning = false;
    pImpl->suitePaused = false;
}

void BenchmarkSuite::pauseBenchmarks() {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    pImpl->suitePaused = true;
}

void BenchmarkSuite::resumeBenchmarks() {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    pImpl->suitePaused = false;
}

bool BenchmarkSuite::Impl::validateBenchmarkConfig(const BenchmarkConfig& config) const {
    // Validate benchmark configuration
    if (config.name.empty()) {
        NLM_LOG_ERROR("BenchmarkSuite: Benchmark name cannot be empty");
        return false;
    }
    
    if (benchmarkConfigs.find(config.name) != benchmarkConfigs.end()) {
        NLM_LOG_WARNING("BenchmarkSuite: Benchmark already exists: " + config.name);
    }
    
    return true;
}

std::vector<BenchmarkSuite::BenchmarkResult> BenchmarkSuite::runBenchmark(const std::string& benchmarkName) {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    
    std::vector<BenchmarkResult> results;
    
    if (pImpl->benchmarkFunctions.find(benchmarkName) == pImpl->benchmarkFunctions.end()) {
        NLM_LOG_ERROR("BenchmarkSuite: Benchmark not found: " + benchmarkName);
        return results;
    }
    
    if (isBenchmarkRunning(benchmarkName)) {
        NLM_LOG_WARNING("BenchmarkSuite: Benchmark already running: " + benchmarkName);
        return results;
    }
    
    pImpl->runningBenchmarks.push_back(benchmarkName);
    
    // Run benchmark in thread pool or async
    runBenchmarkAsync(benchmarkName);
    
    return results;
}

std::vector<BenchmarkSuite::BenchmarkResult> BenchmarkSuite::runAllBenchmarks() {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    
    std::vector<BenchmarkResult> results;
    
    // Add all enabled benchmarks to queue
    for (const auto& pair : pImpl->benchmarkConfigs) {
        if (pair.second.enabled) {
            pImpl->pendingBenchmarks.push(pair.first);
        }
    }
    
    // Wait for completion (simplified - in real implementation would wait)
    return results;
}

std::vector<BenchmarkSuite::BenchmarkResult> BenchmarkSuite::runBenchmarksByType(TestType type) {
    std::lock_guard<std::mutex> lock(pImpl->runningMutex);
    
    std::vector<BenchmarkResult> results;
    
    // Filter benchmarks by type (simplified - would need type information in config)
    for (const auto& pair : pImpl->benchmarkConfigs) {
        if (pair.second.enabled) {
            // Simple filtering based on name prefix
            std::string name = pair.first;
            if ((type == TestType::Configuration && name.find("config_") == 0) ||
                (type == TestType::Performance && name.find("perf_") == 0) ||
                (type == TestType::Stress && name.find("stress_") == 0) ||
                (type == TestType::Regression && name.find("reg_") == 0) ||
                (type == TestType::Comparison && name.find("comp_") == 0) ||
                (type == TestType::Load && name.find("load_") == 0)) {
                pImpl->pendingBenchmarks.push(pair.first);
            }
        }
    }
    
    return results;
}

void BenchmarkSuite::runBenchmarkAsync(const std::string& benchmarkName) {
    // Run benchmark in background thread
    std::thread([this, benchmarkName]() {
        try {
            if (pImpl->benchmarkFunctions.find(benchmarkName) == pImpl->benchmarkFunctions.end()) {
                return;
            }
            
            BenchmarkResult result(benchmarkName, 
                                 (pImpl->benchmarkConfigs.find(benchmarkName) != pImpl->benchmarkConfigs.end() ? 
                                  pImpl->benchmarkConfigs.at(benchmarkName).parameters["config"] : "default"));
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Execute benchmark function
            pImpl->benchmarkFunctions[benchmarkName](result);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
            
            {
                std::lock_guard<std::mutex> lock(pImpl->runningMutex);
                auto it = std::find(pImpl->runningBenchmarks.begin(), 
                                   pImpl->runningBenchmarks.end(), benchmarkName);
                if (it != pImpl->runningBenchmarks.end()) {
                    pImpl->runningBenchmarks.erase(it);
                }
            }
            
            {
                std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
                pImpl->completedResults.push_back(result);
            }
            
            pImpl->resultCondition.notify_one();
            
            // Export results if needed
            exportBenchmarkResults(benchmarkName);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("BenchmarkSuite: Error running benchmark " + benchmarkName + ": " + std::string(e.what()));
            
            {
                std::lock_guard<std::mutex> lock(pImpl->runningMutex);
                auto it = std::find(pImpl->runningBenchmarks.begin(), 
                                   pImpl->runningBenchmarks.end(), benchmarkName);
                if (it != pImpl->runningBenchmarks.end()) {
                    pImpl->runningBenchmarks.erase(it);
                }
            }
        }
    }).detach();
}

void BenchmarkSuite::processBenchmarkResults() {
    while (!pImpl->stopWorker && pImpl->suiteRunning) {
        {
            std::unique_lock<std::mutex> lock(pImpl->runningMutex);
            
            if (pImpl->pendingBenchmarks.empty()) {
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            
            // Check if we can start more benchmarks
            size_t runningCount = pImpl->runningBenchmarks.size();
            if (runningCount >= pImpl->maxConcurrentBenchmarks || pImpl->suitePaused) {
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            
            // Get next benchmark
            std::string benchmarkName = pImpl->pendingBenchmarks.front();
            pImpl->pendingBenchmarks.pop();
            
            // Add to running list
            pImpl->runningBenchmarks.push_back(benchmarkName);
            
            lock.unlock();
            
            // Run the benchmark
            runBenchmarkAsync(benchmarkName);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Prevent busy waiting
        }
    }
}

void BenchmarkSuite::cleanupCompletedBenchmarks() {
    if (!pImpl->autoCleanupEnabled) return;
    
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    // Keep only recent results (last 1000)
    if (pImpl->completedResults.size() > 1000) {
        pImpl->completedResults.erase(pImpl->completedResults.begin(),
                                     pImpl->completedResults.begin() + 
                                     (pImpl->completedResults.size() - 1000));
    }
}

void BenchmarkSuite::exportBenchmarkResults(const std::string& benchmarkName) {
    std::string filename = pImpl->outputDirectory + "/" + benchmarkName + "_results.json";
    saveResults(filename, "json");
}

} // namespace nlm
