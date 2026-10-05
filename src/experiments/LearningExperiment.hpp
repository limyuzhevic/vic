// Learning Experiment: Demonstrates measurable synaptic changes through experience
// Phase 2: Real Neural Computation Test
// Extracted from main.cpp for better organization and reusability

#ifndef NLM_LEARNING_EXPERIMENT_HPP
#define NLM_LEARNING_EXPERIMENT_HPP

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <numeric>

namespace nlm {

class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    
    void recordInitialState();
    void recordFinalState();
    void computeStatistics() const;
    
    const std::vector<float>& getInitialWeights() const { return initialWeights; }
    const std::vector<float>& getFinalWeights() const { return finalWeights; }
    const std::vector<NeuronId>& getMostActiveNeurons() const { return mostActiveNeurons; }
    size_t getInitialSynapseCount() const { return initialSynapseCount; }
    
    bool learningOccurred() const;
    
private:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
};

} // namespace nlm

#endif // NLM_LEARNING_EXPERIMENT_HPP