// Comprehensive Integration Test Framework
// Advanced testing framework for NLM cross-system integration

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include <random>

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "development/DevelopmentSystem.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "performance/PerformanceMonitor.hpp"

namespace test_framework {

/**
 * @brief Test configuration for integration tests
 * 
 * Configuration options for the integration test framework.
 */
struct IntegrationTestConfig {
    std::string name;
    std::string description;
    bool enabled = true;
    int numIterations = 1;
    int maxSteps = 1000;
    bool usePerformanceMonitoring = true;
    bool generateReports = true;
    std::string reportDirectory = "test_reports";
    double successThreshold = 0.8; // Minimum success rate for test suite
    int timeoutSeconds = 300; // Test timeout in seconds
    
    // Brain configuration
    struct BrainConfig {
        int neuronCount = 100;
        int regionCount = 1;
        float connectionProbability = 0.1f;
        bool enableDevelopment = true;
        bool enableMemory = true;
        bool enableNeuromodulation = true;
        bool enablePrediction = true;
        bool enableCognition = true;
    } brainConfig;
    
    // Memory configuration
    struct MemoryConfig {
        int workingMemoryCapacity = 50;
        int episodicMemoryCapacity = 100;
        float memoryDecayRate = 0.01f;
        bool enableReplay = true;
    } memoryConfig;
    
    // Neuromodulation configuration
    struct NeuromodulationConfig {
        float dopamineScale = 1.0f;
        bool enableCuriosity = true;
        bool enableNovelty = true;
        float curiosityThreshold = 0.3f;
    } neuromodulationConfig;
    
    // Environment configuration
    struct EnvironmentConfig {
        int worldWidth = 16;
        int worldHeight = 16;
        int visionWidth = 8;
        int visionHeight = 8;
        bool enableVisualInput = true;
        bool enableTouchInput = true;
        bool enableInternalInput = true;
    } environmentConfig;
};

/**
 * @brief Test result structure
 * 
 * Contains detailed information about test execution results.
 */
struct TestResult {
    std::string testName;
    std::string testCategory;
    bool passed = false;
    double executionTime = 0.0;
    double memoryUsage = 0.0;
    double cpuUsage = 0.0;
    std::string errorMessage;
    std::unordered_map<std::string, double> metrics;
    std::vector<std::string> logs;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    TestResult(const std::string& name, const std::string& category) 
        : testName(name), testCategory(category), startTime(std::chrono::high_resolution_clock::now()) {}
    
    double getDuration() const {
        return std::chrono::duration<double>(endTime - startTime).count();
    }
};

/**
 * @brief Integration test suite
 * 
 * Comprehensive test suite for NLM cross-system integration testing.
 */
class IntegrationTestSuite {
public:
    IntegrationTestSuite(const IntegrationTestConfig& config);
    ~IntegrationTestSuite();
    
    /**
     * @brief Add a test case
     * 
     * @param testName Name of the test
     * @param testFunction Function that executes the test
     * @param category Test category (e.g., "integration", "performance", "stress")
     */
    void addTest(const std::string& testName,
                 std::function<bool()> testFunction,
                 const std::string& category = "general");
    
    /**
     * @brief Execute all tests
     * 
     * @return std::vector<TestResult> Vector of test results
     */
    std::vector<TestResult> executeAllTests();
    
    /**
     * @brief Execute tests with specific configuration
     * 
     * @param testNames Names of tests to execute
     * @return std::vector<TestResult> Vector of test results
     */
    std::vector<TestResult> executeTests(const std::vector<std::string>& testNames);
    
    /**
     * @brief Get test configuration
     * 
     * @return IntegrationTestConfig Test configuration
     */
    IntegrationTestConfig getConfig() const { return config_; }
    
    /**
     * @brief Set test configuration
     * 
     * @param config New test configuration
     */
    void setConfig(const IntegrationTestConfig& config) { config_ = config; }
    
    /**
     * @brief Get test results
     * 
     * @return std::vector<TestResult> Vector of test results
     */
    std::vector<TestResult> getTestResults() const { return testResults_; }
    
    /**
     * @brief Get performance monitor
     * 
     * @return PerformanceMonitor& Performance monitor reference
     */
    PerformanceMonitor& getPerformanceMonitor() { return performanceMonitor_; }
    
    /**
     * @brief Generate test report
     * 
     * @param format Report format (json, csv, html)
     * @return std::string Test report as string
     */
    std::string generateReport(const std::string& format = "json") const;
    
    /**
     * @brief Export test results
     * 
     * @param filename Output file path
     * @param format Export format
     * @return bool true if export was successful
     */
    bool exportResults(const std::string& filename, const std::string& format = "json");
    
    /**
     * @brief Clear test results
     */
    void clearResults() { testResults_.clear(); }
    
    /**
     * @brief Get number of passed tests
     * 
     * @return size_t Number of passed tests
     */
    size_t getPassedTestsCount() const {
        return std::count_if(testResults_.begin(), testResults_.end(),
                           [](const TestResult& result) { return result.passed; });
    }
    
    /**
     * @brief Get total number of tests
     * 
     * @return size_t Total number of tests
     */
    size_t getTotalTestsCount() const {
        return testResults_.size();
    }
    
    /**
     * @brief Get success rate
     * 
     * @return double Success rate (0.0 to 1.0)
     */
    double getSuccessRate() const {
        if (testResults_.empty()) return 0.0;
        return static_cast<double>(getPassedTestsCount()) / testResults_.size();
    }
    
private:
    /**
     * @brief Execute a single test with performance monitoring
     */
    TestResult executeTest(const std::string& testName,
                          std::function<bool()> testFunction,
                          const std::string& category);
    
    /**
     * @brief Generate test suite report
     */
    std::string generateSuiteReport() const;
    
    /**
     * @brief Export test results to JSON
     */
    bool exportResultsJSON(const std::string& filename) const;
    
    /**
     * @brief Export test results to CSV
     */
    bool exportResultsCSV(const std::string& filename) const;
    
    /**
     * @brief Export test results to HTML
     */
    bool exportResultsHTML(const std::string& filename) const;
    
    IntegrationTestConfig config_;
    std::vector<std::pair<std::string, std::function<bool()>>> testCases_;
    std::vector<TestResult> testResults_;
    PerformanceMonitor performanceMonitor_;
    mutable std::mutex mutex_;
};

/**
 * @brief Cross-system integration tests
 * 
 * Comprehensive integration tests for NLM cross-system functionality.
 */
namespace integration_tests {

/**
 * @brief Memory system integration test
 * 
 * Tests integration between memory systems and other neural components.
 */
bool testMemorySystemIntegration(const IntegrationTestConfig& config);

/**
 * @brief Neuromodulation system integration test
 * 
 * Tests integration between neuromodulation systems and other neural components.
 */
bool testNeuromodulationSystemIntegration(const IntegrationTestConfig& config);

/**
 * @brief Development system integration test
 * 
 * Tests integration between development system and other neural components.
 */
bool testDevelopmentSystemIntegration(const IntegrationTestConfig& config);

/**
 * @brief Prediction system integration test
 * 
 * Tests integration between prediction system and other neural components.
 */
bool testPredictionSystemIntegration(const IntegrationTestConfig& config);

/**
 * @brief Cognition system integration test
 * 
 * Tests integration between cognition systems and other neural components.
 */
bool testCognitionSystemIntegration(const IntegrationTestConfig& config);

/**
 * @brief Full system integration test
 * 
 * Tests complete integration of all NLM systems.
 */
bool testFullSystemIntegration(const IntegrationTestConfig& config);

/**
 * @brief Stress integration test
 * 
 * Tests integration under high load and stress conditions.
 */
bool testStressIntegration(const IntegrationTestConfig& config);

/**
 * @brief Performance integration test
 * 
 * Tests integration with performance monitoring.
 */
bool testPerformanceIntegration(const IntegrationTestConfig& config);

/**
 * @brief Recovery integration test
 * 
 * Tests integration recovery after failures.
 */
bool testRecoveryIntegration(const IntegrationTestConfig& config);

/**
 * @brief Scalability integration test
 * 
 * Tests integration with different system sizes.
 */
bool testScalabilityIntegration(const IntegrationTestConfig& config);

/**
 * @brief Consistency integration test
 * 
 * Tests integration consistency across multiple runs.
 */
bool testConsistencyIntegration(const IntegrationTestConfig& config);

/**
 * @brief Reliability integration test
 * 
 * Tests integration reliability under varying conditions.
 */
bool testReliabilityIntegration(const IntegrationTestConfig& config);

/**
 * @brief Execute all cross-system integration tests
 * 
 * @param config Test configuration
 * @return std::vector<TestResult> Vector of test results
 */
std::vector<TestResult> runAllIntegrationTests(const IntegrationTestConfig& config);

/**
 * @brief Generate integration test report
 * 
 * @param results Vector of test results
 * @param format Report format (json, csv, html)
 * @return std::string Integration test report
 */
std::string generateIntegrationTestReport(const std::vector<TestResult>& results,
                                          const std::string& format = "json");

} // namespace integration_tests

/**
 * @brief Performance benchmarking tests
 * 
 * Performance benchmarking for NLM systems.
 */
namespace performance_tests {

/**
 * @brief Memory performance test
 * 
 * Tests memory performance characteristics.
 */
bool testMemoryPerformance(const IntegrationTestConfig& config);

/**
 * @brief CPU performance test
 * 
 * Tests CPU performance characteristics.
 */
bool testCPUPerformance(const IntegrationTestConfig& config);

/**
 * @brief Neural network performance test
 * 
 * Tests neural network performance characteristics.
 */
bool testNeuralNetworkPerformance(const IntegrationTestConfig& config);

/**
 * @brief Integration performance test
 * 
 * Tests integration performance characteristics.
 */
bool testIntegrationPerformance(const IntegrationTestConfig& config);

/**
 * @brief Scalability performance test
 * 
 * Tests performance scalability characteristics.
 */
bool testScalabilityPerformance(const IntegrationTestConfig& config);

/**
 * @brief Stress performance test
 * 
 * Tests performance under stress conditions.
 */
bool testStressPerformance(const IntegrationTestConfig& config);

/**
 * @brief Execute all performance tests
 * 
 * @param config Test configuration
 * @return std::vector<TestResult> Vector of test results
 */
std::vector<TestResult> runAllPerformanceTests(const IntegrationTestConfig& config);

} // namespace performance_tests

/**
 * @brief Stress testing framework
 * 
 * Framework for testing NLM systems under stress conditions.
 */
namespace stress_tests {

/**
 * @brief Memory stress test
 * 
 * Tests system behavior under memory pressure.
 */
bool testMemoryStress(const IntegrationTestConfig& config);

/**
 * @brief CPU stress test
 * 
 * Tests system behavior under CPU pressure.
 */
bool testCPUStress(const IntegrationTestConfig& config);

/**
 * @brief Neural stress test
 * 
 * Tests system behavior under neural load.
 */
bool testNeuralStress(const IntegrationTestConfig& config);

/**
 * @brief Integration stress test
 * 
 * Tests integration under high load conditions.
 */
bool testIntegrationStress(const IntegrationTestConfig& config);

/**
 * @brief Execute all stress tests
 * 
 * @param config Test configuration
 * @return std::vector<TestResult> Vector of test results
 */
std::vector<TestResult> runAllStressTests(const IntegrationTestConfig& config);

} // namespace stress_tests

/**
 * @brief Automated test runner
 * 
 * Automated test runner for NLM systems.
 */
class TestRunner {
public:
    TestRunner();
    ~TestRunner();
    
    /**
     * @brief Run integration test suite
     * 
     * @param testSuite Integration test suite to run
     * @return std::vector<TestResult> Vector of test results
     */
    std::vector<TestResult> runIntegrationTestSuite(IntegrationTestSuite& testSuite);
    
    /**
     * @brief Run performance test suite
     * 
     * @param config Test configuration
     * @return std::vector<TestResult> Vector of test results
     */
    std::vector<TestResult> runPerformanceTestSuite(const IntegrationTestConfig& config);
    
    /**
     * @brief Run stress test suite
     * 
     * @param config Test configuration
     * @return std::vector<TestResult> Vector of test results
     */
    std::vector<TestResult> runStressTestSuite(const IntegrationTestConfig& config);
    
    /**
     * @brief Run full test suite
     * 
     * @param config Test configuration
     * @return std::vector<TestResult> Vector of test results
     */
    std::vector<TestResult> runFullTestSuite(const IntegrationTestConfig& config);
    
    /**
     * @brief Generate comprehensive test report
     * 
     * @param results Vector of all test results
     * @param format Report format
     * @return std::string Comprehensive test report
     */
    std::string generateComprehensiveReport(const std::vector<TestResult>& results,
                                            const std::string& format = "json") const;
    
    /**
     * @brief Save test results to file
     * 
     * @param results Vector of test results
     * @param filename Output file path
     * @param format Export format
     * @return bool true if save was successful
     */
    bool saveTestResults(const std::vector<TestResult>& results,
                         const std::string& filename,
                         const std::string& format = "json");
    
    /**
     * @brief Load test configuration from file
     * 
     * @param filename Configuration file path
     * @return IntegrationTestConfig Test configuration
     */
    IntegrationTestConfig loadTestConfig(const std::string& filename) const;
    
    /**
     * @brief Save test configuration to file
     * 
     * @param config Test configuration
     * @param filename Output file path
     */
    void saveTestConfig(const IntegrationTestConfig& config, const std::string& filename) const;
    
    /**
     * @brief Get default test configuration
     * 
     * @return IntegrationTestConfig Default test configuration
     */
    static IntegrationTestConfig getDefaultConfig() {
        IntegrationTestConfig config;
        config.name = "NLM Integration Test Suite";
        config.description = "Comprehensive integration test suite for NLM systems";
        config.numIterations = 3;
        config.maxSteps = 1000;
        config.usePerformanceMonitoring = true;
        config.generateReports = true;
        config.reportDirectory = "test_reports";
        config.successThreshold = 0.8;
        config.timeoutSeconds = 300;
        
        // Brain configuration
        config.brainConfig.neuronCount = 200;
        config.brainConfig.regionCount = 2;
        config.brainConfig.connectionProbability = 0.15f;
        config.brainConfig.enableDevelopment = true;
        config.brainConfig.enableMemory = true;
        config.brainConfig.enableNeuromodulation = true;
        config.brainConfig.enablePrediction = true;
        config.brainConfig.enableCognition = true;
        
        // Memory configuration
        config.memoryConfig.workingMemoryCapacity = 100;
        config.memoryConfig.episodicMemoryCapacity = 200;
        config.memoryConfig.memoryDecayRate = 0.02f;
        config.memoryConfig.enableReplay = true;
        
        // Neuromodulation configuration
        config.neuromodulationConfig.dopamineScale = 1.0f;
        config.neuromodulationConfig.enableCuriosity = true;
        config.neuromodulationConfig.enableNovelty = true;
        config.neuromodulationConfig.curiosityThreshold = 0.3f;
        
        // Environment configuration
        config.environmentConfig.worldWidth = 20;
        config.environmentConfig.worldHeight = 20;
        config.environmentConfig.visionWidth = 10;
        config.environmentConfig.visionHeight = 10;
        config.environmentConfig.enableVisualInput = true;
        config.environmentConfig.enableTouchInput = true;
        config.environmentConfig.enableInternalInput = true;
        
        return config;
    }
    
private:
    std::vector<TestResult> runTestSuite(std::function<std::vector<TestResult>(const IntegrationTestConfig&)> testRunner,
                                          const IntegrationTestConfig& config);
};

} // namespace test_framework

// Convenience functions
namespace test_framework {

/**
 * @brief Quick integration test run
 * 
 * Quick integration test run with default configuration.
 * 
 * @return std::vector<TestResult> Vector of test results
 */
inline std::vector<TestResult> quickIntegrationTest() {
    IntegrationTestSuite testSuite(test_framework::TestRunner::getDefaultConfig());
    return testSuite.executeAllTests();
}

/**
 * @brief Run integration tests with custom configuration
 * 
 * Run integration tests with custom configuration.
 * 
 * @param config Custom test configuration
 * @return std::vector<TestResult> Vector of test results
 */
inline std::vector<TestResult> runIntegrationTests(const IntegrationTestConfig& config) {
    IntegrationTestSuite testSuite(config);
    return testSuite.executeAllTests();
}

/**
 * @brief Generate test report from results
 * 
 * Generate test report from results.
 * 
 * @param results Vector of test results
 * @param format Report format (json, csv, html)
 * @return std::string Test report
 */
inline std::string generateTestReport(const std::vector<TestResult>& results,
                                       const std::string& format = "json") {
    return test_framework::TestRunner::getDefaultConfig().generateReport(format);
}

} // namespace test_framework

} // namespace nlm