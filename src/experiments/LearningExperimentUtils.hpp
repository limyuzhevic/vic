// Learning Experiment utilities for NLM Phase 2
// Extracted from main.cpp to improve code organization and reusability

#pragma once

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <vector>
#include <string>

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
    bool learningOccurred() const;
    float getWeightChangeMean() const;
};

// Learning experiment utility functions
namespace learning_experiment {
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    void runPlasticityExperiment(std::shared_ptr<Brain> brain);
    void runStdpVerification(std::shared_ptr<Brain> brain);
    void runComprehensiveLearningExperiment(std::shared_ptr<Brain> brain);
}

} // namespace nlm
