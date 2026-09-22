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
    
    // ===== COMPREHENSIVE STATISTICS TRACKING =====
    
    // Memory system performance metrics
    float memoryWorkingMemoryTraceCount;
    float memoryWorkingMemoryCapacity;
    float memoryWorkingMemoryUtilization;
    
    float memoryEpisodicMaxCapacity;
    float memoryEpisodicCurrentCapacity;
    float memoryEpisodicUtilization;
    
    float memoryAssociativeEntryCount;
    float memoryAssociativeCapacity;
    float memoryAssociativeUtilization;
    
    // Neuromodulation system metrics
    float neuromodulationDopamineLevel;
    float neuromodulationDopaminePlasticityFactor;
    float neuromodulationCuriosityLevel;
    float neuromodulationNoveltyLevel;
    float neuromodulationPredictionErrorLevel;
    
    // Prediction system metrics
    float predictionPredictionAccuracy;
    float predictionPredictionError;
    float predictionPredictionConfidences;
    
    // Cognition system metrics
    float cognitionPlannerComplexity;
    float cognitionPlannerDepth;
    float cognitionPlannerNodeCount;
    
    float cognitionConceptCount;
    float cognitionConceptCohesion;
    
    float cognitionAttentionSpan;
    float cognitionInhibitionStrength;
    float cognitionExcitationStrength;
    
    // Development system metrics
    int developmentCurrentStage;
    double developmentDevelopmentalAge;
    float developmentPlasticityFactor;
    
    // Structural plasticity metrics
    float plasticitySynaptogenesisRate;
    float plasticityPruningRate;
    float plasticityConnectionDensity;
    
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
        , totalWallClockTime(0.0)
        // ===== COMPREHENSIVE STATISTICS TRACKING INITIALIZATION =====
        // Memory system
        , memoryWorkingMemoryTraceCount(0.0f)
        , memoryWorkingMemoryCapacity(0.0f)
        , memoryWorkingMemoryUtilization(0.0f)
        , memoryEpisodicMaxCapacity(0.0f)
        , memoryEpisodicCurrentCapacity(0.0f)
        , memoryEpisodicUtilization(0.0f)
        , memoryAssociativeEntryCount(0.0f)
        , memoryAssociativeCapacity(0.0f)
        , memoryAssociativeUtilization(0.0f)
        // Neuromodulation
        , neuromodulationDopamineLevel(0.0f)
        , neuromodulationDopaminePlasticityFactor(0.0f)
        , neuromodulationCuriosityLevel(0.0f)
        , neuromodulationNoveltyLevel(0.0f)
        , neuromodulationPredictionErrorLevel(0.0f)
        // Prediction system
        , predictionPredictionAccuracy(0.0f)
        , predictionPredictionError(0.0f)
        , predictionPredictionConfidences(0.0f)
        // Cognition system
        , cognitionPlannerComplexity(0.0f)
        , cognitionPlannerDepth(0)
        , cognitionPlannerNodeCount(0)
        , cognitionConceptCount(0.0f)
        , cognitionConceptCohesion(0.0f)
        , cognitionAttentionSpan(0.0f)
        , cognitionInhibitionStrength(0.0f)
        , cognitionExcitationStrength(0.0f)
        // Development system
        , developmentCurrentStage(0)
        , developmentDevelopmentalAge(0.0)
        , developmentPlasticityFactor(0.0f)
        // Structural plasticity
        , plasticitySynaptogenesisRate(0.0f)
        , plasticityPruningRate(0.0f)
        , plasticityConnectionDensity(0.0f) {}
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
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
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
