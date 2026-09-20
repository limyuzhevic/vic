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

struct Phase6IntegrationResult {
    // System integration status
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    bool replayWorks;
    
    // Performance metrics
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEpisodesStored;
    float memoryWorkingMemoryActiveTraces;
    size_t totalSpikeCount;
    size_t totalSynapseCount;
    size_t totalNeuronCount;
    float memoryConsolidationEfficiency;
    float predictionAccuracy;
    float planningEfficiency;
    float attentionFocusScore;
    float conceptFormationScore;
    float developmentalProgress;
    float neuromodulationBalance;
    float explorationExploitationRatio;
    
    // Neuromodulation metrics
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    float predictionError;
    float rewardPredictionError;
    
    // Time metrics
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    double avgStepTime;
    double simulationEfficiency;
    
    // Memory system metrics
    float episodicMemoryUtilization;
    float workingMemoryUtilization;
    float memoryRetentionRate;
    size_t memoryReplayCount;
    float memoryConsolidationQuality;
    
    // Plasticity and development metrics
    float synapticPlasticityRate;
    float structuralPlasticityRate;
    float developmentalStageProgress;
    float plasticityHomeostasis;
    
    // Integration quality scores
    float systemCohesionScore;
    float integrationCompleteness;
    float emergentBehaviorScore;
    float learningCurveQuality;
    
    // Error and stability metrics
    size_t simulationErrors;
    size_t convergenceFailures;
    float stabilityScore;
    float robustnessScore;
    
    // Resource utilization
    double peakMemoryUsage;
    double averageCPUUsage;
    double networkActivity;
    
    // Behavioral metrics
    float behavioralDiversity;
    float adaptationSpeed;
    float behavioralStability;
    float goalProgress;
    
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
        , memoryWorkingMemoryActiveTraces(0.0f)
        , totalSpikeCount(0)
        , totalSynapseCount(0)
        , totalNeuronCount(0)
        , memoryConsolidationEfficiency(0.0f)
        , predictionAccuracy(0.0f)
        , planningEfficiency(0.0f)
        , attentionFocusScore(0.0f)
        , conceptFormationScore(0.0f)
        , developmentalProgress(0.0f)
        , neuromodulationBalance(0.0f)
        , explorationExploitationRatio(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , predictionError(0.0f)
        , rewardPredictionError(0.0f)
        , totalWallClockTime(0.0)
        , avgStepTime(0.0)
        , simulationEfficiency(0.0)
        , episodicMemoryUtilization(0.0f)
        , workingMemoryUtilization(0.0f)
        , memoryRetentionRate(0.0f)
        , memoryReplayCount(0)
        , memoryConsolidationQuality(0.0f)
        , synapticPlasticityRate(0.0f)
        , structuralPlasticityRate(0.0f)
        , developmentalStageProgress(0.0f)
        , plasticityHomeostasis(0.0f)
        , systemCohesionScore(0.0f)
        , integrationCompleteness(0.0f)
        , emergentBehaviorScore(0.0f)
        , learningCurveQuality(0.0f)
        , simulationErrors(0)
        , convergenceFailures(0)
        , stabilityScore(0.0f)
        , robustnessScore(0.0f)
        , peakMemoryUsage(0.0)
        , averageCPUUsage(0.0)
        , networkActivity(0.0)
        , behavioralDiversity(0.0f)
        , adaptationSpeed(0.0f)
        , behavioralStability(0.0f)
        , goalProgress(0.0f)
        , startTime(0)
        , endTime(0) {}
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
