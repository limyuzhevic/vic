#pragma once

#include <cstdint>
#include <type_traits>

namespace nlm {

// Type aliases for the NLM system
using NeuronId = uint64_t;
using SynapseId = uint64_t;
using RegionId = uint64_t;
using SimulationStep = uint64_t;
using Timestamp = double;
using SynapticWeight = float;
using Delay = uint64_t;

// Additional type aliases used throughout the system
using TimestepDuration = Timestamp;

// Synapse type
enum class SynapseType {
    Excitatory,
    Inhibitory
};

// Event types
struct SpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep simulationStep;
    
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), simulationStep(s) {}
};

struct DelayedSpikeEvent {
    NeuronId source_neuron;
    NeuronId destination_neuron;
    SynapseId synapse;
    Timestamp timestamp;
    SimulationStep simulationStep;
    
    DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn, Timestamp ts, SimulationStep s)
        : source_neuron(src), destination_neuron(dst), synapse(syn), timestamp(ts), simulationStep(s) {}
};

} // namespace nlm
