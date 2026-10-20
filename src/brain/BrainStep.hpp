#pragma once

#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "brain/SpikeSystem.hpp"
#include "brain/NeuralWorkingMemory.hpp"
#include <vector>

namespace nlm {

class BrainStep {
public:
    static void processNeurons(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                              TimestepDuration timestep,
                              Timestamp currentTime,
                              size_t& totalSpikesThisStep,
                              SpikeSystem* spikeSystem,
                              std::vector<Neuron*>& sensoryNeurons,
                              NeuralWorkingMemory* workingMemory);
};

} // namespace nlm
