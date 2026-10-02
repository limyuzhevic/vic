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

#include "Phase6Config.hpp"
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
 * Phase 6 integrated experiment runner
 * 
 * This class runs the Phase 6 integration experiment using the unified
 * Phase6Config configuration system. It demonstrates that all brain
 * systems are properly connected and functional.
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test
     * 
     * @param config Configuration for the Phase 6 integration experiment
     * @return Integration test results
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Run a simple integration verification
     * 
     * @return true if integration is successful, false otherwise
     */
    bool verifyIntegration();
    
    /**
     * Test memory integration
     * 
     * @return true if memory integration is successful, false otherwise
     */
    bool testMemoryIntegration();
    
    /**
     * Test neuromodulation integration
     * 
     * @return true if neuromodulation integration is successful, false otherwise
     */
    bool testNeuromodulationIntegration();
    
    /**
     * Test checkpoint save/load
     * 
     * @return true if checkpointing is successful, false otherwise
     */
    bool testCheckpointing();
    
    /**
     * Test replay system
     * 
     * @return true if replay system is functional, false otherwise
     */
    bool testReplay();
    
    /**
     * Print configuration summary
     * 
     * @param config Configuration to print
     */
    static void printConfiguration(const Phase6Config& config);
    
    /**
     * Create standard configuration presets
     * 
     * @param presetType Type of preset configuration
     */
    static Phase6Config createPreset(const std::string& presetType);
    
private:
    // Internal implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm