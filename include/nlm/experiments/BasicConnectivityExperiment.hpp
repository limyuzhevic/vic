#pragma once

#include "ExperimentParams.hpp"
#include "BaseExperiment.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;
class Synapse;

class BasicConnectivityExperiment : public BaseExperiment {
public:
    BasicConnectivityExperiment() = default;
    ~BasicConnectivityExperiment() override = default;
    
    ExperimentResult run(std::shared_ptr<Brain> brain) override;
    std::string getName() const override { return "Basic Connectivity Test"; }
    
    ExperimentParams getDefaultParams() const override {
        ExperimentParams params;
        params.neurons_to_stimulate = 10;
        params.current_per_neuron = 50.0f;
        params.duration_steps = 50;
        return params;
    }
};

} // namespace nlm