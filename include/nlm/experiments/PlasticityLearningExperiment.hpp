#pragma once

#include "ExperimentParams.hpp"
#include "BaseExperiment.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Learning experiment data structure (extracted from original main.cpp)
struct LearningExperimentData {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperimentData(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
};

class PlasticityLearningExperiment : public BaseExperiment {
public:
    PlasticityLearningExperiment() = default;
    ~PlasticityLearningExperiment() override = default;
    
    ExperimentResult run(std::shared_ptr<Brain> brain) override;
    std::string getName() const override { return "Plasticity Learning Experiment"; }
    
    ExperimentParams getDefaultParams() const override {
        ExperimentParams params;
        params.learning_duration = 1000;
        params.sensory_neurons_to_stimulate = 20;
        params.sensory_current = 30.0f;
        return params;
    }
    
    // Helper methods for learning experiment
    void recordInitialState(LearningExperimentData& experiment);
    void recordFinalState(LearningExperimentData& experiment);
    void computeStatistics(LearningExperimentData& experiment);
    bool detectLearning(const LearningExperimentData& experiment) const;
};

} // namespace nlm