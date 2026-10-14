#pragma once

#include <memory>
#include <string>
#include <vector>
#include "brain/Brain.hpp"

namespace nlm {

/**
 * LearningExperiment: Demonstrates measurable synaptic changes through experience
 * 
 * This struct encapsulates the complete workflow of a learning experiment:
 * 1. Record initial brain state (synapse count and weights)
 * 2. Apply external stimuli for learning
 * 3. Record final brain state  
 * 4. Compute and report statistical changes
 * 
 * The experiment can be independently tested and composed within larger
 * test frameworks. It provides detailed logging of synaptic modifications.
 * 
 * @note Extracted from main.cpp to improve modularity and testability.
 *       Members are public for easier test fixture construction.
 */
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
};

} // namespace nlm
