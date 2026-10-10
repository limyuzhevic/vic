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
    
    // Basic metrics
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    
    // Memory retention metrics
    float memoryRetentionDay1;
    float memoryRetentionDay10;
    float memoryRetentionDay100;
    float memoryDecayRate;
    float consolidationEffectiveness;
    
    // Prediction system metrics
    float predictionAccuracyInitial;
    float predictionAccuracyFinal;
    float predictionErrorInitial;
    float predictionErrorFinal;
    float predictionConfidenceInitial;
    float predictionConfidenceFinal;
    
    // Continual learning metrics
    float continualLearningScore;
    float catastrophicForgettingMeasure;
    float transferLearningScore;
    
    // Development metrics
    float plasticityInitial;
    float plasticityFinal;
    float developmentProgressionScore;
    float stageEffectiveness[4];  // Initial, CriticalPeriod, Maturation, Adult
    
    // Multi-system interaction metrics
    float systemSynergyScore;
    float integrationEffectiveness;
    float emergentBehaviorScore;
    
    // Performance metrics
    float learningSpeed;
    float resourceEfficiency;
    float computationalEfficiency;
    
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
        , memoryRetentionDay1(0.0f)
        , memoryRetentionDay10(0.0f)
        , memoryRetentionDay100(0.0f)
        , memoryDecayRate(0.0f)
        , consolidationEffectiveness(0.0f)
        , predictionAccuracyInitial(0.0f)
        , predictionAccuracyFinal(0.0f)
        , predictionErrorInitial(0.0f)
        , predictionErrorFinal(0.0f)
        , predictionConfidenceInitial(0.0f)
        , predictionConfidenceFinal(0.0f)
        , continualLearningScore(0.0f)
        , catastrophicForgettingMeasure(0.0f)
        , transferLearningScore(0.0f)
        , plasticityInitial(0.0f)
        , plasticityFinal(0.0f)
        , developmentProgressionScore(0.0f)
        , systemSynergyScore(0.0f)
        , integrationEffectiveness(0.0f)
        , emergentBehaviorScore(0.0f)
        , learningSpeed(0.0f)
        , resourceEfficiency(0.0f)
        , computationalEfficiency(0.0f)
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0) {
        // Initialize stage effectiveness array
        for (int i = 0; i < 4; ++i) {
            stageEffectiveness[i] = 0.0f;
        }
    }
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
    
    // Scientific validation settings
    uint64_t memoryRetentionTestSteps;
    uint64_t predictionAccuracyTestSteps;
    uint64_t continualLearningTestSteps;
    uint64_t developmentTestSteps;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin")
        , memoryRetentionTestSteps(1000)
        , predictionAccuracyTestSteps(2000)
        , continualLearningTestSteps(5000)
        , developmentTestSteps(8000) {}
};

/**
 * Phase 6 integrated experiment runner
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test with comprehensive scientific validation
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Run a simple integration verification
     */
    bool verifyIntegration();
    
    /**
     * Test memory integration and retention
     */
    bool testMemoryIntegration();
    
    /**
     * Test prediction system integration
     */
    bool testPredictionIntegration();
    
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
     * Test continual learning without catastrophic forgetting
     */
    bool testContinualLearning();
    
    /**
     * Test developmental progression
     */
    bool testDevelopment();
    
    /**
     * Test multi-system interaction and synergy
     */
    bool testMultiSystemInteraction();
    
    /**
     * Calculate overall integration score
     */
    float calculateIntegrationScore(const Phase6IntegrationResult& result) const;
    
    /**
     * Get scientific validation report
     */
    std::string getScientificReport(const Phase6IntegrationResult& result) const;
};

} // namespace nlm
