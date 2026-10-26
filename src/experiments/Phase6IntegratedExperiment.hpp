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

namespace nlm {

/**
 * Phase 6 integration test result
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
    
    Phase6IntegrationResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , avgSynapticWeight(0.0f)
        , memoryEpisodesStored(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0) {}
};

/**
 * Configuration validation and documentation for Phase 6 integration experiment
 */
namespace Phase6ConfigValidation {
    
    /**
     * Validate Phase6Config parameters
     * 
     * @param config Configuration to validate
     * @return true if configuration is valid, false otherwise
     */
    bool validate(const Phase6Config& config);
    
    /**
     * Get human-readable error message for validation failure
     * 
     * @param config Configuration to validate
     * @return Error message string, empty if validation passes
     */
    std::string getValidationError(const Phase6Config& config);
    
    /**
     * Apply safe defaults to configuration
     * 
     * @param config Configuration to modify
     * @param targetEnv Target environment (development, testing, production)
     */
    void applySafeDefaults(Phase6Config& config, const std::string& targetEnv = "development");
    
    /**
     * Configuration parameter bounds
     */
    struct Bounds {
        size_t minNeuronCount = 10;
        size_t maxNeuronCount = 10000;
        size_t minRegionCount = 1;
        size_t maxRegionCount = 10;
        float minConnectionProbability = 0.0f;
        float maxConnectionProbability = 1.0f;
        uint64_t minMaxSteps = 100;
        uint64_t maxMaxSteps = 1000000;
        size_t minReplayCount = 1;
        size_t maxReplayCount = 100;
    };
    
    /**
     * Global bounds instance
     */
    inline Bounds defaultBounds;
}

/**
 * Configuration for Phase 6 integration experiment
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
    size_t replayCount;
    
    /**
     * Default constructor with safe values
     */
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin")
        , replayCount(10) {}
    
    /**
     * Enhanced constructor with parameter validation
     */
    Phase6Config(uint64_t steps, size_t neurons, size_t regions, float connProb)
        : maxSteps(std::max(steps, Phase6ConfigValidation::defaultBounds.minMaxSteps))
        , neuronCount(std::clamp(neurons, Phase6ConfigValidation::defaultBounds.minNeuronCount, 
                                 Phase6ConfigValidation::defaultBounds.maxNeuronCount))
        , regionCount(std::clamp(regions, Phase6ConfigValidation::defaultBounds.minRegionCount,
                                 Phase6ConfigValidation::defaultBounds.maxRegionCount))
        , connectionProbability(std::clamp(connProb, Phase6ConfigValidation::defaultBounds.minConnectionProbability,
                                              Phase6ConfigValidation::defaultBounds.maxConnectionProbability))
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin")
        , replayCount(10) {}
    
    /**
     * Validate this configuration
     * 
     * @return true if configuration is valid
     */
    bool isValid() const {
        return Phase6ConfigValidation::validate(*this);
    }
    
    /**
     * Get validation error message
     * 
     * @return Error message or empty string if valid
     */
    std::string getValidationError() const {
        return Phase6ConfigValidation::getValidationError(*this);
    }
    
    /**
     * Apply safe defaults for specific environment
     */
    void applySafeDefaults(const std::string& env = "development") {
        Phase6ConfigValidation::applySafeDefaults(*this, env);
    }
    
    /**
     * Get configuration summary for logging
     */
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Phase6Config[maxSteps=" << maxSteps
            << ", neuronCount=" << neuronCount
            << ", regionCount=" << regionCount
            << ", connectionProbability=" << connectionProbability
            << ", enableCheckpointing=" << (enableCheckpointing ? "true" : "false")
            << ", enableReplay=" << (enableReplay ? "true" : "false")
            << ", enableDevelopment=" << (enableDevelopment ? "true" : "false")
            << ", checkpointPath=" << checkpointPath
            << ", replayCount=" << replayCount << "]";
        return oss.str();
    }
    
    /**
     * Print configuration to log
     */
    void logConfig() const {
        NLM_LOG_INFO("=== Phase 6 Configuration ===");
        NLM_LOG_INFO("maxSteps: " + std::to_string(maxSteps));
        NLM_LOG_INFO("neuronCount: " + std::to_string(neuronCount));
        NLM_LOG_INFO("regionCount: " + std::to_string(regionCount));
        NLM_LOG_INFO("connectionProbability: " + std::to_string(connectionProbability));
        NLM_LOG_INFO("enableCheckpointing: " + std::string(enableCheckpointing ? "true" : "false"));
        NLM_LOG_INFO("enableReplay: " + std::string(enableReplay ? "true" : "false"));
        NLM_LOG_INFO("enableDevelopment: " + std::string(enableDevelopment ? "true" : "false"));
        NLM_LOG_INFO("checkpointPath: " + checkpointPath);
        NLM_LOG_INFO("replayCount: " + std::to_string(replayCount));
        NLM_LOG_INFO("==========================");
    }
};

/**
 * Phase 6 integrated experiment runner
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test
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
};

} // namespace nlm
