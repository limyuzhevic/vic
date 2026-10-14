#pragma once

/**
 * Comprehensive test suite for NLM brain system
 * 
 * This test suite provides:
 * - Unit tests for individual components
 * - Integration tests for system connectivity
 * - Performance benchmarks
 * - Stress testing for large-scale simulations
 * - Property-based testing for edge cases
 * - Memory leak detection
 * - Configuration validation
 * - Cross-platform compatibility testing
 */

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "integration/NLMFacade.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <fstream>
#include <cassert>
#include <iostream>

namespace nlm {

/**
 * Test result structure
 */
struct TestResult {
    std::string testName;
    bool passed;
    double executionTime;  // in seconds
    std::string errorMessage;
    size_t neuronsUsed;
    size_t synapsesUsed;
    
    TestResult(const std::string& name, bool pass, double time = 0.0)
        : testName(name), passed(pass), executionTime(time), 
          errorMessage(""), neuronsUsed(0), synapsesUsed(0) {}
};

/**
 * Performance benchmark structure
 */
struct BenchmarkResult {
    std::string benchmarkName;
    double executionTime;  // seconds
    size_t neuronsProcessed;
    size_t synapsesProcessed;
    double throughput;  // operations per second
    
    BenchmarkResult(const std::string& name, double time, size_t neurons, size_t synapses)
        : benchmarkName(name), executionTime(time), neuronsProcessed(neurons),
          synapsesProcessed(synapses), throughput(0.0) {
        double totalOps = neurons + synapses;
        throughput = totalOps / time;
    }
};

/**
 * Memory usage tracking
 */
struct MemoryStats {
    size_t allocatedBytes;
    size_t peakAllocatedBytes;
    size_t deallocatedBytes;
    size_t currentAllocatedBytes;
    
    MemoryStats() : allocatedBytes(0), peakAllocatedBytes(0), 
                   deallocatedBytes(0), currentAllocatedBytes(0) {}
};

/**
 * Comprehensive test suite for NLM
 */
class NLMTestSuite {
public:
    NLMTestSuite() : randomEngine(std::chrono::system_clock::now().time_since_epoch().count()) {}
    ~NLMTestSuite() = default;
    
    // Test suite execution
    std::vector<TestResult> runAllTests();
    std::vector<TestResult> runComponentTests();
    std::vector<TestResult> runIntegrationTests();
    std::vector<TestResult> runPerformanceTests();
    std::vector<TestResult> runStressTests();
    std::vector<TestResult> runPropertyTests();
    
    // Individual test categories
    std::vector<TestResult> testConfiguration();
    std::vector<TestResult> testBrainInitialization();
    std::vector<TestResult> testNeuronDynamics();
    std::vector<TestResult> testSynapticPlasticity();
    std::vector<TestResult> testMemorySystems();
    std::vector<TestResult> testNeuromodulation();
    std::vector<TestResult> testPrediction();
    std::vector<TestResult> testCognition();
    std::vector<TestResult> testDevelopment();
    std::vector<TestResult> testCheckpointing();
    std::vector<TestResult> testReplay();
    
    // Performance benchmarks
    std::vector<BenchmarkResult> runPerformanceBenchmarks();
    BenchmarkResult benchmarkSmallScale(size_t neuronCount);
    BenchmarkResult benchmarkMediumScale(size_t neuronCount);
    BenchmarkResult benchmarkLargeScale(size_t neuronCount);
    
    // Stress testing
    std::vector<TestResult> runStressTests();
    TestResult stressTestHighNeuronCount(size_t neuronCount);
    TestResult stressTestHighSynapseCount(size_t neuronCount);
    TestResult stressTestLongRunning(size_t steps);
    
    // Memory leak detection
    std::vector<TestResult> runMemoryLeakTests();
    TestResult detectMemoryLeaks();
    
    // Property-based testing
    std::vector<TestResult> runPropertyTests();
    TestResult testNeuronInvariants();
    TestResult testSynapseInvariants();
    TestResult testBrainStateInvariants();
    
    // Cross-validation
    std::vector<TestResult> runCrossValidationTests();
    TestResult testConfigurationConsistency();
    TestResult testSimulationReproducibility();
    
    // Integration testing
    std::vector<TestResult> runIntegrationTests();
    TestResult testSystemIntegration();
    TestResult testMemoryIntegration();
    TestResult testNeuromodulationIntegration();
    TestResult testPredictionIntegration();
    TestResult testCognitionIntegration();
    TestResult testDevelopmentIntegration();
    
    // Advanced tests
    std::vector<TestResult> runAdvancedTests();
    TestResult testEligibilityTracePlasticity();
    TestResult testAdvancedLearning();
    TestResult testRealTimeMode();
    TestResult testBatchProcessing();
    
    // Configuration validation
    std::vector<TestResult> testConfigurationValidation();
    TestResult validateConfiguration(const std::string& configFile = "");
    
    // Results and reporting
    void saveTestResults(const std::string& filename) const;
    void loadTestResults(const std::string& filename);
    void printTestResults() const;
    void printBenchmarkResults() const;
    
    // Progress tracking
    double getTestProgress() const;
    std::string getCurrentTestName() const;
    
    // Configuration for tests
    void setTestConfig(const std::string& key, const std::string& value);
    void setTestNeuronCount(size_t count);
    void setTestSynapseCount(size_t count);
    void setTestStepCount(size_t count);
    void setTestDuration(double duration);
    
    // Random number generation
    float randomFloat(float min = 0.0f, float max = 1.0f);
    int randomInt(int min = 0, int max = 100);
    
private:
    // Test helper functions
    bool testBrainStability(std::shared_ptr<Brain> brain, size_t steps);
    bool testPlasticityLearning(std::shared_ptr<Brain> brain, size_t trials);
    bool testMemoryCapacity(std::shared_ptr<Brain> brain, size_t capacity);
    bool testNeuromodulationEffect(std::shared_ptr<Brain> brain, float reward);
    
    // Performance measurement
    double measureExecutionTime(std::function<void()> testFunc);
    MemoryStats measureMemoryUsage(std::function<void()> testFunc);
    
    // Environment
    std::mt19937 randomEngine;
    
    // Test configuration
    struct {
        size_t neuronCount;
        size_t synapseCount;
        size_t stepCount;
        double duration;
        std::map<std::string, std::string> configOverrides;
    } testConfig;
    
    // Results
    std::vector<TestResult> testResults;
    std::vector<BenchmarkResult> benchmarkResults;
    MemoryStats currentMemoryStats;
    
    // Progress tracking
    size_t currentTestIndex;
    std::string currentTestName;
};