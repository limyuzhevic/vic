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
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    // ============================================================================
    // MEMORY SYSTEMS CONFIGURATION
    // ============================================================================
    size_t workingMemoryCapacity;      // Working memory capacity
    size_t episodicMemoryMaxEpisodes;  // Maximum number of episodes
    size_t associativeMemoryPatternCount;  // Number of patterns to store
    size_t memoryReplayInterval;       // Steps between replay events
    size_t memoryConsolidationInterval; // Steps between consolidation
    
    // ============================================================================
    // PREDICTION SYSTEM CONFIGURATION
    // ============================================================================
    uint64_t predictionHorizon;        // Look-ahead steps
    float predictionErrorThreshold;    // Threshold for prediction error
    float predictionConfidenceThreshold; // Confidence threshold
    
    // ============================================================================
    // COGNITION SYSTEMS CONFIGURATION
    // ============================================================================
    size_t plannerPlanningDepth;        // Planning horizon depth
    float conceptFormationThreshold;    // Concept formation threshold
    float attentionInhibitionStrength;  // Attention inhibition strength
    float attentionExcitationStrength;   // Attention excitation strength
    float attentionSelectionThreshold;  // Attention selection threshold
    
    // ============================================================================
    // DEVELOPMENT SYSTEM CONFIGURATION
    // ============================================================================
    int64_t developmentalStage;         // Initial developmental stage
    float plasticityRateInitial;       // Initial plasticity rate
    float plasticityRateCriticalPeriod; // Critical period plasticity rate
    float plasticityRateMaturation;     // Maturation plasticity rate
    float plasticityRateAdult;          // Adult plasticity rate
    uint64_t developmentUpdateInterval; // Development update frequency
    
    // ============================================================================
    // NEUROMODULATION SYSTEMS CONFIGURATION
    // ============================================================================
    float dopamineBaselineLevel;       // Baseline dopamine level
    float dopaminePlasticityFactor;    // Dopamine effect on plasticity
    float curiosityExplorationRate;   // Curiosity-driven exploration
    float noveltyThreshold;            // Novelty detection threshold
    float predictionErrorGain;         // Prediction error learning rate
    
    // ============================================================================
    // SENSORY AND MOTOR CONFIGURATION
    // ============================================================================
    float sensoryInputScale;           // Sensory input scaling factor
    float motorOutputStrength;         // Motor output strength
    
    // ============================================================================
    // ENVIRONMENT AND AGENT CONFIGURATION
    // ============================================================================
    uint64_t environmentWidth;         // Environment width
    uint64_t environmentHeight;        // Environment height
    float agentExplorationRate;        // Agent exploration rate
    bool agentCuriosityEnabled;        // Enable curiosity
    bool agentRewardModulationEnabled; // Enable reward modulation
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin")
        // Memory systems
        , workingMemoryCapacity(100)
        , episodicMemoryMaxEpisodes(1000)
        , associativeMemoryPatternCount(500)
        , memoryReplayInterval(100)
        , memoryConsolidationInterval(1000)
        // Prediction system
        , predictionHorizon(10)
        , predictionErrorThreshold(0.1f)
        , predictionConfidenceThreshold(0.8f)
        // Cognition systems
        , plannerPlanningDepth(5)
        , conceptFormationThreshold(0.7f)
        , attentionInhibitionStrength(0.5f)
        , attentionExcitationStrength(1.5f)
        , attentionSelectionThreshold(0.6f)
        // Development system
        , developmentalStage(0)
        , plasticityRateInitial(1.0f)
        , plasticityRateCriticalPeriod(0.8f)
        , plasticityRateMaturation(0.5f)
        , plasticityRateAdult(0.2f)
        , developmentUpdateInterval(1000)
        // Neuromodulation systems
        , dopamineBaselineLevel(0.1f)
        , dopaminePlasticityFactor(1.0f)
        , curiosityExplorationRate(0.3f)
        , noveltyThreshold(0.5f)
        , predictionErrorGain(1.0f)
        // Sensory and motor
        , sensoryInputScale(10.0f)
        , motorOutputStrength(1.0f)
        // Environment and agent
        , environmentWidth(16)
        , environmentHeight(16)
        , agentExplorationRate(0.3f)
        , agentCuriosityEnabled(true)
        , agentRewardModulationEnabled(true) {}

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
