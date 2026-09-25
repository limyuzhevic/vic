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
    bool curiosityIntegrated;
    bool noveltyIntegrated;
    bool rewardIntegrated;
    bool planningIntegrated;
    bool attentionIntegrated;
    bool structuralPlasticityIntegrated;
    bool synaptogenesisIntegrated;
    bool pruningIntegrated;
    bool homeostaticPlasticityIntegrated;
    bool checkpointingWorks;
    bool replayWorks;
    bool crossSystemConnectivity;
    bool feedbackLoopsFunctional;
    
    // Metrics
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    float serotoninLevel;
    float acetylcholineLevel;
    float learningRate;
    float plasticityStrength;
    float memoryStability;
    float cognitiveFlexibility;
    
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
        , curiosityIntegrated(false)
        , noveltyIntegrated(false)
        , rewardIntegrated(false)
        , planningIntegrated(false)
        , attentionIntegrated(false)
        , structuralPlasticityIntegrated(false)
        , synaptogenesisIntegrated(false)
        , pruningIntegrated(false)
        , homeostaticPlasticityIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        , crossSystemConnectivity(false)
        , feedbackLoopsFunctional(false)
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , avgSynapticWeight(0.0f)
        , memoryEpisodesStored(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , serotoninLevel(0.0f)
        , acetylcholineLevel(0.0f)
        , learningRate(0.0f)
        , plasticityStrength(0.0f)
        , memoryStability(0.0f)
        , cognitiveFlexibility(0.0f)
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
    bool enableAdvancedTesting;
    bool enableCrossSystemValidation;
    bool enableFeedbackAnalysis;
    bool enableStressTesting;
    float stressTestMultiplier;
    bool enableRealisticScenarios;
    std::string checkpointPath;
    std::string outputPath;
    uint64_t dataCollectionInterval;
    size_t validationSampleSize;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , enableAdvancedTesting(false)
        , enableCrossSystemValidation(false)
        , enableFeedbackAnalysis(false)
        , enableStressTesting(false)
        , stressTestMultiplier(1.0f)
        , enableRealisticScenarios(false)
        , checkpointPath("./checkpoint_test.bin")
        , outputPath("./integration_results.json")
        , dataCollectionInterval(100)
        , validationSampleSize(10) {}
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
