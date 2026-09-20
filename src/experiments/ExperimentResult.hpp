// ExperimentResult.hpp - Stores results from brain experiments
#pragma once

#include <string>
#include <chrono>

namespace nlm {

struct ExperimentResult {
    bool success;
    std::string message;
    double totalReward;
    double averageFiringRate;
    size_t memoryEpisodesStored;
    float dopamineLevel;
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    std::chrono::duration<double> totalWallClockTime;
    
    ExperimentResult()
        : success(false)
        , totalReward(0.0)
        , averageFiringRate(0.0)
        , memoryEpisodesStored(0)
        , dopamineLevel(0.0f)
        , memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , checkpointingWorks(false)
        , totalWallClockTime(0.0)
    {}
};

} // namespace nlm
