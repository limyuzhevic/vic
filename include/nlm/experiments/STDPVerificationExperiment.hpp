#pragma once

#include "ExperimentParams.hpp"
#include "BaseExperiment.hpp"
#include <memory>
#include <vector>

namespace nlm {

class StdpVerificationExperiment : public BaseExperiment {
public:
    StdpVerificationExperiment() = default;
    ~StdpVerificationExperiment() override = default;
    
    ExperimentResult run(std::shared_ptr<Brain> brain) override;
    std::string getName() const override { return "STDP Verification Experiment"; }
    
    ExperimentParams getDefaultParams() const override {
        ExperimentParams params;
        params.stdp_trials = 50;
        params.test_synapses = 5;
        params.test_neuron_pairs = 1;
        return params;
    }
};

} // namespace nlm