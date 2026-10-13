// IntegrationTestProfiler.h
// Integration test profiler for NLM brain system

#ifndef NLM_INTEGRATION_TEST_PROFILER_H
#define NLM_INTEGRATION_TEST_PROFILER_H

#include "performance/PerformanceProfiler.h"
#include "performance/PerformanceBenchmarkSuite.h"
#include "performance/MemoryProfiler.h"
#include "brain/Brain.hpp"
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <unordered_map>

// Test result structure
struct TestResult {
    std::string testName;
    std::string testCategory;
    bool passed;
    double executionTime;       // seconds
    size_t memoryUsage;          // bytes
    size_t allocations;          // number of allocations
    size_t deallocations;        // number of deallocations
    std::string performanceGrade;
    
    TestResult(const std::string& name, const std::string& category)
        : testName(name), testCategory(category), passed(false), 
          executionTime(0.0), memoryUsage(0), allocations(0), 
          deallocations(0), performanceGrade("C") {}
};

// Integration test profiler configuration
struct IntegrationTestProfilerConfig {
    bool enablePerformanceProfiling = true;      // Enable performance profiling
    bool enableMemoryProfiling = true;           // Enable memory profiling
    bool enableCPUMonitoring = true;             // Enable CPU monitoring
    bool enableLeakDetection = true;             // Enable memory leak detection
    bool enableRegressionDetection = true;       // Detect performance regressions
    bool enableDetailedOutput = true;            // Enable detailed output
    bool enableExport = true;                    // Enable export of results
    double performanceThreshold = 1.0;           // Performance threshold in seconds
    double memoryThreshold = 100 * 1024 * 1024;  // Memory threshold in bytes (100MB)
    std::string exportPath = "./test_profiles"; // Directory for test exports
    
    IntegrationTestProfilerConfig() {}
};

// Integration test profiler
class IntegrationTestProfiler {
private:
    IntegrationTestProfilerConfig config;
    PerformanceProfiler performanceProfiler;
    MemoryProfiler memoryProfiler;
    PerformanceBenchmarkSuite benchmarkSuite;
    
    // Test tracking
    std::vector<TestResult> testResults;
    std::vector<std::string> testLog;
    std::unordered_map<std::string, double> baselinePerformance;
    
    // Helper methods
    void logTest(const std::string& message);
    std::string getPerformanceGrade(double executionTime) const;
    double calculateMemoryEfficiency(size_t memoryUsage, size_t allocations) const;
    bool isRegression(const std::string& testName, double currentTime) const;
    
    // Integration test implementations
    TestResult runBrainCreationTest();
    TestResult runBrainInitializationTest();
    TestResult runBrainStepTest();
    TestResult runBrainResetTest();
    TestResult runBrainRegionTest();
    TestResult runActionProductionTest();
    TestResult runMemorySystemTest();
    TestResult runPredictionSystemTest();
    TestResult runNeuromodulationTest();
    TestResult runPlasticityTest();
    TestResult runDevelopmentTest();
    TestResult runIntegrationTest();
    
public:
    IntegrationTestProfiler(const IntegrationTestProfilerConfig& config = IntegrationTestProfilerConfig());
    ~IntegrationTestProfiler();
    
    // Core test operations
    void runFullTestSuite();
    void runIndividualTests();
    TestResult runTest(const std::string& testName);
    
    // Configuration
    void setConfig(const IntegrationTestProfilerConfig& newConfig);
    const IntegrationTestProfilerConfig& getConfig() const { return config; }
    
    // Test management
    void setBaseline(const std::string& testName, double baselineTime);
    bool hasBaseline(const std::string& testName) const;
    double getBaseline(const std::string& testName) const;
    
    // Results access
    const std::vector<TestResult>& getTestResults() const { return testResults; }
    TestResult getTestResult(const std::string& testName) const;
    std::vector<TestResult> getPassedTests() const;
    std::vector<TestResult> getFailedTests() const;
    std::vector<TestResult> getSlowTests(double thresholdSeconds) const;
    
    // Analysis and reporting
    void analyzePerformanceRegression();
    void generateTestReport() const;
    void exportTestResults(const std::string& filename) const;
    void generatePerformanceDashboard() const;
    
    // Utility methods
    void clear();
    void resetBaseline();
    size_t getTestCount() const { return testResults.size(); }
    size_t getPassedCount() const;
    size_t getFailedCount() const;
    
    // Integration with existing test framework
    template<typename TestFunc>
    TestResult profileTest(const std::string& testName, const std::string& category, TestFunc&& testFunc) {
        TestResult result(testName, category);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto memoryStart = memoryProfiler.getCurrentMemoryUsage();
        
        try {
            // Run the test with profiling
            if (config.enablePerformanceProfiling) {
                auto profileStart = std::chrono::high_resolution_clock::now();
                result.passed = testFunc();
                auto profileEnd = std::chrono::high_resolution_clock::now();
                result.executionTime = std::chrono::duration<double>(profileEnd - profileStart).count();
            } else {
                result.passed = testFunc();
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
            
            if (config.enableMemoryProfiling) {
                result.memoryUsage = memoryProfiler.getCurrentMemoryUsage() - memoryStart;
            }
            
            // Count allocations/deallocations
            result.allocations = memoryProfiler.getAllocationCount();
            result.deallocations = memoryProfiler.getDeallocationCount();
            
            // Set performance grade
            result.performanceGrade = getPerformanceGrade(result.executionTime);
            
            // Check for regression
            if (config.enableRegressionDetection && hasBaseline(testName)) {
                if (isRegression(testName, result.executionTime)) {
                    logTest("REGRESSION DETECTED: " + testName + " took " + 
                           std::to_string(result.executionTime) + "s (baseline: " + 
                           std::to_string(getBaseline(testName)) + "s)");
                }
            }
            
        } catch (const std::exception& e) {
            logTest("Test " + testName + " failed with exception: " + e.what());
            result.passed = false;
            result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
            result.performanceGrade = "F";
        }
        
        testResults.push_back(result);
        return result;
    }
    
    // Singleton pattern for global access
    static IntegrationTestProfiler& getInstance(const IntegrationTestProfilerConfig& config = IntegrationTestProfilerConfig());
    static void cleanupInstance();
};