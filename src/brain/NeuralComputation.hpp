// NeuralComputation.hpp
// Neural computation system interface

#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include <memory>

namespace nlm {

class NeuralComputationSystem {
public:
    void initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain);
    void processSensoryInput(const class SensoryInput& input);
    void computeStep(SimulationStep currentStep, Timestamp currentTime);
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    size_t neuronCount_;
    size_t regionCount_;
    float connectionProbability_;
    double timestep_;
};

} // namespace nlm
