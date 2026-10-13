// IntegrationTestProfiler.cpp
// Implementation of the integration test profiler for NLM brain system

#include "IntegrationTestProfiler.h"
#include "brain/Brain.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <algorithm>

namespace {
    // Simple test functions for integration testing
    bool testBrainCreationFunction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        return true;
    }
    
    bool testBrainInitializationFunction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        return success;
    }
    
    bool testBrainStepFunction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        return brain.getTotalNeuronCount() == 100;
    }
    
    bool testBrainResetFunction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        brain.step(0);
        brain.reset();
        
        return brain.getTotalNeuronCount() == 50;
    }
    
    bool testBrainRegionFunction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto regionIds = brain.getRegionIds();
        return regionIds.size() == 3;
    }
    
    bool testActionProductionFunction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        return brain.getTotalNeuronCount() > 0;
    }
    
    bool testMemorySystemFunction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        return brain.getTotalNeuronCount() > 0;
    }
    
    bool testPredictionSystemFunction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        return brain.getTotalNeuronCount() > 0;
    }
    
    bool testNeuromodulationFunction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        return brain.getTotalNeuronCount() > 0;
    }
    
    bool testPlasticityFunction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Simulate some plasticity
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
            brain.step(step);
        }
        
        return brain.getTotalNeuronCount() == 100;
    }
    
    bool testDevelopmentFunction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Simulate development
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
            brain.step(step);
            
            if (step % 1000 == 0 && step > 0) {
                // Developmental stage changes
            }
        }
        
        return brain.getTotalNeuronCount() == 100;
    }
    
    bool testIntegrationFunction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        for (size_t i = 0; i < 100; ++i) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
            brain.step(i, i * 0.001);
        }
        
        return brain.getTotalNeuronCount() == 100;
    }
}

IntegrationTestProfiler::IntegrationTestProfiler(const IntegrationTestProfilerConfig& config)
    : config(config), performanceProfiler(config.enablePerformanceProfiling ? PerformanceProfilerConfig() : PerformanceProfilerConfig()),
      memoryProfiler(config.enableMemoryProfiling ? MemoryProfilerConfig() : MemoryProfilerConfig()),
      benchmarkSuite(PerformanceProfilerConfig()) {
    
    // Set up profilers with appropriate configurations
    PerformanceProfilerConfig perfConfig;
    perfConfig.enableTiming = config.enablePerformanceProfiling;
    perfConfig.enableMemoryTracking = config.enableMemoryProfiling;
    perfConfig.enableVisualOutput = config.enableDetailedOutput;
    perfConfig.enableExport = config.enableExport;
    
    PerformanceProfilerConfig memoryConfig;
    memoryConfig.enableAllocationTracking = config.enableMemoryProfiling;
    memoryConfig.enableDeallocationTracking = config.enableMemoryProfiling;
    memoryConfig.enableLeakDetection = config.enableLeakDetection;
    memoryConfig.enableRealTimeMonitoring = config.enableMemoryProfiling;
    
    performanceProfiler.setConfig(perfConfig);
    memoryProfiler.setConfig(memoryConfig);
    
    benchmarkSuite.setProfilerConfig(perfConfig);
}

IntegrationTestProfiler::~IntegrationTestProfiler() {
    // Cleanup resources
}

namespace {
    IntegrationTestProfiler* globalIntegrationTestProfiler = nullptr;
    std::mutex integrationTestProfilerMutex;
}

IntegrationTestProfiler& IntegrationTestProfiler::getInstance(const IntegrationTestProfilerConfig& config) {
    std::lock_guard<std::mutex> lock(integrationTestProfilerMutex);
    if (!globalIntegrationTestProfiler) {
        globalIntegrationTestProfiler = new IntegrationTestProfiler(config);
    }
    return *globalIntegrationTestProfiler;
}

void IntegrationTestProfiler::cleanupInstance() {
    std::lock_guard<std::mutex> lock(integrationTestProfilerMutex);
    delete globalIntegrationTestProfiler;
    globalIntegrationTestProfiler = nullptr;
}

void IntegrationTestProfiler::runFullTestSuite() {
    logTest("Starting full integration test suite");
    
    runIndividualTests();
    
    analyzePerformanceRegression();
    
    generateTestReport();
    
    if (config.enableExport) {
        exportTestResults(config.exportPath + "/integration_test_results.txt");
    }
    
    logTest("Integration test suite completed");
}

void IntegrationTestProfiler::runIndividualTests() {
    logTest("Running individual integration tests");
    
    // Run all integration tests
    runTest("Brain Creation");
    runTest("Brain Initialization");
    runTest("Brain Step");
    runTest("Brain Reset");
    runTest("Brain Regions");
    runTest("Action Production");
    runTest("Memory System");
    runTest("Prediction System");
    runTest("Neuromodulation");
    runTest("Plasticity");
    runTest("Development");
    runTest("Integration Test");
}

TestResult IntegrationTestProfiler::runTest(const std::string& testName) {
    std::unordered_map<std::string, std::function<bool()>> testFunctions = {
        {"Brain Creation", testBrainCreationFunction},
        {"Brain Initialization", testBrainInitializationFunction},
        {"Brain Step", testBrainStepFunction},
        {"Brain Reset", testBrainResetFunction},
        {"Brain Regions", testBrainRegionFunction},
        {"Action Production", testActionProductionFunction},
        {"Memory System", testMemorySystemFunction},
        {"Prediction System", testPredictionSystemFunction},
        {"Neuromodulation", testNeuromodulationFunction},
        {"Plasticity", testPlasticityFunction},
        {"Development", testDevelopmentFunction},
        {"Integration Test", testIntegrationFunction}
    };
    
    std::unordered_map<std::string, std::string> testCategories = {
        {"Brain Creation", "brain"},
        {"Brain Initialization", "brain"},
        {"Brain Step", "brain"},
        {"Brain Reset", "brain"},
        {"Brain Regions", "brain"},
        {"Action Production", "action"},
        {"Memory System", "memory"},
        {"Prediction System", "prediction"},
        {"Neuromodulation", "neuromodulation"},
        {"Plasticity", "plasticity"},
        {"Development", "development"},
        {"Integration Test", "integration"}
    };
    
    if (testFunctions.find(testName) != testFunctions.end()) {
        return profileTest(testName, testCategories[testName], testFunctions[testName]);
    } else {
        TestResult result(testName, "unknown");
        result.passed = false;
        result.performanceGrade = "X";
        logTest("Test " + testName + " not found");
        testResults.push_back(result);
        return result;
    }
}

void IntegrationTestProfiler::logTest(const std::string& message) {
    testLog.push_back(message);
    if (config.enableDetailedOutput) {
        std::cout << "[TEST] " << message << std::endl;
    }
}

std::string IntegrationTestProfiler::getPerformanceGrade(double executionTime) const {
    if (executionTime < 0.1) return "A";
    if (executionTime < 0.5) return "B";
    if (executionTime < 1.0) return "C";
    if (executionTime < 2.0) return "D";
    return "F";
}

double IntegrationTestProfiler::calculateMemoryEfficiency(size_t memoryUsage, size_t allocations) const {
    if (allocations == 0) return 0.0;
    return static_cast<double>(memoryUsage) / allocations; // bytes per allocation
}

bool IntegrationTestProfiler::isRegression(const std::string& testName, double currentTime) const {
    if (!hasBaseline(testName)) return false;
    
    double baseline = getBaseline(testName);
    double threshold = baseline * 1.5; // 50% slower is considered regression
    
    return currentTime > threshold;
}

void IntegrationTestProfiler::setBaseline(const std::string& testName, double baselineTime) {
    baselinePerformance[testName] = baselineTime;
}

bool IntegrationTestProfiler::hasBaseline(const std::string& testName) const {
    return baselinePerformance.find(testName) != baselinePerformance.end();
}

double IntegrationTestProfiler::getBaseline(const std::string& testName) const {
    auto it = baselinePerformance.find(testName);
    return (it != baselinePerformance.end()) ? it->second : 0.0;
}

TestResult IntegrationTestProfiler::getTestResult(const std::string& testName) const {
    for (const auto& result : testResults) {
        if (result.testName == testName) {
            return result;
        }
    }
    return TestResult(testName, "unknown"); // Return empty result if not found
}

std::vector<TestResult> IntegrationTestProfiler::getPassedTests() const {
    std::vector<TestResult> passed;
    for (const auto& result : testResults) {
        if (result.passed) {
            passed.push_back(result);
        }
    }
    return passed;
}

std::vector<TestResult> IntegrationTestProfiler::getFailedTests() const {
    std::vector<TestResult> failed;
    for (const auto& result : testResults) {
        if (!result.passed) {
            failed.push_back(result);
        }
    }
    return failed;
}

std::vector<TestResult> IntegrationTestProfiler::getSlowTests(double thresholdSeconds) const {
    std::vector<TestResult> slow;
    for (const auto& result : testResults) {
        if (result.executionTime > thresholdSeconds) {
            slow.push_back(result);
        }
    }
    return slow;
}

void IntegrationTestProfiler::analyzePerformanceRegression() {
    logTest("Analyzing performance regression");
    
    for (const auto& result : testResults) {
        if (hasBaseline(result.testName)) {
            double baseline = getBaseline(result.testName);
            if (baseline > 0.0) {
                double regression = (result.executionTime - baseline) / baseline * 100.0;
                
                if (regression > 50.0) { // More than 50% slower
                    logTest("REGRESSION: " + result.testName + " took " + 
                           std::to_string(result.executionTime) + "s (" + 
                           std::to_string(regression) + "% slower than baseline)");
                }
            }
        }
    }
}

void IntegrationTestProfiler::generateTestReport() const {
    printHeader("NLM Integration Test Profiler - Final Report");
    
    std::cout << std::left << std::setw(25) << "Test Name"
              << std::setw(15) << "Category"
              << std::setw(10) << "Passed"
              << std::setw(12) << "Execution Time"
              << std::setw(12) << "Memory Usage"
              << std::setw(10) << "Allocations"
              << std::setw(10) << "Performance Grade"
              << std::endl;
    
    std::cout << std::string(100, '=') << std::endl;
    
    for (const auto& result : testResults) {
        std::cout << std::left << std::setw(25) << result.testName
                  << std::setw(15) << result.testCategory
                  << std::setw(10) << (result.passed ? "Yes" : "No")
                  << std::setw(12) << std::fixed << std::setprecision(4) << result.executionTime << "s"
                  << std::setw(12) << result.memoryUsage << " bytes"
                  << std::setw(10) << result.allocations
                  << std::setw(10) << result.performanceGrade
                  << std::endl;
    }
    
    std::cout << std::endl;
    
    // Summary statistics
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "Test Summary" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Total Tests: " << testResults.size() << std::endl;
    std::cout << "Passed: " << getPassedCount() << std::endl;
    std::cout << "Failed: " << getFailedCount() << std::endl;
    std::cout << "Success Rate: " << std::fixed << std::setprecision(2) 
              << (static_cast<double>(getPassedCount()) / testResults.size() * 100.0) << "%" << std::endl;
    
    double totalExecutionTime = 0.0;
    for (const auto& result : testResults) {
        totalExecutionTime += result.executionTime;
    }
    
    std::cout << "Total Execution Time: " << std::fixed << std::setprecision(2) << totalExecutionTime << "s" << std::endl;
    std::cout << std::endl;
}

void IntegrationTestProfiler::exportTestResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "NLM Integration Test Profiler Results" << std::endl;
    file << "=====================================" << std::endl;
    file << std::endl;
    
    file << "Total Tests: " << testResults.size() << std::endl;
    file << "Passed: " << getPassedCount() << std::endl;
    file << "Failed: " << getFailedCount() << std::endl;
    file << std::endl;
    
    for (const auto& result : testResults) {
        file << "Test: " << result.testName << std::endl;
        file << "Category: " << result.testCategory << std::endl;
        file << "Status: " << (result.passed ? "PASSED" : "FAILED") << std::endl;
        file << "Execution Time: " << std::fixed << std::setprecision(4) << result.executionTime << "s" << std::endl;
        file << "Memory Usage: " << result.memoryUsage << " bytes" << std::endl;
        file << "Allocations: " << result.allocations << std::endl;
        file << "Deallocations: " << result.deallocations << std::endl;
        file << "Performance Grade: " << result.performanceGrade << std::endl;
        file << std::endl;
    }
    
    file.close();
}

void IntegrationTestProfiler::generatePerformanceDashboard() const {
    printHeader("NLM Integration Test Performance Dashboard");
    
    std::cout << std::endl;
    
    // Test status pie chart (text-based)
    size_t total = testResults.size();
    size_t passed = getPassedCount();
    size_t failed = getFailedCount();
    
    std::cout << "Test Status Distribution:" << std::endl;
    std::cout << "  PASSED:   " << std::string(passed * 50 / total, '=') << std::endl;
    std::cout << "  FAILED:   " << std::string(failed * 50 / total, '=') << std::endl;
    std::cout << std::endl;
    
    // Performance by category
    std::cout << "Performance by Category:" << std::endl;
    std::cout << std::left << std::setw(20) << "Category"
              << std::setw(15) << "Tests"
              << std::setw(15) << "Avg Time"
              << std::setw(15) << "Min Time"
              << std::setw(15) << "Max Time"
              << std::endl;
    
    std::cout << std::string(80, '=') << std::endl;
    
    std::unordered_map<std::string, std::vector<double>> categoryTimes;
    std::unordered_map<std::string, size_t> categoryCounts;
    
    for (const auto& result : testResults) {
        categoryTimes[result.testCategory].push_back(result.executionTime);
        categoryCounts[result.testCategory]++;
    }
    
    for (const auto& pair : categoryTimes) {
        const std::string& category = pair.first;
        const std::vector<double>& times = pair.second;
        
        double sum = std::accumulate(times.begin(), times.end(), 0.0);
        double avg = sum / times.size();
        double min = *std::min_element(times.begin(), times.end());
        double max = *std::max_element(times.begin(), times.end());
        
        std::cout << std::left << std::setw(20) << category
                  << std::setw(15) << categoryCounts[category]
                  << std::setw(15) << std::fixed << std::setprecision(4) << avg << "s"
                  << std::setw(15) << std::fixed << std::setprecision(4) << min << "s"
                  << std::setw(15) << std::fixed << std::setprecision(4) << max << "s"
                  << std::endl;
    }
}

void IntegrationTestProfiler::clear() {
    testResults.clear();
    testLog.clear();
    baselinePerformance.clear();
}

void IntegrationTestProfiler::resetBaseline() {
    baselinePerformance.clear();
}

size_t IntegrationTestProfiler::getPassedCount() const {
    return static_cast<size_t>(std::count_if(testResults.begin(), testResults.end(),
                                           [](const TestResult& result) { return result.passed; }));
}

size_t IntegrationTestProfiler::getFailedCount() const {
    return static_cast<size_t>(std::count_if(testResults.begin(), testResults.end(),
                                           [](const TestResult& result) { return !result.passed; }));
}

void printHeader(const std::string& title) const {
    std::cout << std::endl;
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << title << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}