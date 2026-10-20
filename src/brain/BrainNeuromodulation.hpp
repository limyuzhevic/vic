#pragma once

#include "../brain/Brain.hpp"

namespace nlm {

class BrainNeuromodulation {
public:
    BrainNeuromodulation(Brain::Impl* pImpl) : pImpl(pImpl) {}
    
    // Apply neuromodulation effects on neural excitability
    void applyNeuromodulationEffects(Timestamp currentTime);
    
    // Apply plasticity rules with neuromodulation
    void applyPlasticityRules(Timestamp currentTime);

private:
    Brain::Impl* pImpl;
};

} // namespace nlm