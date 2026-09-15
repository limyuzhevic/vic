// Type definitions for NLM (Neural Learning Machine)
// Strongly-typed identifiers and basic types for the brain simulation

#ifndef NLM_TYPES_HPP
#define NLM_TYPES_HPP

#include <cstdint>
#include <type_traits>

namespace nlm {

// Neuron identifier - unique across the entire brain
struct NeuronId {
    uint64_t value;
    
    constexpr NeuronId() : value(0) {}
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Synapse identifier - unique across the entire brain
struct SynapseId {
    uint64_t value;
    
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Region identifier - represents a brain region
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Population identifier - represents a group of neurons
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Simulation step counter - discrete time steps
using SimulationStep = uint64_t;

// Timestamp - continuous simulation time in seconds
using Timestamp = double;

// Timestep duration - length of each simulation step
using TimestepDuration = double;

// Index types for array access
using NeuronIndex = uint64_t;
using SynapseIndex = uint64_t;
using PopulationIndex = uint64_t;
using RegionIndex = uint64_t;

// Mathematical types for neural computation
using SynapticWeight = float;        // Connection strength
using MembranePotential = float;     // Electrical potential across membrane
using FiringRate = float;            // Spikes per second
using Delay = uint32_t;              // Synaptic delay in steps

// Neuron type classification
enum class NeuronType : uint8_t {
    Excitatory,   // Positive current injection
    Inhibitory,   // Negative current injection
    Modulatory,   // Neuromodulatory signals
    Sensory,      // Sensory input neurons
    Motor,        // Motor output neurons
    Internal      // Internal processing neurons
};

// Synapse type classification
enum class SynapseType : uint8_t {
    Excitatory,   // Depolarizing, increases firing probability
    Inhibitory,   // Hyperpolarizing, decreases firing probability
    Modulatory,   // Neuromodulatory effects
    Electrical,   // Gap junction coupling
    GapJunction   // Direct electrical coupling
};

// Plasticity flags for tracking synapse properties
struct PlasticityFlags {
    bool hebbian : 1;           // Subject to Hebbian learning
    bool stdp : 1;              // Subject to STDP
    bool reward_modulated : 1;  // Responsive to reward signals
    bool structural : 1;        // Can change connectivity
    bool eligible : 1;          // Currently plastic
    
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

// Developmental stage of neural circuit
enum class DevelopmentalStage : uint8_t {
    Initial,       // Newborn, high plasticity
    CriticalPeriod, // Sensitive period for learning
    Maturation,    // Developing circuits
    Adult,         // Mature, stable
    Aging          // Declining function
};

// Spike event for event-driven simulation
struct SpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;    // When spike occurred
    SimulationStep step;    // Which simulation step
    
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

// Delayed spike event for synaptic transmission
struct DelayedSpikeEvent {
    NeuronId source_neuron;
    NeuronId destination_neuron;
    SynapseId synapse_id;
    SynapticWeight weight;      // Synaptic weight at time of spike
    SynapseType synapse_type;
    Timestamp timestamp;       // When spike originated
    Timestamp delivery_time;   // When spike should be delivered
    SimulationStep step;        // Originating step
    SimulationStep delivery_step; // Delivery step
    bool is_excitatory;         // Cached for efficiency
    
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                      SynapticWeight w, SynapseType type,
                      Timestamp ts, Timestamp delivery, SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
};

// Neuronal membrane state
enum class FiringState : uint8_t {
    Resting,   // Below threshold, no firing
    Active,    // Firing action potential
    Refractory, // Cannot fire after spike
    Inhibited  // Actively suppressed
};

// Handle types for efficient memory access
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

// Invalid identifier sentinels
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

} // namespace nlm

#endif // NLM_TYPES_HPP