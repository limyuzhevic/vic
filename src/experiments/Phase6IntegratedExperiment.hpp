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
    
    // Total exploration events and discoveries during the experiment
    // These metrics track how well the agent learned to explore and discover resources
    size_t totalExplorationEvents;
    size_t totalResourceDiscoveries;
    float finalExplorationTendency;
    
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
        , totalExplorationEvents(0)
        , totalResourceDiscoveries(0)
        , finalExplorationTendency(0.5f)
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
    
    // Enhanced experiment parameters
    bool enableResourceDiscovery;      // Enable discovery of valuable resources
    bool enableHazardAvoidance;        // Enable avoidance of hazards
    bool enableExplorationBonus;       // Enable exploration movement bonus
    bool enableCuriosityDriven;        // Enable curiosity-driven exploration
    float resourceValueRange;           // Min/max resource values
    float hazardValueRange;             // Min/max hazard values
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin")
        , enableResourceDiscovery(true)
        , enableHazardAvoidance(true)
        , enableExplorationBonus(true)
        , enableCuriosityDriven(true)
        , resourceValueRange(10.0f)
        , hazardValueRange(5.0f) {}
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
