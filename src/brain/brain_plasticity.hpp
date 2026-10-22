#pragma once

#include "Brain.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"

namespace nlm {

// Core neural computation accessors
SpikeSystem* Brain::getSpikeSystem();
const SpikeSystem* Brain::getSpikeSystem() const;
STDP* Brain::getSTDP();
Hebbian* Brain::getHebbian();
StructuralPlasticity* Brain::getStructuralPlasticity();

// Plasticity and development systems
void Brain::updatePlasticity();
void Brain::develop();
void Brain::reset();

} // namespace nlm
