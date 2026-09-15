#pragma once

#include <string>
#include <memory>
#include <vector>

namespace nlm {

/**
 * @struct IntegrationTestResult
 * @brief Result of an integration test
 * 
 * Contains the outcome of integration tests, including test name, pass/fail status,
 * and detailed error messages if the test failed.
 */
struct IntegrationTestResult {
    std::string testName;
    bool passed;
    std::string message;
};

/**
 * @class IntegrationTestRunner
 * @brief Runs comprehensive integration tests for NLM Phase 6
 * 
 * This class executes a series of integration tests to verify that all major
 * improvements and fixes are working correctly together. The tests cover:
 * - Configuration file loading and validation
 * - BrainInitializer functionality
 * - Neuromodulator system integration
 * - SpikeSystem performance optimizations
 * - STDP plasticity implementation
 * 
 * These tests ensure that the integrated artificial brain system is functioning
 * correctly across all components and subsystems.
 */
class IntegrationTestRunner {
public:
    IntegrationTestRunner();
    ~IntegrationTestRunner();
    
    // Disable copying and moving
    IntegrationTestRunner(const IntegrationTestRunner&) = delete;
    IntegrationTestRunner& operator=(const IntegrationTestRunner&) = delete;
    IntegrationTestRunner(IntegrationTestRunner&&) noexcept = default;
    IntegrationTestRunner& operator=(IntegrationTestRunner&&) noexcept = default;
    
    /**
     * @brief Run all integration tests
     * 
     * Executes the complete test suite and reports results. Each test is designed
     * to verify that specific improvements and fixes are working correctly.
     */
    void runAllTests();
    
    /**
     * @brief Run a complete integration test
     * 
     * @return Integration test result with overall status
     */
    IntegrationTestResult runCompleteIntegrationTest();
    
    /**
     * @brief Test configuration file loading
     * 
     * @return Integration test result for config file loading
     */
    IntegrationTestResult testConfigFileLoading();
    
    /**
     * @brief Test BrainInitializer functionality
     * 
     * @return Integration test result for BrainInitializer
     */
    IntegrationTestResult testBrainInitializer();
    
    /**
     * @brief Test neuromodulator system integration
     * 
     * @return Integration test result for neuromodulator system
     */
    IntegrationTestResult testNeuromodulatorSystem();
    
    /**
     * @brief Test SpikeSystem performance optimizations
     * 
     * @return Integration test result for SpikeSystem
     */
    IntegrationTestResult testSpikeSystemPerformance();
    
    /**
     * @brief Test STDP plasticity implementation
     * 
     * @return Integration test result for STDP
     */
    IntegrationTestResult testSTDPPlasticity();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm