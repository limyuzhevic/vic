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
 * Configuration for Phase 6 integration experiment
 * 
 * Comprehensive configuration supporting all Phase 6 systems and their parameters
 */
struct Phase6Config {
    // Core simulation parameters
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    
    // System enable/disable flags
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    bool enablePrediction;
    bool enableCognition;
    bool enableNoveltyDetection;
    bool enableStructuralPlasticity;
    
    // Memory system parameters
    size_t workingMemoryCapacity;
    size_t episodicMemoryCapacity;
    size_t associativeMemorySize;
    float memoryDecayRate;
    
    // Neuromodulation parameters
    float dopamineLTPWeight;
    float dopamineLTDWeight;
    float curiosityLevel;
    float noveltyThreshold;
    
    // Development parameters
    float developmentalPlasticityRate;
    float criticalPeriodDuration;
    float maturationRate;
    
    // Plasticity parameters
    float stdpLTPWeight;
    float stdpLTDWeight;
    float stdpTau;
    float hebbianLearningRate;
    float synaptogenesisRate;
    float pruningRate;
    
    // Performance and behavior parameters
    float rewardScale;
    float explorationRate;
    float attentionFocus;
    
    // Persistence and replay parameters
    std::string checkpointPath;
    float replayProbability;
    size_t maxReplayEpisodes;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , enablePrediction(true)
        , enableCognition(true)
        , enableNoveltyDetection(true)
        , enableStructuralPlasticity(true)
        , workingMemoryCapacity(100)
        , episodicMemoryCapacity(50)
        , associativeMemorySize(200)
        , memoryDecayRate(0.01f)
        , dopamineLTPWeight(0.01f)
        , dopamineLTDWeight(0.012f)
        , curiosityLevel(0.5f)
        , noveltyThreshold(0.3f)
        , developmentalPlasticityRate(0.1f)
        , criticalPeriodDuration(100.0f)
        , maturationRate(0.05f)
        , stdpLTPWeight(0.01f)
        , stdpLTDWeight(0.012f)
        , stdpTau(20.0f)
        , hebbianLearningRate(0.02f)
        , synaptogenesisRate(0.0001f)
        , pruningRate(0.00001f)
        , rewardScale(1.0f)
        , explorationRate(0.1f)
        , attentionFocus(0.5f)
        , checkpointPath("./checkpoint_test.bin")
        , replayProbability(0.3f)
        , maxReplayEpisodes(10) {}
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
