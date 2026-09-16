#pragma once

#include "../core/Types/Types.hpp"
#include "Brain.hpp"

namespace nlm {

class BrainPlasticity {
public:
    explicit BrainPlasticity(Brain::Impl* impl) : pImpl(impl) {}
    
    void applyPlasticity(SimulationStep currentStep, Timestamp currentTime);
    
private:
    Brain::Impl* pImpl;
};

} // namespace nlm
