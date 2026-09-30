// Plasticity.hpp
// Plasticity system interface

#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class PlasticitySystem {
public:
    void initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain);
    void applyPlasticity(SimulationStep currentStep, float neuromodulationFactor);
    
private:
    void applySTDP(Synapse* synapse, SimulationStep currentStep, float modulationFactor);
    void applyHebbian(Synapse* synapse);
    void applyStructuralPlasticity();
    float calculateSTDPWeightChange(Timestamp preTime, Timestamp postTime);
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    double ltpTimeConstant_ = 20.0f;
    double ltdTimeConstant_ = 20.0f;
};

} // namespace nlm
