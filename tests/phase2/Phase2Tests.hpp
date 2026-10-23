// NLM Phase 2 Test Framework
// Contains experimental verification tests for Neural Learning Machine

#ifndef NLM_PHASE2_TESTS_HPP
#define NLM_PHASE2_TESTS_HPP

#include "brain/Brain.hpp"
#include <string>

namespace nlm {

// Learning experiment data structure
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

// Test function declarations
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

// Helper functions
void printBanner();
void logConfiguration(std::shared_ptr<Config> config);

} // namespace nlm

#endif // NLM_PHASE2_TESTS_HPP
