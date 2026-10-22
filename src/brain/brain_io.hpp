#pragma once

#include "Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"

namespace nlm {

// Input/output methods
void Brain::receiveSensoryInput(const class SensoryInput& input);
void Brain::injectCurrent(NeuronId neuron, MembranePotential current);
void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current);

// Action production
std::unique_ptr<class Action> Brain::produceAction();

} // namespace nlm
