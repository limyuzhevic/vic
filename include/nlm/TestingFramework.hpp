// Comprehensive NLM Testing and Benchmarking Framework
// This framework provides advanced testing capabilities for the NLM neural simulation system
// Includes performance benchmarking, stress testing, comparison testing, and automated test reporting

#ifndef NLM_TESTING_FRAMEWORK_HPP
#define NLM_TESTING_FRAMEWORK_HPP

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <algorithm>
#include <random>

namespace nlm {
namespace testing {

// Forward declarations
class BenchmarkSuite;
class PerformanceProfiler;
class MemoryProfiler;
class TestResult;
class TestSuite;

// Performance metrics collection
struct PerformanceMetrics {
    std::string testName;
    double executionTimeMs;
    double cpuTimeMs;
    double memoryUsageMB;
    size_t neuronsPerSecond;
    size_t spikesPerSecond;
    double realTimeFactor;
    size_t activeNeurons;
    size_t firingNeurons;
    size_t pendingSpikes;
    double avgFiringRate;
    double memoryUsageMB;
    double stepTimeMs;
    size_t totalSteps;
    size_t totalSynapses;
    
    PerformanceMetrics() : executionTimeMs(0.0), cpuTimeMs(0.0), memoryUsageMB(0.0),
                           neuronsPerSecond(0), spikesPerSecond(0), realTimeFactor(1.0),
                           activeNeurons(0), firingNeurons(0), pendingSpikes(0),
                           avgFiringRate(0.0), memoryUsageMB(0.0), stepTimeMs(0.0),
                           totalSteps(0), totalSynapses(0) {}
};

// Test result structure
struct TestResult {
    std::string testName;
    std::string testSuite;
    bool passed;
    std::string errorMessage;
    double executionTimeMs;
    std::unordered_map<std::string, std::string> metrics;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    TestResult() : passed(false), executionTimeMs(0.0) {}
    TestResult(const std::string& name, const std::string& suite)
        : testName(name), testSuite(suite), passed(false), executionTimeMs(0.0) {}
};

// Benchmarking configuration
struct BenchmarkConfig {
    size_t threadCount = std::thread::hardware_concurrency();
    bool enableProfiling = true;
    bool enableMemoryTracking = true;
    double minExecutionTime = 1000.0; // milliseconds
    size_t warmupSteps = 100;
    bool runInBackground = false;
    std::string outputFormat = "detailed"; // detailed, summary, json
    double confidenceThreshold = 0.95;
    size_t maxIterations = 10;
};

// Performance profiler for detailed timing analysis
class PerformanceProfiler {
public:
    PerformanceProfiler() : active_(false) {}
    
    void start() {
        startTime_ = std::chrono::high_resolution_clock::now();
        cpuStartTime_ = std::chrono::steady_clock::now();
        active_ = true;
    }
    
    void stop() {
        if (!active_) return;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto cpuEndTime = std::chrono::steady_clock::now();
        
        wallTimeMs_ = std::chrono::duration<double, std::milli>(endTime - startTime_).count();
        cpuTimeMs_ = std::chrono::duration<double, std::milli>(cpuEndTime - cpuStartTime_).count();
        
        active_ = false;
    }
    
    double getWallTimeMs() const { return wallTimeMs_; }
    double getCpuTimeMs() const { return cpuTimeMs_; }
    double getRealTimeFactor(double realTimeMs) const {
        return (realTimeMs > 0) ? (wallTimeMs_ / realTimeMs) : 1.0;
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::steady_clock::time_point cpuStartTime_;
    double wallTimeMs_ = 0.0;
    double cpuTimeMs_ = 0.0;
    bool active_ = false;
};

// Memory profiler for memory usage tracking
class MemoryProfiler {
public:
    struct MemorySample {
        double timestamp;
        double memoryUsageMB;
        size_t allocatedObjects;
        size_t deallocatedObjects;
    };
    
    MemoryProfiler() : tracking_(false) {}
    
    void startTracking() {
        tracking_ = true;
        samples_.clear();
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void stopTracking() {
        tracking_ = false;
    }
    
    void takeSample() {
        if (!tracking_) return;
        
        auto now = std::chrono::high_resolution_clock::now();
        double timestamp = std::chrono::duration<double>(now - startTime_).count();
        
        MemorySample sample;
        sample.timestamp = timestamp;
        sample.memoryUsageMB = getCurrentMemoryUsage();
        sample.allocatedObjects = allocatedObjects_;
        sample.deallocatedObjects = deallocatedObjects_;
        
        samples_.push_back(sample);
    }
    
    double getPeakMemoryUsage() const {
        if (samples_.empty()) return 0.0;
        double peak = 0.0;
        for (const auto& sample : samples_) {
            peak = std::max(peak, sample.memoryUsageMB);
        }
        return peak;
    }
    
    double getAverageMemoryUsage() const {
        if (samples_.empty()) return 0.0;
        double total = 0.0;
        for (const auto& sample : samples_) {
            total += sample.memoryUsageMB;
        }
        return total / samples_.size();
    }
    
    const std::vector<MemorySample>& getSamples() const { return samples_; }
    
private:
    double getCurrentMemoryUsage() {
        // Placeholder - in real implementation, would use platform-specific APIs
        // For now, return a dummy value
        return 0.0;
    }
    
    bool tracking_;
    std::vector<MemorySample> samples_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::atomic<size_t> allocatedObjects_ = 0;
    std::atomic<size_t> deallocatedObjects_ = 0;
};

// Benchmark suite for running performance benchmarks
class BenchmarkSuite {
public:
    BenchmarkSuite(const std::string& name) : name_(name) {}
    
    void addBenchmark(const std::string& testName, 
                     std::function<void(std::shared_ptr<Brain>)> testFunc,
                     const BenchmarkConfig& config = BenchmarkConfig()) {
        benchmarks_.push_back({testName, testFunc, config});
    }
    
    std::vector<TestResult> runAll(std::shared_ptr<Brain> brain) {
        std::vector<TestResult> results;
        
        for (const auto& benchmark : benchmarks_) {
            TestResult result = runBenchmark(brain, benchmark);
            results.push_back(result);
        }
        
        return results;
    }
    
    TestResult runBenchmark(std::shared_ptr<Brain> brain, 
                           const std::function<void(std::shared_ptr<Brain>)>& testFunc,
                           const BenchmarkConfig& config) {
        TestResult result(benchmarkName, name_);
        result.startTime = std::chrono::system_clock::now();
        
        try {
            // Warmup
            for (size_t i = 0; i < config.warmupSteps; ++i) {
                testFunc(brain);
            }
            
            PerformanceProfiler profiler;
            profiler.start();
            
            // Run benchmark
            for (size_t i = 0; i < config.maxIterations; ++i) {
                testFunc(brain);
            }
            
            profiler.stop();
            
            result.endTime = std::chrono::system_clock::now();
            result.executionTimeMs = profiler.getWallTimeMs();
            result.passed = true;
            
            // Record metrics
            result.metrics["cpu_time_ms"] = std::to_string(profiler.getCpuTimeMs());
            result.metrics["wall_time_ms"] = std::to_string(profiler.getWallTimeMs());
            
        } catch (const std::exception& e) {
            result.passed = false;
            result.errorMessage = e.what();
            NLM_LOG_ERROR(std::string("Benchmark failed: ") + e.what());
        }
        
        return result;
    }
    
    const std::string& getName() const { return name_; }
    
private:
    struct Benchmark {
        std::string testName;
        std::function<void(std::shared_ptr<Brain>)> testFunc;
        BenchmarkConfig config;
    };
    
    std::string name_;
    std::vector<Benchmark> benchmarks_;
    std::string benchmarkName = "Unnamed Benchmark";
};

// Comparison tester for comparing different configurations
class ComparisonTester {
public:
    struct ComparisonConfig {
        std::vector<std::shared_ptr<Config>> configs;
        std::string testName;
        size_t stepsPerConfig = 1000;
        bool detailedComparison = true;
    };
    
    ComparisonTester() = default;
    
    std::vector<std::unordered_map<std::string, std::string>> compare(
        const std::vector<std::shared_ptr<Config>>& configs,
        const std::string& testName,
        size_t stepsPerConfig = 1000) {
        
        ComparisonConfig config;
        config.configs = configs;
        config.testName = testName;
        config.stepsPerConfig = stepsPerConfig;
        
        return compare(config);
    }
    
    std::vector<std::unordered_map<std::string, std::string>> compare(
        const ComparisonConfig& config) {
        
        std::vector<std::unordered_map<std::string, std::string>> results;
        
        for (const auto& brainConfig : config.configs) {
            auto brain = std::make_shared<Brain>(brainConfig);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain for comparison test");
                continue;
            }
            
            // Run test with this configuration
            auto result = runComparisonTest(brain, config.testName, config.stepsPerConfig);
            results.push_back(result);
        }
        
        return results;
    }
    
private:
    std::unordered_map<std::string, std::string> runComparisonTest(
        std::shared_ptr<Brain> brain,
        const std::string& testName,
        size_t steps) {
        
        std::unordered_map<std::string, std::string> result;
        result["test_name"] = testName;
        
        PerformanceProfiler profiler;
        profiler.start();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation steps
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
            
            // Record performance metrics every 100 steps
            if (step % 100 == 0) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                double elapsedMs = std::chrono::duration<double, std::milli>(currentTime - startTime).count();
                
                result["steps_per_second"] = std::to_string(step / (elapsedMs / 1000.0));
                result["current_step"] = std::to_string(step);
                result["total_neurons"] = std::to_string(brain->getTotalNeuronCount());
                result["total_synapses"] = std::to_string(brain->getTotalSynapseCount());
                result["spike_count"] = std::to_string(brain->getTotalSpikeCount());
            }
        }
        
        profiler.stop();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        result["total_time_ms"] = std::to_string(totalTimeMs);
        result["steps_per_second"] = std::to_string(steps / (totalTimeMs / 1000.0));
        result["avg_step_time_ms"] = std::to_string(totalTimeMs / steps);
        result["final_neuron_count"] = std::to_string(brain->getTotalNeuronCount());
        result["final_synapse_count"] = std::to_string(brain->getTotalSynapseCount());
        result["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
        
        return result;
    }
};

// Stress tester for testing system limits
class StressTester {
public:
    struct StressTestConfig {
        size_t maxNeurons = 10000;
        double maxMemoryUsageMB = 2048.0;
        double maxStepTimeMs = 1000.0;
        bool testMemoryLimits = true;
        bool testPerformanceLimits = true;
        bool testNetworkLimits = true;
    };
    
    StressTester() = default;
    
    std::vector<TestResult> runStressTests(const StressTestConfig& config) {
        std::vector<TestResult> results;
        
        if (config.testMemoryLimits) {
            results.push_back(testMemoryLimits(config));
        }
        
        if (config.testPerformanceLimits) {
            results.push_back(testPerformanceLimits(config));
        }
        
        if (config.testNetworkLimits) {
            results.push_back(testNetworkLimits(config));
        }
        
        return results;
    }
    
private:
    TestResult testMemoryLimits(const StressTestConfig& config) {
        TestResult result("Memory Limit Test", "Stress Testing");
        result.startTime = std::chrono::system_clock::now();
        
        try {
            // Test with increasing neuron counts until limits are reached
            for (size_t neuronCount : {1000, 5000, 10000, 20000}) {
                if (neuronCount > config.maxNeurons) break;
                
                auto brainConfig = std::make_shared<Config>();
                brainConfig->set("neuron_count", static_cast<int64_t>(neuronCount),
                               ConfigSource::Default);
                brainConfig->set("region_count", static_cast<int64_t>(1),
                               ConfigSource::Default);
                
                auto brain = std::make_shared<Brain>(brainConfig);
                if (!brain->initialize()) {
                    result.passed = false;
                    result.errorMessage = "Failed to initialize brain with " + std::to_string(neuronCount) + " neurons";
                    break;
                }
                
                // Run some steps
                for (size_t step = 0; step < 100; ++step) {
                    brain->step(step);
                }
                
                // Check memory usage (placeholder)
                // In real implementation, would use platform-specific memory monitoring
            }
            
            result.endTime = std::chrono::system_clock::now();
            result.passed = true;
            
        } catch (const std::exception& e) {
            result.passed = false;
            result.errorMessage = e.what();
        }
        
        return result;
    }
    
    TestResult testPerformanceLimits(const StressTestConfig& config) {
        TestResult result("Performance Limit Test", "Stress Testing");
        result.startTime = std::chrono::system_clock::now();
        
        try {
            auto brainConfig = std::make_shared<Config>();
            brainConfig->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
            brainConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(brainConfig);
            if (!brain->initialize()) {
                result.passed = false;
                result.errorMessage = "Failed to initialize brain for performance test";
                return result;
            }
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Run many steps to test performance limits
            size_t stepsCompleted = 0;
            for (size_t step = 0; step < 100000; ++step) {
                brain->step(step);
                stepsCompleted++;
                
                auto currentTime = std::chrono::high_resolution_clock::now();
                double elapsedMs = std::chrono::duration<double, std::milli>(currentTime - startTime).count();
                
                if (elapsedMs > config.maxStepTimeMs * 10) { // Allow some overhead
                    result.passed = false;
                    result.errorMessage = "Step time exceeded limit";
                    result.metrics["steps_completed"] = std::to_string(stepsCompleted);
                    result.metrics["elapsed_time_ms"] = std::to_string(elapsedMs);
                    break;
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double totalTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            
            result.endTime = endTime;
            result.passed = true;
            result.metrics["steps_completed"] = std::to_string(stepsCompleted);
            result.metrics["total_time_ms"] = std::to_string(totalTimeMs);
            result.metrics["steps_per_second"] = std::to_string(stepsCompleted / (totalTimeMs / 1000.0));
            
        } catch (const std::exception& e) {
            result.passed = false;
            result.errorMessage = e.what();
        }
        
        return result;
    }
    
    TestResult testNetworkLimits(const StressTestConfig& config) {
        TestResult result("Network Limit Test", "Stress Testing");
        result.startTime = std::chrono::system_clock::now();
        
        try {
            // Test with increasing connection probabilities
            for (double connProb : {0.01, 0.05, 0.1, 0.2}) {
                auto brainConfig = std::make_shared<Config>();
                brainConfig->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
                brainConfig->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
                brainConfig->set("connection_probability", connProb, ConfigSource::Default);
                
                auto brain = std::make_shared<Brain>(brainConfig);
                if (!brain->initialize()) {
                    result.passed = false;
                    result.errorMessage = "Failed to initialize brain with connection probability " + std::to_string(connProb);
                    break;
                }
                
                // Run simulation
                for (size_t step = 0; step < 500; ++step) {
                    brain->step(step);
                }
                
                result.metrics["conn_prob_" + std::to_string(connProb)] = std::to_string(brain->getTotalSynapseCount());
            }
            
            result.endTime = std::chrono::system_clock::now();
            result.passed = true;
            
        } catch (const std::exception& e) {
            result.passed = false;
            result.errorMessage = e.what();
        }
        
        return result;
    }
};

// Test suite for managing collections of tests
class TestSuite {
public:
    TestSuite(const std::string& name) : name_(name) {}
    
    void addTest(const std::string& testName,
                std::function<void()> testFunc,
                const std::string& category = "general") {
        tests_.push_back({testName, testFunc, category});
    }
    
    std::vector<TestResult> runAll() {
        std::vector<TestResult> results;
        
        for (const auto& test : tests_) {
            TestResult result(test.testName, name_);
            result.startTime = std::chrono::system_clock::now();
            
            try {
                test.testFunc();
                result.endTime = std::chrono::system_clock::now();
                result.passed = true;
                result.executionTimeMs = 
                    std::chrono::duration<double, std::milli>(result.endTime - result.startTime).count();
                
            } catch (const std::exception& e) {
                result.endTime = std::chrono::system_clock::now();
                result.passed = false;
                result.errorMessage = e.what();
                result.executionTimeMs = 
                    std::chrono::duration<double, std::milli>(result.endTime - result.startTime).count();
                
                NLM_LOG_ERROR(std::string("Test failed: ") + e.what());
            }
            
            results.push_back(result);
        }
        
        return results;
    }
    
    const std::string& getName() const { return name_; }
    
private:
    struct Test {
        std::string testName;
        std::function<void()> testFunc;
        std::string category;
    };
    
    std::string name_;
    std::vector<Test> tests_;
};

// Test reporter for generating test reports
class TestReporter {
public:
    enum class ReportFormat {
        TEXT,
        JSON,
        CSV,
        HTML
    };
    
    static void generateReport(const std::vector<TestResult>& results,
                               const std::string& filename,
                               ReportFormat format = ReportFormat::TEXT) {
        switch (format) {
            case ReportFormat::TEXT:
                generateTextReport(results, filename);
                break;
            case ReportFormat::JSON:
                generateJsonReport(results, filename);
                break;
            case ReportFormat::CSV:
                generateCsvReport(results, filename);
                break;
            case ReportFormat::HTML:
                generateHtmlReport(results, filename);
                break;
        }
    }
    
    static void printSummary(const std::vector<TestResult>& results) {
        size_t totalTests = results.size();
        size_t passedTests = 0;
        double totalTime = 0.0;
        
        for (const auto& result : results) {
            if (result.passed) passedTests++;
            totalTime += result.executionTimeMs;
        }
        
        double successRate = (totalTests > 0) ? (passedTests * 100.0 / totalTests) : 0.0;
        
        std::cout << "=== NLM Test Suite Summary ===" << std::endl;
        std::cout << "Total Tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << (totalTests - passedTests) << std::endl;
        std::cout << "Success Rate: " << std::fixed << std::setprecision(2) << successRate << "%" << std::endl;
        std::cout << "Total Execution Time: " << std::fixed << std::setprecision(2) << totalTime << " ms" << std::endl;
        std::cout << "Average Test Time: " << std::fixed << std::setprecision(2) << (totalTests > 0 ? totalTime / totalTests : 0.0) << " ms" << std::endl;
    }
    
private:
    static void generateTextReport(const std::vector<TestResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open report file: " + filename);
            return;
        }
        
        file << "=== NLM Test Suite Report ===" << std::endl << std::endl;
        
        for (const auto& result : results) {
            file << "Test: " << result.testName << " (Suite: " << result.testSuite << ")" << std::endl;
            file << "Status: " << (result.passed ? "PASSED" : "FAILED") << std::endl;
            if (!result.errorMessage.empty()) {
                file << "Error: " << result.errorMessage << std::endl;
            }
            file << "Execution Time: " << std::fixed << std::setprecision(2) << result.executionTimeMs << " ms" << std::endl;
            file << std::endl;
        }
        
        file.close();
    }
    
    static void generateJsonReport(const std::vector<TestResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open report file: " + filename);
            return;
        }
        
        file << "{" << std::endl;
        file << "  \"test_suite\": \"NLM Tests\"," << std::endl;
        file << "  \"total_tests\": " << results.size() << "," << std::endl;
        
        size_t passedTests = 0;
        for (const auto& result : results) {
            if (result.passed) passedTests++;
        }
        
        file << "  \"passed_tests\": " << passedTests << "," << std::endl;
        file << "  \"results\": [" << std::endl;
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            file << "    {" << std::endl;
            file << "      \"name\": \"" << result.testName << "\"," << std::endl;
            file << "      \"suite\": \"" << result.testSuite << "\"," << std::endl;
            file << "      \"passed\": " << (result.passed ? "true" : "false") << "," << std::endl;
            if (!result.errorMessage.empty()) {
                file << "      \"error\": \"" << result.errorMessage << "\"," << std::endl;
            }
            file << "      \"execution_time_ms\": " << std::fixed << std::setprecision(2) << result.executionTimeMs"";
            if (i < results.size() - 1) {
                file << "," << std::endl;
            } else {
                file << std::endl;
            }
            file << "    }";
            if (i < results.size() - 1) {
                file << "," << std::endl;
            } else {
                file << std::endl;
            }
        }
        
        file << "  ]" << std::endl;
        file << "}" << std::endl;
        
        file.close();
    }
    
    static void generateCsvReport(const std::vector<TestResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open report file: " + filename);
            return;
        }
        
        file << "Test Name,Test Suite,Status,Execution Time (ms),Error Message" << std::endl;
        
        for (const auto& result : results) {
            file << "\"" << result.testName << "\",\"" << result.testSuite << "\",";
            file << (result.passed ? "PASSED" : "FAILED") << ",";
            file << std::fixed << std::setprecision(2) << result.executionTimeMs << ",";
            if (!result.errorMessage.empty()) {
                // Escape quotes in error message
                std::string errorMsg = result.errorMessage;
                size_t pos = 0;
                while ((pos = errorMsg.find('"', pos)) != std::string::npos) {
                    errorMsg.replace(pos, 1, "\"\"");
                    pos += 2;
                }
                file << "\"" << errorMsg << "\"";
            }
            file << std::endl;
        }
        
        file.close();
    }
    
    static void generateHtmlReport(const std::vector<TestResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open report file: " + filename);
            return;
        }
        
        file << "<!DOCTYPE html>" << std::endl;
        file << "<html><head><title>NLM Test Report</title></head><body>" << std::endl;
        file << "<h1>NLM Test Suite Report</h1>" << std::endl;
        file << "<table border=\"1\">" << std::endl;
        file << "<tr><th>Test Name</th><th>Test Suite</th><th>Status</th><th>Execution Time (ms)</th><th>Error Message</th></tr>" << std::endl;
        
        for (const auto& result : results) {
            file << "<tr>" << std::endl;
            file << "<td>" << result.testName << "</td>" << std::endl;
            file << "<td>" << result.testSuite << "</td>" << std::endl;
            file << "<td>" << (result.passed ? "<span style=\"color: green;\">PASSED</span>" : "<span style=\"color: red;\">FAILED</span>") << "</td>" << std::endl;
            file << "<td>" << std::fixed << std::setprecision(2) << result.executionTimeMs << "</td>" << std::endl;
            if (!result.errorMessage.empty()) {
                // Escape HTML special characters
                std::string errorMsg = result.errorMessage;
                size_t pos = 0;
                while ((pos = errorMsg.find('&', pos)) != std::string::npos) {
                    errorMsg.replace(pos, 1, "&amp;");
                    pos += 5;
                }
                while ((pos = errorMsg.find('<', pos)) != std::string::npos) {
                    errorMsg.replace(pos, 1, "&lt;");
                    pos += 4;
                }
                while ((pos = errorMsg.find('>', pos)) != std::string::npos) {
                    errorMsg.replace(pos, 1, "&gt;");
                    pos += 4;
                }
                file << "<td>" << errorMsg << "</td>" << std::endl;
            } else {
                file << "<td></td>" << std::endl;
            }
            file << "</tr>" << std::endl;
        }
        
        file << "</table></body></html>" << std::endl;
        
        file.close();
    }
};

} // namespace testing
} // namespace nlm

#endif // NLM_TESTING_FRAMEWORK_HPP