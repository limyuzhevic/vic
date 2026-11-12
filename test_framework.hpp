// Comprehensive Testing Framework for NLM
// Provides unit testing, integration testing, and performance benchmarking
// for the Neural Learning Machine project

#pragma once

#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include <chrono>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <memory>

namespace test {

// Test result structure
struct TestResult {
    std::string name;
    bool passed;
    double executionTime;  // in seconds
    std::string message;
    std::string errorDetails;
    
    TestResult(const std::string& testName, bool success, double time,
               const std::string& msg = "", const std::string& error = "")
        : name(testName), passed(success), executionTime(time),
          message(msg), errorDetails(error) {}
};

// Benchmark result structure
struct BenchmarkResult {
    std::string benchmarkName;
    double executionTime;  // total time
    double avgStepTime;   // time per step
    size_t stepsExecuted;
    size_t spikesGenerated;
    float finalFiringRate;
    float memoryUsage;    // in MB
    
    BenchmarkResult(const std::string& name, double totalTime, double avgStep,
                   size_t steps, size_t spikes, float firingRate, float memory)
        : benchmarkName(name), executionTime(totalTime), avgStepTime(avgStep),
          stepsExecuted(steps), spikesGenerated(spikes), finalFiringRate(firingRate),
          memoryUsage(memory) {}
};

// Performance profiler for timing critical sections
class PerformanceProfiler {
private:
    struct TimingData {
        std::string sectionName;
        double totalTime;
        int callCount;
        double avgTime;
    };
    
    std::map<std::string, TimingData> timingData;
    bool enabled;
    
public:
    PerformanceProfiler() : enabled(false) {}
    
    void enable(bool enable = true) { this->enabled = enable; }
    
    void startTiming(const std::string& section) {
        if (!enabled) return;
        // Implementation would capture start time
    }
    
    void stopTiming(const std::string& section) {
        if (!enabled) return;
        // Implementation would calculate elapsed time and update statistics
    }
    
    void printResults() const {
        if (!enabled) return;
        std::cout << "=== Performance Profiling Results ===" << std::endl;
        for (const auto& pair : timingData) {
            const TimingData& data = pair.second;
            std::cout << pair.first << ": " << data.totalTime << "s total, "
                      << data.callCount << " calls, " << data.avgTime << "s avg" << std::endl;
        }
    }
};

// Test framework base class
class TestCase {
protected:
    std::shared_ptr<Logger> logger;
    
public:
    TestCase() {
        logger = Logger::getGlobal();
        if (!logger) {
            logger = std::make_shared<Logger>();
            Logger::setGlobal(logger);
            auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
            logger->addLogger(consoleLogger);
        }
    }
    
    virtual ~TestCase() = default;
    
    virtual std::string getName() const = 0;
    virtual void run() = 0;
    
    void logInfo(const std::string& message) {
        if (logger) {
            NLM_LOG_INFO(message);
        }
    }
    
    void logError(const std::string& message) {
        if (logger) {
            NLM_LOG_ERROR(message);
        }
    }
    
    void logDebug(const std::string& message) {
        if (logger) {
            NLM_LOG_DEBUG(message);
        }
    }
};

// Unit test framework for individual components
class UnitTestFramework {
private:
    std::vector<std::shared_ptr<TestCase>> testCases;
    PerformanceProfiler profiler;
    std::vector<TestResult> results;
    std::vector<BenchmarkResult> benchmarks;
    
public:
    UnitTestFramework() {
        profiler.enable(true);
    }
    
    void addTest(std::shared_ptr<TestCase> test) {
        testCases.push_back(test);
    }
    
    void runAllTests() {
        std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
        std::cout << "Running " << testCases.size() << " test cases..." << std::endl << std::endl;
        
        double totalTestTime = 0.0;
        
        for (const auto& test : testCases) {
            std::cout << "Running test: " << test->getName() << "..." << std::endl;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            try {
                test->run();
                auto endTime = std::chrono::high_resolution_clock::now();
                double elapsed = std::chrono::duration<double>(endTime - startTime).count();
                totalTestTime += elapsed;
                
                TestResult result(test->getName(), true, elapsed, "Test completed successfully.");
                results.push_back(result);
                
                std::cout << "  ✓ PASSED (" << elapsed << "s)" << std::endl;
            } catch (const std::exception& e) {
                auto endTime = std::chrono::high_resolution_clock::now();
                double elapsed = std::chrono::duration<double>(endTime - startTime).count();
                totalTestTime += elapsed;
                
                std::string errorMsg = "Exception: " + std::string(e.what());
                TestResult result(test->getName(), false, elapsed, "Test failed.", errorMsg);
                results.push_back(result);
                
                std::cout << "  ✗ FAILED (" << elapsed << "s)" << std::endl;
                std::cout << "    Error: " << errorMsg << std::endl;
            } catch (...) {
                auto endTime = std::chrono::high_resolution_clock::now();
                double elapsed = std::chrono::duration<double>(endTime - startTime).count();
                totalTestTime += elapsed;
                
                TestResult result(test->getName(), false, elapsed, "Test failed with unknown error.");
                results.push_back(result);
                
                std::cout << "  ✗ FAILED (" << elapsed << "s)" << std::endl;
                std::cout << "    Error: Unknown exception" << std::endl;
            }
            
            std::cout << std::endl;
        }
        
        printTestSummary(totalTestTime);
        profiler.printResults();
    }
    
    void printTestSummary(double totalTestTime) const {
        std::cout << "=== Test Suite Summary ===" << std::endl;
        std::cout << "Total test cases: " << testCases.size() << std::endl;
        std::cout << "Total execution time: " << totalTestTime << "s" << std::endl;
        
        int passedCount = 0;
        int failedCount = 0;
        double totalTestExecutionTime = 0.0;
        
        for (const TestResult& result : results) {
            if (result.passed) {
                passedCount++;
            } else {
                failedCount++;
            }
            totalTestExecutionTime += result.executionTime;
        }
        
        std::cout << "Tests passed: " << passedCount << std::endl;
        std::cout << "Tests failed: " << failedCount << std::endl;
        std::cout << "Average test time: " << (totalTestExecutionTime / testCases.size()) << "s" << std::endl;
        
        if (failedCount > 0) {
            std::cout << std::endl;
            std::cout << "=== Failed Tests ===" << std::endl;
            for (const TestResult& result : results) {
                if (!result.passed) {
                    std::cout << result.name << ": " << result.errorDetails << std::endl;
                }
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== All tests completed ===" << std::endl;
    }
    
    void runBenchmarks() {
        std::cout << "=== NLM Performance Benchmarks ===" << std::endl;
        
        // Example benchmark - would be expanded with actual benchmarks
        // This demonstrates the benchmark framework structure
        
        std::cout << "Running benchmarks..." << std::endl;
        
        // Placeholder for benchmark execution
        // In a real implementation, this would run actual benchmarks
        
        std::cout << "Benchmark framework initialized. Add benchmark implementations here." << std::endl;
    }
    
    void printBenchmarkSummary() const {
        std::cout << "=== Benchmark Summary ===" << std::endl;
        std::cout << "Benchmark results would be displayed here." << std::endl;
    }
};

// Memory usage monitoring utilities
class MemoryMonitor {
public:
    struct MemoryStats {
        size_t totalAllocated;
        size_t peakUsage;
        size_t currentUsage;
        double memoryPerNeuron;
        double memoryPerSynapse;
        
        MemoryStats() : totalAllocated(0), peakUsage(0), currentUsage(0),
                       memoryPerNeuron(0.0), memoryPerSynapse(0.0) {}
    };
    
    static MemoryStats getMemoryStats() {
        MemoryStats stats;
        // Implementation would collect actual memory usage statistics
        // This is a placeholder for the actual memory monitoring implementation
        return stats;
    }
    
    static std::string formatMemorySize(size_t bytes) {
        const char* units[] = {"B", "KB", "MB", "GB"};
        int unitIndex = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024.0 && unitIndex < 3) {
            size /= 1024.0;
            unitIndex++;
        }
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
        return oss.str();
    }
};

} // namespace test
