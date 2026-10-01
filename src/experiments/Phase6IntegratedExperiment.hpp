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
 * 
 * Comprehensive results from the Phase 6 integration experiment that tests
 * the integration of all brain systems into a coherent artificial brain.
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
    
    // Performance metrics
    float totalReward;                    // Average reward per step
    float avgFiringRate;                  // Average neural firing rate
    float synapticWeightMean;             // Mean synaptic weight across brain
    float synapticWeightStdDev;           // Standard deviation of weights (measures stability)
    
    // Memory metrics
    float episodicEpisodesStored;        // Number of episodic memory episodes formed
    float workingMemoryUsage;             // Working memory active trace count
    float memoryRetention;                // Memory retention quality score (0-1)
    
    // Neuromodulation metrics
    float noveltyLevel;                   // Current novelty detection level
    float curiosityLevel;                 // Current curiosity/exploration level
    float dopamineLevel;                  // Current dopamine (reward prediction error) level
    
    // Learning metrics
    float learningEfficiency;             // Overall learning efficiency score (0-1)
    float predictionAccuracy;             // Prediction system accuracy (0-1)
    float behavioralAdaptation;           // Rate of behavioral adaptation to environment
    
    // Temporal metrics
    time_t startTime;                     // Experiment start timestamp
    time_t endTime;                       // Experiment end timestamp
    double totalWallClockTime;            // Total real-world time elapsed
    
    Phase6IntegrationResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        // Performance metrics
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , synapticWeightMean(0.0f)
        , synapticWeightStdDev(0.0f)
        // Memory metrics
        , episodicEpisodesStored(0.0f)
        , workingMemoryUsage(0.0f)
        , memoryRetention(0.0f)
        // Neuromodulation metrics
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        // Learning metrics
        , learningEfficiency(0.0f)
        , predictionAccuracy(0.0f)
        , behavioralAdaptation(0.0f)
        // Temporal metrics
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0) {}
    
// Calculate overall integration score (0-1, higher is better)
    float calculateIntegrationScore() const;
    
    // String representation of results for output
    std::string toString() const;
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
