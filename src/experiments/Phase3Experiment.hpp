#pragma once

#include <string>
#include <map>
#include <vector>
#include <string>
#include "../core/Types/Types.hpp"

namespace nlm {

// Simple metrics collector for Phase 3 experiments
struct ExperimentMetrics {
    // Episode metrics
    int episode;
    int steps;
    float totalReward;
    float finalEnergy;
    float finalNovelty;
    float finalCuriosity;
    float predictionError;
    
    // Development
    float developmentalAge;
    std::string developmentalStage;
    
    // Network state
    size_t totalSynapses;
    float averageWeight;
    float maxWeight;
    size_t totalSpikes;
    float averageFiringRate;
    
    // Motor behavior
    int forwardCount;
    int backwardCount;
    int turnLeftCount;
    int turnRightCount;
    int interactCount;
    int waitCount;
    
    // Experimental tracking
    std::string experimentName;
    
    ExperimentMetrics();
    void reset();
    std::string toCSV() const;
    
    // Get experiment name
    const std::string& getExperimentName() const { return experimentName; }
    
    // Set experiment name
    void setExperimentName(const std::string& name) { experimentName = name; }
};

} // namespace nlm