// Forwarding header for backward compatibility
// Original brain classes moved to src/brain/core/

#pragma once

#include "brain/core/NeuralRegion.hpp"
#include "brain/core/NeuralPopulation.hpp"
#include "brain/core/Neuron.hpp"
#include "brain/core/Synapse.hpp"
#include "brain/core/Brain.hpp"

namespace nlm {
    using NeuralRegion = nlm::NeuralRegion;
    using NeuralPopulation = nlm::NeuralPopulation;
    using Neuron = nlm::Neuron;
    using Synapse = nlm::Synapse;
    using Brain = nlm::Brain;
} // namespace nlm
