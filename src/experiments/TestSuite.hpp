// NLM (熙然) - Neural Learning Machine
// Test Suite
// Organizes all neural computation tests with logging, reporting, and configuration

#pragma once

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../experiments/LearningExperiment.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

/**
 * @brief TestSuite class for organizing and running neural computation tests
 * 
 * This class encapsulates all test functions from main.cpp:
 * - Basic Neural Connectivity Test
 * - Plasticity Learning Experiment
 * - STDP Verification Test
 * 
 * Provides configuration management, logging, results reporting, and brain lifecycle management.
 * Each test is modular and independently runnable.
 */
class TestSuite {
public:
    /**
     * @brief Configuration options for test suite parameters
     */
    struct TestConfig {
        // Basic test parameters
        bool enableLogging = true;
        bool resetBetweenTests = true;
        SimulationStep connectivityTestSteps = 50;
        SimulationStep plasticityTestSteps = 1000;
        SimulationStep stdpTestTrials = 50;
        
        // Connectivity test parameters
        size_t connectivityTestNeurons = 10;
        float connectivityTestCurrent = 50.0f;
        
        // Plasticity test parameters
        bool enableHebbianPlasticity = true;
        bool enableStdpPlasticity = true;
        bool enableStructuralPlasticity = false;
        size_t plasticityInputNeurons = 20;
        float plasticityInputCurrent = 30.0f;
        
        // STDP test parameters
        float stdpCorrelationCurrent = 60.0f;
        float stdpWeightThreshold = 0.001f;
        
        // Random seed for reproducibility
        uint64_t seed = 42;
        
        // Report format
        bool enableHumanReadableReport = true;
        std::string reportFilePath = "test_results.txt";
    };

    /**
     * @brief Test results structure
     */
    struct TestResult {
        std::string testName;
        bool passed = false;
        float durationSeconds = 0.0f;
        std::string message;
        std::map<std::string, double> metrics;
        std::string details;
    };

    /**
     * @brief Test suite statistics
     */
    struct SuiteStats {
        int totalTests = 0;
        int passedTests = 0;
        int failedTests = 0;
        float totalDurationSeconds = 0.0f;
        std::map<std::string, TestResult> results;
    };

public:
    /**
     * @brief Constructor with configuration
     * 
     * @param config Test suite configuration
     */
    explicit TestSuite(const TestConfig& config = TestConfig());
    
    /**
     * @brief Default destructor
     */
    ~TestSuite() = default;
    
    // Disable copying
    TestSuite(const TestSuite&) = delete;
    TestSuite& operator=(const TestSuite&) = delete;
    
    // Enable moving
    TestSuite(TestSuite&&) noexcept = default;
    TestSuite& operator=(TestSuite&&) noexcept = default;

    /**
     * @brief Get current configuration
     * @return Current test configuration
     */
    const TestConfig& getConfig() const { return config_; }
    
    /**
     * @brief Set test configuration
     * @param config New configuration
     */
    void setConfig(const TestConfig& config) { config_ = config; }

    /**
     * @brief Run all tests in sequence
     * @return Test suite statistics
     */
    SuiteStats runAllTests(std::shared_ptr<Brain> brain);

    /**
     * @brief Run basic connectivity test
     * @param brain Brain instance to test
     * @return Test result
     */
    TestResult runBasicConnectivityTest(std::shared_ptr<Brain> brain);

    /**
     * @brief Run plasticity learning experiment
     * @param brain Brain instance to test
     * @return Test result
     */
    TestResult runPlasticityExperiment(std::shared_ptr<Brain> brain);

    /**
     * @brief Run STDP verification test
     * @param brain Brain instance to test
     * @return Test result
     */
    TestResult runStdpVerification(std::shared_ptr<Brain> brain);

    /**
     * @brief Reset brain state (if configured)
     * @param brain Brain instance to reset
     */
    void resetBrain(std::shared_ptr<Brain> brain);

    /**
     * @brief Generate human-readable test report
     * @param stats Test suite statistics
     * @return Formatted report string
     */
    std::string generateHumanReadableReport(const SuiteStats& stats) const;

    /**
     * @brief Save test results to file
     * @param stats Test suite statistics
     * @param filepath Output file path
     * @return Success status
     */
    bool saveResults(const SuiteStats& stats, const std::string& filepath) const;

    /**
     * @brief Print test summary to console
     * @param stats Test suite statistics
     */
    void printSummary(const SuiteStats& stats) const;

    /**
     * @brief Enable/disable logging
     * @param enable Whether logging is enabled
     */
    void setLoggingEnabled(bool enable) { config_.enableLogging = enable; }

    /**
     * @brief Set random seed for reproducible tests
     * @param seed Random seed
     */
    void setSeed(uint64_t seed) { config_.seed = seed; }

private:
    // Test configuration
    TestConfig config_;
    
    // Internal helper methods
    std::string getCurrentTimestamp() const;
    float getElapsedSeconds(std::chrono::high_resolution_clock::time_point start) const;
    bool checkCondition(float value, float expected, float tolerance) const;
    
    // Test implementations
    TestResult runBasicConnectivityTestImpl(std::shared_ptr<Brain> brain);
    TestResult runPlasticityExperimentImpl(std::shared_ptr<Brain> brain);
    TestResult runStdpVerificationImpl(std::shared_ptr<Brain> brain);
    
    // Logging helpers
    void logTestStart(const std::string& testName) const;
    void logTestEnd(const TestResult& result) const;
    void logTestProgress(const std::string& testName, const std::string& message) const;
}; // class TestSuite

} // namespace nlm
