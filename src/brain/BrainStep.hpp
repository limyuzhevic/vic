#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class Brain::Impl;

class BrainStep {
public:
    BrainStep(Brain::Impl* pImpl);
    
    void perform(SimulationStep currentStep, Timestamp currentTime);
    
private:
    Brain::Impl* pImpl;
};

} // namespace nlm