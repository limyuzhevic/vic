#pragma once

/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "../experiments/Phase6IntegratedExperiment.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>

namespace nlm {

/**
 * Phase 6 demo configuration
 */
struct Phase6DemoConfig {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    size_t worldWidth;
    size_t worldHeight;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    Phase6DemoConfig()
        : maxSteps(2000)
        , neuronCount(500)
        , regionCount(1)
        , connectionProbability(0.1f)
        , worldWidth(16)
        , worldHeight(16)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin") {}
};

/**
 * Phase 6 demo result
 */
struct Phase6DemoResult {
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
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    
    // Timestamps
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    
    Phase6DemoResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , memoryEpisodesStored(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0) {}
};

/**
 * Phase 6 demo runner
 */
class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    /**
     * Run the complete integration demo
     */
    Phase6DemoResult run(const Phase6DemoConfig& config);
    
private:
    Logger& logger_;
};

} // namespace nlm