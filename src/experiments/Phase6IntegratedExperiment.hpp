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
 * 
 * Advanced Features:
 * - Multiple configuration profiles (BEGINNER, INTERMEDIATE, ADVANCED)
 * - Performance profiling support
 * - Metrics export for analysis
 * - Visualization output generation
 * - Better error handling and user feedback
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
    // Advanced features
    bool enableProfiling;
    bool enableMetricsExport;
    bool enableVisualization;
    std::string metricsOutputPath;
    std::string visualizationOutputPath;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(kt_1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , enableProfiling(false)
        , enableMetricsExport(false)
        , enableVisualization(false)
        , checkpointPath("./checkpoint_test.bin")
        , metricsOutputPath("")
        , visualizationOutputPath("") {}
    
    // Helper method to create configuration for different user levels
    static Phase6Config createBeginnerConfig() {
        Phase6Config config;
        config.neuronCount = 200;
        config.maxSteps = 1000;
        config.regionCount = 1;
        config.connectionProbability = 0.15f;
        config.enableCheckpointing = true;
        config.enableReplay = false;
        config.enableDevelopment = false;
        config.enableProfiling = false;
        config.enableMetricsExport = false;
        config.enableVisualization = false;
        return config;
    }
    
    static Phase6Config createIntermediateConfig() {
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 3000;
        config.regionCount = 2;
        config.connectionProbability = 0.12f;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.enableProfiling = false;
        config.enableMetricsExport = true;
        config.enableVisualization = false;
        return config;
    }
    
    static Phase6Config createAdvancedConfig() {
        Phase6Config config;
        config.neuronCount = 1000;
        config.maxSteps = 10000;
        config.regionCount = 3;
        config.connectionProbability = 0.1f;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.enableProfiling = true;
        config.enableMetricsExport = true;
        config.enableVisualization = true;
        config.metricsOutputPath = "metrics.csv";
        config.visualizationOutputPath = "visualization/";
        return config;
    }
    
    // Configuration profile names
    enum ConfigProfile {
        QUICK_TEST,
        BEGINNER_EXPERIMENT,
        INTERMEDIATE_EXPERIMENT,
        ADVANCED_EXPERIMENT,
        CUSTOM
    };
    
    static Phase6Config createFromProfile(ConfigProfile profile) {
        switch (profile) {
            case QUICK_TEST:
                return createBeginnerConfig();
            case BEGINNER_EXPERIMENT:
                return createBeginnerConfig();
            case INTERMEDIATE_EXPERIMENT:
                return createIntermediateConfig();
            case ADVANCED_EXPERIMENT:
                return createAdvancedConfig();
            case CUSTOM:
            default:
                return Phase6Config();
        }
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
