#pragma once

#include "../brain/Brain.hpp"

namespace nlm {

class BrainPlasticity {
public:
    BrainPlasticity(Brain::Impl* pImpl) : pImpl(pImpl) {}
    
    // Apply all plasticity rules
    void applyPlasticityRules(Timestamp currentTime);
    
    // Apply structural plasticity
    void applyStructuralPlasticity(SimulationStep currentStep);

private:
    Brain::Impl* pImpl;
};

} // namespace nlm