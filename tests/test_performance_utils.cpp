// test_performance_utils.cpp - Performance testing utilities
#include "TestFramework.hpp"
#include "TestFixture.hpp"
#include "test_utils.cpp"
#include <algorithm>
#include <numeric>
#include <map>
#include <set>
#include <cmath>
#include <thread>
#include <chrono>

namespace test_performance_utils {

// Performance benchmarking
class PerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string operation;
        double executionTimeMs;
        size_t memoryUsage;
        double throughput; // operations per second
        double efficiency; // ratio of actual to theoretical maximum
        
        BenchmarkResult(const std::string& op, double time, size_t mem, double throughput = 0.0, double efficiency = 0.0)
            : operation(op), executionTimeMs(time), memoryUsage(mem), throughput(throughput), efficiency(efficiency) {}
    };
    
    static PerformanceBenchmark& getInstance() {
        static PerformanceBenchmark instance;
        return instance;
    }
    
    void startBenchmark(const std::string& operation) {
        auto it = benchmarks_.find(operation);
        if (it != benchmarks_.end()) {
            it->second.startTime = std::chrono::steady_clock::now();
        } else {
            BenchmarkResult result(operation, 0.0, 0, 0.0, 0.0);
            result.startTime = std::chrono::steady_clock::now();
            benchmarks_[operation] = result;
        }
    }
    
    void endBenchmark(const std::string& operation, size_t operationCount = 1) {
        auto it = benchmarks_.find(operation);
        if (it != benchmarks_.end()) {
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(endTime - it->second.startTime).count();
            
            it->second.executionTimeMs = duration;
            if (duration > 0.0) {
                it->second.throughput = (static_cast<double>(operationCount) / duration) * 1000.0; // ops/ms
            }
            
            benchmarks_[operation] = it->second;
        }
    }
    
    double getExecutionTime(const std::string& operation) const {
        auto it = benchmarks_.find(operation);
        if (it != benchmarks_.end()) {
            return it->second.executionTimeMs;
        }
        return 0.0;
    }
    
    size_t getMemoryUsage(const std::string& operation) const {
        auto it = benchmarks_.find(operation);
        if (it != benchmarks_.end()) {
            return it->second.memoryUsage;
        }
        return 0;
    }
    
    double getThroughput(const std::string& operation) const {
        auto it = benchmarks_.find(operation);
        if (it != benchmarks_.end()) {
            return it->second.throughput;
        }
        return 0.0;
    }
    
    std::vector<BenchmarkResult> getAllBenchmarks() const {
        std::vector<BenchmarkResult> result;
        for (const auto& pair : benchmarks_) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    void clear() {
        benchmarks_.clear();
    }
    
private:
    PerformanceBenchmark() = default;
    
    struct BenchmarkWithStartTime : public BenchmarkResult {
        std::chrono::steady_clock::time_point startTime;
        
        BenchmarkWithStartTime(const std::string& op, double time, size_t mem, double throughput, double efficiency)
            : BenchmarkResult(op, time, mem, throughput, efficiency) {}
    };
    
    std::unordered_map<std::string, BenchmarkWithStartTime> benchmarks_;
};

// Scalability testing
class ScalabilityTester {
public:
    struct ScalabilityMetrics {
        size_t inputSize;
        double executionTimeMs;
        size_t memoryUsage;
        double scalabilityFactor; // performance per unit size
        double memoryEfficiency; // memory usage per unit size
        
        ScalabilityMetrics(size_t size, double time, size_t mem, double sf = 0.0, double me = 0.0)
            : inputSize(size), executionTimeMs(time), memoryUsage(mem), 
              scalabilityFactor(sf), memoryEfficiency(me) {}
    };
    
    static ScalabilityTester& getInstance() {
        static ScalabilityTester instance;
        return instance;
    }
    
    std::vector<ScalabilityMetrics> testScalability(const std::vector<size_t>& sizes, 
                                                   std::function<void(size_t)> testFunction) {
        std::vector<ScalabilityMetrics> results;
        
        for (size_t size : sizes) {
            PerformanceTimer timer;
            testFunction(size);
            double executionTime = timer.elapsedMs();
            size_t memoryUsage = PerformanceMonitor::getInstance().getCurrentMemoryUsage();
            
            double scalabilityFactor = (size > 0) ? (1.0 / executionTime) : 0.0;
            double memoryEfficiency = (size > 0) ? (1.0 / memoryUsage) : 0.0;
            
            results.emplace_back(size, executionTime, memoryUsage, scalabilityFactor, memoryEfficiency);
        }
        
        return results;
    }
    
    std::string analyzeScalability(const std::vector<ScalabilityMetrics>& metrics) {
        std::stringstream analysis;
        analysis << "Scalability Analysis\n";
        analysis << "=====================\n\n";
        
        if (metrics.empty()) {
            analysis << "No scalability data available.\n";
            return analysis.str();
        }
        
        analysis << "Input Size vs Performance:\n";
        for (const auto& metric : metrics) {
            analysis << metric.inputSize << " -> " << metric.executionTimeMs << " ms, " 
                     << metric.memoryUsage << " bytes\n";
        }
        
        analysis << "\n\nScalability Factors:\n";
        double avgScalabilityFactor = 0.0;
        for (const auto& metric : metrics) {
            analysis << metric.inputSize << ": " << metric.scalabilityFactor << "\n";
            avgScalabilityFactor += metric.scalabilityFactor;
        }
        avgScalabilityFactor /= metrics.size();
        analysis << "Average: " << avgScalabilityFactor << "\n";
        
        analysis << "\n\nMemory Efficiency:\n";
        double avgMemoryEfficiency = 0.0;
        for (const auto& metric : metrics) {
            analysis << metric.inputSize << ": " << metric.memoryEfficiency << "\n";
            avgMemoryEfficiency += metric.memoryEfficiency;
        }
        avgMemoryEfficiency /= metrics.size();
        analysis << "Average: " << avgMemoryEfficiency << "\n";
        
        // Determine scalability pattern
        if (metrics.size() >= 3) {
            bool isLinear = true;
            bool isSuperLinear = true;
            bool isSubLinear = true;
            
            for (size_t i = 1; i < metrics.size() - 1; ++i) {
                double ratio = metrics[i].executionTimeMs / metrics[i-1].executionTimeMs;
                double sizeRatio = static_cast<double>(metrics[i].inputSize) / metrics[i-1].inputSize;
                
                if (std::abs(ratio - sizeRatio) > 0.1) {
                    isLinear = false;
                }
                
                if (ratio > sizeRatio * 1.5) {
                    isSubLinear = false;
                }
                
                if (ratio < sizeRatio * 0.5) {
                    isSuperLinear = false;
                }
            }
            
            analysis << "\n\nScalability Pattern:\n";
            if (isLinear) analysis << "  Linear (O(n))\n";
            if (isSuperLinear) analysis << "  Super-linear (better than O(n))\n";
            if (isSubLinear) analysis << "  Sub-linear (better than linear)\n";
            if (!isLinear && !isSuperLinear && !isSubLinear) {
                analysis << "  Non-standard scaling\n";
            }
        }
        
        return analysis.str();
    }
    
private:
    ScalabilityTester() = default;
};

// Stress testing
class StressTester {
public:
    struct StressTestConfig {
        size_t threadCount = 1;
        double durationSeconds = 10.0;
        size_t memoryLimitBytes = 1024 * 1024; // 1 MB
        double timeLimitSeconds = 60.0;
        bool enableMemoryLeakDetection = true;
        bool enableResourceCleanup = true;
        bool enablePerformanceMonitoring = true;
        bool enableTimeoutDetection = true;
    };
    
    struct StressTestResult {
        std::string testName;
        bool passed;
        double executionTimeMs;
        size_t memoryUsage;
        std::string errorMessage;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        
        StressTestResult(const std::string& name) 
            : testName(name), passed(false), executionTimeMs(0.0), memoryUsage(0) {}
    };
    
    static StressTester& getInstance() {
        static StressTester instance;
        return instance;
    }
    
    void runStressTest(const StressTestConfig& config, 
                      const std::string& testName,
                      std::function<void()> testFunction) {
        StressTestResult result(testName);
        result.startTime = std::chrono::steady_clock::now();
        
        std::atomic<bool> timeout(false);
        std::thread timeoutThread;
        
        if (config.enableTimeoutDetection) {
            timeoutThread = std::thread([&]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(config.timeLimitSeconds * 1000)));
                timeout = true;
            });
        }
        
        std::vector<std::thread> threads;
        
        // Reset monitors
        if (config.enableMemoryLeakDetection) {
            MemoryLeakDetector::getInstance().clear();
        }
        
        if (config.enablePerformanceMonitoring) {
            PerformanceProfiler::getInstance().clear();
        }
        
        if (config.enableResourceCleanup) {
            ResourceCleanupTracker::getInstance().clear();
        }
        
        // Run test function
        try {
            if (config.threadCount == 1) {
                testFunction();
            } else {
                std::vector<std::thread> workerThreads;
                std::atomic<int> completedWorkers{0};
                
                for (size_t i = 0; i < config.threadCount; ++i) {
                    workerThreads.emplace_back([&]() {
                        testFunction();
                        completedWorkers++;
                    });
                }
                
                for (auto& thread : workerThreads) {
                    thread.join();
                }
            }
        } catch (const std::exception& e) {
            result.errorMessage = e.what();
        } catch (...) {
            result.errorMessage = "Unknown exception";
        }
        
        // Cancel timeout thread
        if (timeoutThread.joinable()) {
            timeoutThread.join();
        }
        
        result.endTime = std::chrono::steady_clock::now();
        result.executionTimeMs = std::chrono::duration<double, std::milli>(result.endTime - result.startTime).count();
        
        // Check for failures
        if (timeout) {
            result.errorMessage = "Test timed out";
        }
        
        if (config.enableMemoryLeakDetection && MemoryLeakDetector::getInstance().getLeakedBytes() > 0) {
            result.errorMessage = "Memory leak detected: " + 
                                 std::to_string(MemoryLeakDetector::getInstance().getLeakedBytes()) + " bytes leaked";
        }
        
        if (config.enableResourceCleanup && 
            !ResourceCleanupTracker::getInstance().getUncleanedResources().empty()) {
            if (!result.errorMessage.empty()) {
                result.errorMessage += "; ";
            }
            result.errorMessage += "Resource cleanup failed";
        }
        
        result.passed = result.errorMessage.empty();
        
        // Print result
        std::cout << "Stress Test: " << testName << " - " 
                  << (result.passed ? "PASSED" : "FAILED") << std::endl;
        if (!result.errorMessage.empty()) {
            std::cout << "  Error: " << result.errorMessage << std::endl;
        }
        std::cout << "  Execution time: " << result.executionTimeMs << " ms" << std::endl;
        
        // Store result (implementation dependent)
        // For now, just print to console
    }
    
    // Specific stress test implementations
    static void testMemoryPressure(size_t neuronCount, nlm::SimulationStep steps) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        runBrainSteps(brain, steps);
    }
    
    static void testTimePressure(double durationSeconds, nlm::SimulationStep stepCount) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto startTime = std::chrono::steady_clock::now();
        for (nlm::SimulationStep step = 0; step < stepCount; ++step) {
            brain.step(step);
            
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<double>(currentTime - startTime).count();
            
            if (elapsed >= durationSeconds) {
                break;
            }
        }
    }
    
    static void testConcurrency(size_t threadCount, nlm::SimulationStep stepsPerThread) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        std::vector<std::thread> threads;
        std::atomic<int> completedThreads{0};
        
        for (size_t i = 0; i < threadCount; ++i) {
            threads.emplace_back([&]() {
                for (nlm::SimulationStep step = 0; step < stepsPerThread; ++step) {
                    brain.step(step);
                }
                completedThreads++;
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
    
private:
    StressTester() = default;
};

// Memory monitoring
class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    size_t getCurrentMemoryUsage() const {
        // Implementation depends on platform
        // For now, return a placeholder value
        return 0;
    }
    
    double getCPUTime() const {
        // Implementation depends on platform
        // For now, return a placeholder value
        return 0.0;
    }
    
    std::map<std::string, double> getAllResourceUsage() const {
        std::map<std::string, double> usage;
        usage["memory"] = getCurrentMemoryUsage();
        usage["cpu"] = getCPUTime();
        return usage;
    }
    
    std::string getResourceUsageReport() const {
        std::stringstream report;
        report << "Resource Usage Report\n";
        report << "=====================\n\n";
        
        auto usage = getAllResourceUsage();
        for (const auto& pair : usage) {
            report << pair.first << ": " << pair.second << "\n";
        }
        
        return report.str();
    }
    
private:
    PerformanceMonitor() = default;
};

} // namespace test_performance_utils