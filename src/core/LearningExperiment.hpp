#pragma once

#include <string>
#include <vector>
#include <memory>
#include <numeric>
#include "core/Types/Types.hpp"
#include "brain/Brain.hpp"

namespace nlm {

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState();
    void recordFinalState();
    void computeStatistics();
    bool hasLearningOccurred() const;
    float getWeightChangePercentage() const;
};

} // namespace nlm