#pragma once

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {

class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    ~LearningExperiment() = default;
    
    // Record initial brain state
    void recordInitialState();
    
    // Record final brain state  
    void recordFinalState();
    
    // Compute and display learning statistics
    void computeStatistics() const;
    
    // Get statistics accessors
    size_t getInitialSynapseCount() const { return initialSynapseCount_; }
    const std::vector<float>& getInitialWeights() const { return initialWeights_; }
    const std::vector<float>& getFinalWeights() const { return finalWeights_; }
    const std::vector<NeuronId>& getMostActiveNeurons() const { return mostActiveNeurons_; }
    size_t getInitialStep() const { return initialStep_; }
    size_t getFinalStep() const { return finalStep_; }
    
    // Determine if learning occurred
    bool didLearningOccur() const;
    
    // Get learning metrics
    float getWeightChange() const;
    size_t getStrengthenedSynapses() const { return strengthened_; }
    size_t getWeakenedSynapses() const { return weakened_; }
    size_t getUnchangedSynapses() const { return unchanged_; }
    
private:
    std::shared_ptr<Brain> brain_;
    uint64_t seed_;
    
    // State tracking
    size_t initialSynapseCount_;
    std::vector<float> initialWeights_;
    std::vector<float> finalWeights_;
    std::vector<NeuronId> mostActiveNeurons_;
    size_t initialStep_;
    size_t finalStep_;
    
    // Statistics
    size_t strengthened_;
    size_t weakened_;
    size_t unchanged_;
};

} // namespace nlm