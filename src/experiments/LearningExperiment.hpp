// NLM (熙然) - Neural Learning Machine
// Learning Experiment Class
//
// This class encapsulates the learning experiment functionality
// that was previously embedded as a struct in main.cpp

#pragma once

#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <numeric>
#include <cmath>

namespace nlm {

// LearningExperiment: Demonstrates measurable synaptic changes through experience
class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    ~LearningExperiment() = default;
    
    // Record initial brain state
    void recordInitialState();
    
    // Record final brain state  
    void recordFinalState();
    
    // Compute and display learning statistics
    void computeStatistics();
    
    // Getters for recorded data
    size_t getInitialSynapseCount() const { return initialSynapseCount; }
    size_t getFinalSynapseCount() const { return finalWeights.size(); }
    float getInitialMeanWeight() const { 
        if (initialWeights.empty()) return 0.0f;
        float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        return sum / initialWeights.size(); 
    }
    float getFinalMeanWeight() const {
        if (finalWeights.empty()) return 0.0f;
        float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        return sum / finalWeights.size();
    }
    
    // Check if learning occurred
    bool didLearningOccur() const {
        return (std::abs(getFinalMeanWeight() - getInitialMeanWeight()) > 0.001f) ||
               (strengthened > 0 || weakened > 0);
    }
    
    // Get counts of synaptic changes
    size_t getStrengthenedCount() const { return strengthened; }
    size_t getWeakenedCount() const { return weakened; }
    size_t getUnchangedCount() const { return unchanged; }
    
private:
    std::shared_ptr<Brain> brain_;
    uint64_t seed_;
    size_t initialSynapseCount_;
    std::vector<float> initialWeights_;
    std::vector<float> finalWeights_;
    std::vector<NeuronId> mostActiveNeurons_;
    
    // Statistics
    size_t strengthened_;
    size_t weakened_;
    size_t unchanged_;
};

} // namespace nlm