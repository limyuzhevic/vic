// Phase 2 Test 2: Plasticity Learning Experiment
// Demonstrates measurable synaptic changes through experience

#pragma once

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Learning experiment structure
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

// Run plasticity learning experiment
int runPlasticityExperimentImpl();

} // namespace nlm