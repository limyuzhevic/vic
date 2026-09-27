// Phase 6 Integration Test Suite
// Tests complete brain loop integration and validates all systems work together

#pragma once

#include "Phase6IntegratedExperiment.hpp"
#include "AblationSystem.hpp"
#include "ScalingBenchmark.hpp"
#include "Experiment.hpp"
#include "Metrics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <random>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

namespace nlm {

/**
 * Comprehensive Phase 6 integration test suite
 * Tests all integrated systems working together with quantitative validation
 */
class Phase6IntegrationTestSuite {
public:
    Phase6IntegrationTestSuite();
    ~Phase6IntegrationTestSuite();
    
    /**
     * Run all integration tests with comprehensive reporting
     */
    bool runAllTests(const std::string& outputDir = "./integration_test_results");
    
    /**
     * Test complete brain loop integration
     */
    bool testBrainLoopIntegration();
    
    /**
     * Test neural computation (LIF neurons)
     */
    bool testNeuralComputation();
    
    /**
     * Test plasticity (STDP, Hebbian, structural)
     */
    bool testPlasticity();
    
    /**
     * Test neuromodulation (Dopamine, Curiosity, Novelty, PredictionError)
     */
    bool testNeuromodulation();
    
    /**
     * Test memory systems (Working, Episodic, Associative)
     */
    bool testMemorySystems();
    
    /**
     * Test cognitive systems (NeuralPlanner, ConceptFormation, Attention)
     */
    bool testCognitiveSystems();
    
    /**
     * Test prediction system
     */
    bool testPredictionSystem();
    
    /**
     * Test development system
     */
    bool testDevelopmentSystem();
    
    /**
     * Test checkpoint system
     */
    bool testCheckpointSystem();
    
    /**
     * Test stress conditions
     */
    bool testStressConditions();
    
    /**
     * Test large-scale brain simulations (1000+ neurons)
     */
    bool testLargeScaleSimulations();
    
    /**
     * Test high firing rate scenarios
     */
    bool testHighFiringRateScenarios();
    
    /**
     * Test memory system saturation
     */
    bool testMemorySystemSaturation();
    
    /**
     * Test plasticity rule conflicts
     */
    bool testPlasticityRuleConflicts();
    
    /**
     * Test neuromodulator interaction overload
     */
    bool testNeuromodulatorInteractionOverload();
    
    /**
     * Test integration scenarios
     */
    bool testIntegrationScenarios();
    
    /**
     * Test sensory input → Neural processing → Memory formation
     */
    bool testSensoryToMemoryIntegration();
    
    /**
     * Test Working memory → Attention → Concept formation
     */
    bool testWorkingMemoryToConceptIntegration();
    
    /**
     * Test Memory replay and consolidation
     */
    bool testMemoryReplayAndConsolidation();
    
    /**
     * Test Development stage transitions
     */
    bool testDevelopmentStageTransitions();
    
    /**
     * Test Checkpoint save/load functionality
     */
    bool testCheckpointFunctionality();
    
    /**
     * Test quantitative metrics
     */
    bool testQuantitativeMetrics();
    
    /**
     * Test neural activity patterns
     */
    bool testNeuralActivityPatterns();
    
    /**
     * Test learning progress measurements
     */
    bool testLearningProgressMeasurements();
    
    /**
     * Test memory retention scores
     */
    bool testMemoryRetentionScores();
    
    /**
     * Test system stability under stress
     */
    bool testSystemStabilityUnderStress();
    
    /**
     * Test integration effectiveness scores
     */
    bool testIntegrationEffectivenessScores();
    
    /**
     * Test performance benchmarks
     */
    bool testPerformanceBenchmarks();
    
    /**
     * Test execution time per simulation step
     */
    bool testExecutionTimePerStep();
    
    /**
     * Test memory usage patterns
     */
    bool testMemoryUsagePatterns();
    
    /**
     * Test scalability characteristics
     */
    bool testScalabilityCharacteristics();
    
    /**
     * Test resource utilization metrics
     */
    bool testResourceUtilizationMetrics();
    
    /**
     * Generate comprehensive test report
     */
    std::string generateTestReport() const;
    
    /**
     * Save test results to files
     */
    bool saveResults(const std::string& outputDir);
    
    /**
     * Run statistical validation on test results
     */
    bool runStatisticalValidation();
    
    /**
     * Verify test repeatability and determinism
     */
    bool verifyDeterminism();
    
    /**
     * Validate for CI/CD integration
     */
    bool validateForCI();
    
    /**
     * Get test suite statistics
     */
    struct TestStatistics {
        size_t totalTests;
        size_t passedTests;
        size_t failedTests;
        double totalTimeSeconds;
        double successRate;
        
        TestStatistics() : totalTests(0), passedTests(0), failedTests(0),
                          totalTimeSeconds(0.0), successRate(0.0) {}
    };
    
    TestStatistics getStatistics() const { return stats_; }
    
    /**
     * Get detailed test results
     */
    const std::vector<std::pair<std::string, bool>>& getTestResults() const { return testResults_; }
    
protected:
    /**
     * Initialize test environment
     */
    bool initializeTestEnvironment();
    
    /**
     * Cleanup test environment
     */
    bool cleanupTestEnvironment();
    
    /**
     * Run a single integration test with timing and error handling
     */
    bool runTest(const std::string& testName, std::function<bool()> testFunction);
    
    /**
     * Setup configuration for a test
     */
    Phase6Config setupTestConfig(const std::string& testName, uint64_t steps = 1000) const;
    
    /**
     * Create brain for testing
     */
    std::shared_ptr<Brain> createTestBrain(const Phase6Config& config);
    
    /**
     * Run comprehensive experiment
     */
    Phase6IntegrationResult runIntegrationExperiment(const Phase6Config& config);
    
    /**
     * Collect metrics from experiment
     */
    Metrics collectExperimentMetrics(const Phase6IntegrationResult& result);
    
    /**
     * Compare metrics against thresholds
     */
    bool compareMetrics(const Metrics& metrics, const std::string& testName) const;
    
    /**
     * Generate CSV report
     */
    std::string generateCSVReport() const;
    
    /**
     * Generate JSON report
     */
    std::string generateJSONReport() const;
    
    /**
     * Generate HTML report
     */
    std::string generateHTMLReport() const;
    
    /**
     * Create output directory if it doesn't exist
     */
    bool createOutputDirectory(const std::string& path);
    
    /**
     * Check if directory exists
     */
    bool directoryExists(const std::string& path) const;
    
    /**
     * Get current timestamp as string
     */
    std::string getTimestamp() const;
    
    /**
     * Get system information
     */
    std::string getSystemInfo() const;
    
    /**
     * Validate integration with existing experiment framework
     */
    bool validateWithExistingFramework() const;
    
    /**
     * Run existing Phase6 demo for comparison
     */
    bool runExistingPhase6Demo() const;
    
    /**
     * Run existing Phase5 experiment for comparison
     */
    bool runExistingPhase5Experiment() const;
    
    /**
     * Check if tests can be run (dependencies met)
     */
    bool checkDependencies() const;
    
    /**
     * Measure execution time of test
     */
    template<typename Func>
    double measureTime(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
    
    /**
     * Generate random seed for deterministic tests
     */
    uint64_t generateRandomSeed(const std::string& testName) const;
    
    /**
     * Create unique test identifier
     */
    std::string createTestId(const std::string& testName, uint64_t seed = 0) const;
    
    /**
     * Update statistics
     */
    void updateStatistics(const std::string& testName, bool passed);
    
    /**
     * Log test progress
     */
    void logTestProgress(const std::string& testName, bool passed, double timeSeconds);
    
    /**
     * Validate metric thresholds
     */
    bool validateMetricThresholds(const Metrics& metrics, const std::string& testName) const;
    
    /**
     * Calculate statistical significance
     */
    double calculateStatisticalSignificance(const std::vector<double>& values) const;
    
    /**
     * Generate test name with prefix
     */
    static std::string getPrefixedTestName(const std::string& testName);
    
    /**
     * Get current working directory
     */
    static std::string getCurrentWorkingDirectory();
    
private:
    TestStatistics stats_;
    std::vector<std::pair<std::string, bool>> testResults_;
    std::vector<std::string> testOutput_;
    mutable std::mt19937_64 rng_;
    std::string outputDir_;
    std::string systemInfo_;
    double cpuUsage_;
    double memoryUsageMB_;
    
    // Configuration for different test scenarios
    struct TestConfig {
        uint64_t smallScaleSteps = 1000;
        uint64_t mediumScaleSteps = 10000;
        uint64_t largeScaleSteps = 50000;
        size_t smallNeuronCount = 100;
        size_t mediumNeuronCount = 1000;
        size_t largeNeuronCount = 10000;
        float stressConnectionProbability = 0.05f;
        float saturationMemoryLoad = 0.8f;
        double stabilityThreshold = 0.05;
        size_t stabilityWindow = 100;
        bool enableCI = false;
        bool generatePlots = true;
        bool verboseOutput = true;
        
        TestConfig() : rng_(std::random_device{}()) {}
    };
    
    TestConfig testConfig_;
};

} // namespace nlm