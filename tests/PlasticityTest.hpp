// NLM (熙然) - Neural Learning Machine
// Test Module: Plasticity Learning
// This test demonstrates activity-dependent synaptic changes through repeated input patterns

#ifndef NLM_TESTS_PLASTICITY_TEST_HPP
#define NLM_TESTS_PLASTICITY_TEST_HPP

#include <memory>
#include "brain/Brain.hpp"

namespace nlm::tests {

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

class PlasticityTest {
public:
    static void run(std::shared_ptr<Brain> brain);
    static void runWithConfig(std::shared_ptr<Brain> brain, const std::string& mode);
};

} // namespace nlm::tests

#endif // NLM_TESTS_PLASTICITY_TEST_HPP
