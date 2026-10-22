#pragma once

#include "Brain.hpp"

namespace nlm {

// Statistics
float Brain::getExcitationInhibitionRatio() const;
size_t Brain::getTotalSpikeCount() const;
size_t Brain::getPendingSpikeEventCount() const;
size_t Brain::getTotalNeuronCount() const;
size_t Brain::getTotalSynapseCount() const;
size_t Brain::getActiveNeuronCount() const;
size_t Brain::getFiringNeuronCount() const;
float Brain::getAverageFiringRate() const;

} // namespace nlm
