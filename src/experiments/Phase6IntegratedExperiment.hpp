#pragma once

/**
 * Phase6IntegratedExperiment - Phase 6 final integration test
 * 
 * This experiment demonstrates that the integrated brain systems work together:
 * - Memory systems are connected to neural processing
 * - Neuromodulation affects plasticity and neural dynamics
 * - Development affects plasticity rates
 * - Prediction system is integrated
 * - Replay and consolidation are functional
 * - Checkpoint save/load works
 * 
 * The experiment runs a complete lifetime simulation and verifies
 * that all systems interact properly.
 */

#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <sstream>
#include <iomanip>

namespace nlm {

/**
 * Phase 6 integration test result with enhanced validation
 */
struct Phase6IntegrationResult {
    // System integration status
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    bool replayWorks;
    
    // Integration test results
    bool integrationVerificationPassed;
    bool memoryTestPassed;
    bool neuromodulationTestPassed;
    bool checkpointTestPassed;
    bool replayTestPassed;
    
    // Metrics
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    
    // Timestamps
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    
    // Test counts
    size_t totalTestsRun;
    size_t totalTestsPassed;
    
    Phase6IntegrationResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        , integrationVerificationPassed(false)
        , memoryTestPassed(false)
        , neuromodulationTestPassed(false)
        , checkpointTestPassed(false)
        , replayTestPassed(false)
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , avgSynapticWeight(0.0f)
        , memoryEpisodesStored(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0)
        , totalTestsRun(0)
        , totalTestsPassed(0) {}
};

/**
 * Configuration for Phase 6 integration experiment with validation
 */
struct Phase6Config {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin") {}
    
    /**
     * Validate configuration values
     */
    bool validate() const {
        bool isValid = true;
        
        if (maxSteps == 0) {
            return false;
        }
        
        if (neuronCount == 0) {
            return false;
        }
        
        if (regionCount == 0) {
            return false;
        }
        
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            return false;
        }
        
        return isValid;
    }
    
    /**
     * Get configuration as string for logging
     */
    std::string toString() const {
        std::stringstream ss;
        ss << "Phase6Config: maxSteps=" << maxSteps 
           << ", neuronCount=" << neuronCount
           << ", regionCount=" << regionCount
           << ", connectionProbability=" << connectionProbability
           << ", enableCheckpointing=" << (enableCheckpointing ? "true" : "false")
           << ", enableReplay=" << (enableReplay ? "true" : "false")
           << ", enableDevelopment=" << (enableDevelopment ? "true" : "false")
           << ", checkpointPath=" << checkpointPath;
        return ss.str();
    }
};

/**
 * Test assertion result
 */
struct TestResult {
    std::string testName;
    bool passed;
    std::string message;
    double executionTime;
    
    TestResult(const std::string& name, bool success, const std::string& msg = "")
        : testName(name), passed(success), message(msg), executionTime(0.0) {}
};

/**
 * Phase 6 integrated experiment runner with enhanced testing framework
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test with comprehensive reporting
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Run a simple integration verification
     */
    bool verifyIntegration();
    
    /**
     * Test memory integration
     */
    bool testMemoryIntegration();
    
    /**
     * Test neuromodulation integration
     */
    bool testNeuromodulationIntegration();
    
    /**
     * Test checkpoint save/load
     */
    bool testCheckpointing();
    
    /**
     * Test replay system
     */
    bool testReplay();
    
    /**
     * Run all integration tests and return detailed results
     */
    Phase6IntegrationResult runAllTests(const Phase6Config& config);
    
    /**
     * Validate configuration parameters
     */
    static bool validateConfig(const Phase6Config& config);
    
    /**
     * Format test results for output
     */
    static std::string formatResults(const Phase6IntegrationResult& result);
    
    /**
     * Format test results in human-readable format
     */
    static void printResults(const Phase6IntegrationResult& result);
    
    /**
     * Print test summary
     */
    static void printSummary(const std::vector<TestResult>& testResults);

private:
    // Helper methods for testing
    bool runIntegrationVerification(const Phase6Config& config, Phase6IntegrationResult& result);
    bool runMemoryIntegrationTests(const Phase6Config& config, Phase6IntegrationResult& result);
    bool runNeuromodulationIntegrationTests(const Phase6Config& config, Phase6IntegrationResult& result);
    bool runCheckpointIntegrationTests(const Phase6Config& config, Phase6IntegrationResult& result);
    bool runReplayIntegrationTests(const Phase6Config& config, Phase6IntegrationResult& result);
    
    // Utility functions
    std::vector<TestResult> executeTestSuite(const std::string& suiteName, const std::vector<std::function<bool()>>& tests);
    double getCurrentTime() const;
};

} // namespace nlm
