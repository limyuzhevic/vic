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
 * Includes all configuration parameters needed for comprehensive brain integration testing,
 * including plasticity settings, developmental parameters, and optimization controls.
 */
struct Phase6Config {
    // Simulation parameters
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    
    // Plasticity configuration (optimization controls)
    float stdpLtpWeight;         // STDP Long-Term Potentiation weight
    float stdpLtdWeight;         // STDP Long-Term Depression weight
    float synaptogenesisRate;    // Rate of new synapse formation during development
    float pruningRate;          // Rate of synapse elimination during development
    float plasticityLearningRate; // General plasticity learning rate
    
    // Development and maturation
    bool enableDevelopment;
    float developmentSynaptogenesisRate;
    float developmentPruningRate;
    
    // Neuromodulation
    float dopamineBaseline;
    float noveltyThreshold;
    float curiosityLevel;
    
    // Feature flags
    bool enableCheckpointing;
    bool enableReplay;
    bool enableCuriosity;
    bool enableNovelty;
    
    // Persistence
    std::string checkpointPath;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        // Plasticity defaults
        , stdpLtpWeight(0.01f)
        , stdpLtdWeight(0.012f)
        , synaptogenesisRate(0.0001f)
        , pruningRate(0.00001f)
        , plasticityLearningRate(0.01f)
        // Development defaults
        , enableDevelopment(true)
        , developmentSynaptogenesisRate(0.001f)
        , developmentPruningRate(0.0001f)
        // Neuromodulation defaults
        , dopamineBaseline(0.1f)
        , noveltyThreshold(0.1f)
        , curiosityLevel(1.0f)
        // Feature flags
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableCuriosity(true)
        , enableNovelty(true)
        // Persistence
        , checkpointPath("./checkpoint_test.bin") {}
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
