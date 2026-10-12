// Core types and utilities for the NLM (Neural Learning Machine) system
// This file contains fundamental type definitions used throughout the NLM framework

#pragma once

#include <cstdint>
#include <type_traits>

namespace nlm {

// Neuron identifier - unique identifier for neurons in the neural network
struct NeuronId {
    uint64_t value;
    
    // Default constructor creates invalid neuron ID
    constexpr NeuronId() : value(0) {}
    
    // Constructor from uint64_t
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    // Equality comparison operators
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    
    // Less-than comparison for ordering
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    // Get index from neuron ID (convenience method)
    constexpr uint64_t index() const { return value; }
};

// Synapse identifier - unique identifier for synapses in the neural network
struct SynapseId {
    uint64_t value;
    
    // Default constructor creates invalid synapse ID
    constexpr SynapseId() : value(0) {}
    
    // Constructor from uint64_t
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    // Equality comparison operators
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    
    // Less-than comparison for ordering
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    // Get index from synapse ID (convenience method)
    constexpr uint64_t index() const { return value; }
};

// Region identifier - unique identifier for neural regions
struct RegionId {
    uint64_t value;
    
    // Default constructor creates invalid region ID
    constexpr RegionId() : value(0) {}
    
    // Constructor from uint64_t
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    // Equality comparison operators
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    
    // Less-than comparison for ordering
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    // Get index from region ID (convenience method)
    constexpr uint64_t index() const { return value; }
};

// Population identifier - unique identifier for neuron populations within regions
struct PopulationId {
    uint64_t value;
    
    // Default constructor creates invalid population ID
    constexpr PopulationId() : value(0) {}
    
    // Constructor from uint64_t
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    // Equality comparison operators
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    
    // Less-than comparison for ordering
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    // Get index from population ID (convenience method)
    constexpr uint64_t index() const { return value; }
};

// Simulation step counter - discrete time steps in the neural simulation
using SimulationStep = uint64_t;

// Simulation time - continuous time in seconds (floating-point for precision)
using Timestamp = double;

// Timestep duration - duration of each simulation step in seconds
using TimestepDuration = double;

// Neuron index - used for array indexing and population management
using NeuronIndex = uint64_t;

// Synapse index - used for array indexing and connectivity management
using SynapseIndex = uint64_t;

// Population index - used for array indexing within regions
using PopulationIndex = uint64_t;

// Region index - used for array indexing across regions
using RegionIndex = uint64_t;

// Synaptic weight - floating-point value representing connection strength
using SynapticWeight = float;

// Membrane potential - electrical potential across neuronal membrane (mV)
using MembranePotential = float;

// Neuron firing rate - spikes per second (Hz)
using FiringRate = float;

// Connection delay - synaptic transmission delay in simulation steps
using Delay = uint32_t;

// Typed enumeration for different neuron types with biological basis
enum class NeuronType : uint8_t {
    Excitatory,    // Excitatory neurons that increase membrane potential
    Inhibitory,   // Inhibitory neurons that decrease membrane potential
    Modulatory,   // Neurons that modulate other neurons' activity
    Sensory,      // Sensory input neurons that receive external stimuli
    Motor,        // Motor output neurons that send signals to effectors
    Internal      // Internal neurons for internal processing
};

// Typed enumeration for different synapse types with functional properties
enum class SynapseType : uint8_t {
    Excitatory,    // Excitatory synapses that depolarize postsynaptic neurons
    Inhibitory,   // Inhibitory synapses that hyperpolarize postsynaptic neurons
    Modulatory,   // Synapses that modulate neuronal excitability
    Electrical,   // Electrical synapses that allow direct ion flow
    GapJunction  // Specialized electrical synapses for fast communication
};

// Synaptic plasticity state flags - bitwise flags for plasticity mechanisms
struct PlasticityFlags {
    bool hebbian : 1;           // Hebbian plasticity (activity-dependent strengthening)
    bool stdp : 1;              // Spike-timing dependent plasticity
    bool reward_modulated : 1; // Reward-modulated plasticity
    bool structural : 1;       // Structural plasticity (formation/removal of connections)
    bool eligible : 1;         // Whether synapse is eligible for plasticity
    
    // Default constructor initializes all flags to false
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

// Developmental state enumeration - represents different stages of neural development
enum class DevelopmentalStage : uint8_t {
    Initial,       // Initial developmental stage
    CriticalPeriod,// Period of high plasticity (critical period)
    Maturation,    // Maturation and stabilization phase
    Adult,         // Adult stable state
    Aging          // Age-related decline and changes
};

// Spike event for event-driven computation - represents neural firing events
struct SpikeEvent {
    NeuronId source_neuron;    // Neuron that generated the spike
    Timestamp timestamp;      // Time when spike occurred
    SimulationStep step;      // Simulation step when spike occurred
    
    // Default constructor creates empty spike event
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    
    // Parameterized constructor
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

// Delayed spike event - represents spikes with transmission delays
struct DelayedSpikeEvent {
    NeuronId source_neuron;        // Source neuron
    NeuronId destination_neuron;   // Target neuron
    SynapseId synapse_id;          // Synapse through which signal travels
    SynapticWeight weight;        // Synaptic weight at time of spike
    SynapseType synapse_type;      // Type of synapse
    Timestamp timestamp;          // When spike occurs at source
    Timestamp delivery_time;      // When spike reaches destination
    SimulationStep step;          // Current simulation step
    SimulationStep delivery_step; // Step when spike should be delivered
    bool is_excitatory;           // Cached for quick lookup
    
    // Default constructor creates empty delayed spike event
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    // Parameterized constructor
    DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                      SynapticWeight w, SynapseType type,
                      Timestamp ts, Timestamp delivery, SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
};

// Action potential state enumeration - represents different phases of neuronal activity
enum class FiringState : uint8_t {
    Resting,    // Neuron at resting potential, not active
    Active,    // Neuron actively processing and potentially spiking
    Refractory,// Neuron in refractory period after spiking
    Inhibited  // Neuron actively inhibited
};

// Generic handle types for efficient memory management and performance
using NeuronHandle = uint64_t;    // Handle for neurons (typically pointer-sized)
using SynapseHandle = uint64_t;   // Handle for synapses (typically pointer-sized)

// Invalid ID sentinel - represents invalid/non-existent identifiers
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

} // namespace nlm