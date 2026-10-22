#pragma once

#include "Brain.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Step processing
void Brain::step(SimulationStep currentStep);
void Brain::step(SimulationStep currentStep, Timestamp currentTime);

} // namespace nlm
