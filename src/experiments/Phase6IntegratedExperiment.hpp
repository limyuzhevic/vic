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
 */
struct Phase6Config {
    // Core simulation parameters
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    
    // System features
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    bool enableNeuromodulation;
    bool enablePrediction;
    bool enableCognition;
    
    // Advanced features
    float plasticityStrength;
    float neuromodulationLevel;
    float learningRate;
    float noveltyThreshold;
    float curiosityGain;
    float dopamineBias;
    
    // Memory system parameters
    size_t workingMemoryCapacity;
    size_t episodicMemoryMaxEpisodes;
    size_t associativeMemorySize;
    float memoryDecayRate;
    
    // Replay and consolidation
    size_t replayInterval;
    size_t consolidationInterval;
    float consolidationThreshold;
    
    // Environment and behavior
    std::string worldType;
    std::string agentType;
    std::string rewardScheme;
    
    // Checkpoint and persistence
    std::string checkpointPath;
    bool compressCheckpoints;
    size_t maxCheckpoints;
    
    // Development and maturation
    float developmentalStageRate;
    float plasticityMaturationRate;
    float neuronMaturationRate;
    
    // Monitoring and logging
    bool enableDetailedLogging;
    std::string logLevel;
    std::string logFile;
    bool enableMetrics;
    
    // Performance optimization
    bool enableMultithreading;
    bool enableSIMD;
    size_t parallelThreads;
    
    // Experimental parameters
    float inputNoiseLevel;
    float outputNoiseLevel;
    std::string initializationStrategy;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , enableNeuromodulation(true)
        , enablePrediction(true)
        , enableCognition(true)
        , plasticityStrength(1.0f)
        , neuromodulationLevel(1.0f)
        , learningRate(0.01f)
        , noveltyThreshold(0.1f)
        , curiosityGain(0.5f)
        , dopamineBias(0.0f)
        , workingMemoryCapacity(500)
        , episodicMemoryMaxEpisodes(1000)
        , associativeMemorySize(100)
        , memoryDecayRate(0.01f)
        , replayInterval(100)
        , consolidationInterval(1000)
        , consolidationThreshold(0.3f)
        , worldType("SimpleWorld")
        , agentType("StandardAgent")
        , rewardScheme("Standard")
        , checkpointPath("./checkpoint_test.bin")
        , compressCheckpoints(true)
        , maxCheckpoints(100)
        , developmentalStageRate(1.0f)
        , plasticityMaturationRate(0.5f)
        , neuronMaturationRate(0.3f)
        , enableDetailedLogging(true)
        , logLevel("INFO")
        , logFile("")
        , enableMetrics(true)
        , enableMultithreading(false)
        , enableSIMD(true)
        , parallelThreads(4)
        , inputNoiseLevel(0.0f)
        , outputNoiseLevel(0.0f)
        , initializationStrategy("Random") {}
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
