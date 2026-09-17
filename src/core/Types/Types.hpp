#pragma once

#include <cstdint>
#include <type_traits>

namespace nlm {

// Strongly-typed identifiers for safety

// Neuron identifier
struct NeuronId {
    uint64_t value;
    
    constexpr NeuronId() : value(0) {}
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Synapse identifier
struct SynapseId {
    uint64_t value;
    
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Region identifier
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Population identifier
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Simulation step counter
using SimulationStep = uint64_t;

// Simulation time in seconds (floating point)
using Timestamp = double;

// Timestep duration in seconds
using TimestepDuration = double;

// Index types for arrays
using NeuronIndex = uint64_t;
using SynapseIndex = uint64_t;
using PopulationIndex = uint64_t;
using RegionIndex = uint64_t;

// Synaptic weight type
using SynapticWeight = float;

// Membrane potential type
using MembranePotential = float;

// Neuron firing rate type
using FiringRate = float;

// Connection delay in simulation steps
using Delay = uint32_t;

// Typed enums for neuron types
enum class NeuronType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};

// Typed enums for synapse types
enum class SynapseType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};

// Synaptic plasticity state flags
struct PlasticityFlags {
    bool hebbian : 1;
    bool stdp : 1;
    bool reward_modulated : 1;
    bool structural : 1;
    bool eligible : 1;
    
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

// Developmental state
enum class DevelopmentalStage : uint8_t {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};

// Spike event for event-driven computation
struct SpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep step;
    
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

// Delayed spike event for synaptic transmission with delay
struct DelayedSpikeEvent {
    NeuronId source_neuron;
    NeuronId destination_neuron;
    SynapseId synapse_id;
    SynapticWeight weight;  // Weight at time of spike (frozen)
    SynapseType synapse_type;
    Timestamp timestamp;      // When spike occurs
    Timestamp delivery_time;  // When spike reaches destination (timestamp + delay)
    SimulationStep step;      // Current simulation step
    SimulationStep delivery_step;  // Step when spike should be delivered
    bool is_excitatory;  // Cached for quick lookup
    
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

// Action potential state
enum class FiringState : uint8_t {
    Resting,
    Active,
    Refractory,
    Inhibited
};

// Neuromodulator types for categorization
enum class NeuromodulatorType : uint8_t {
    Dopamine,
    Acetylcholine,
    Norepinephrine,
    Serotonin,
    Novelty,
    Curiosity,
    PredictionError
};

// Observation from sensory processing
struct Observation {
    std::vector<float> vision;
    std::vector<float> touch;
    std::vector<float> internal;
    std::vector<float> proprioception;
    Timestamp timestamp;
    
    Observation()
        : vision(256, 0.0f), touch(8, 0.0f), internal(4, 0.0f), proprioception(6, 0.0f)
        , timestamp(0.0) {}
    
    size_t getTotalFeatureCount() const {
        return vision.size() + touch.size() + internal.size() + proprioception.size();
    }
    
    void normalize() {
        // Simple normalization - scale to [0, 1] range
        auto normalize_vector = [](std::vector<float>& vec) {
            if (!vec.empty()) {
                float max_val = *std::max_element(vec.begin(), vec.end());
                if (max_val > 0.0f) {
                    for (float& val : vec) {
                        val /= max_val;
                    }
                }
            }
        };
        
        normalize_vector(vision);
        normalize_vector(touch);
        normalize_vector(internal);
        normalize_vector(proprioception);
    }
};

// Motor action command
struct MotorCommand {
    enum class Action {
        Wait,
        MoveForward,
        MoveBackward,
        TurnLeft,
        TurnRight,
        Interact,
        LookLeft,
        LookRight
    };
    
    Action action;
    float confidence;
    Timestamp timestamp;
    
    MotorCommand() : action(Action::Wait), confidence(0.0f), timestamp(0.0) {}
    MotorCommand(Action a, float c = 0.0f) : action(a), confidence(c), timestamp(0.0) {}
};

// SimpleWorld dimensions
struct WorldDimensions {
    size_t width;
    size_t height;
    
    WorldDimensions(size_t w = 16, size_t h = 16) : width(w), height(h) {}
    
    size_t getTotalCells() const { return width * height; }
};

// Perception vector (sensory input)
struct SensoryPercept {
    std::vector<float> vision;
    std::vector<float> touch;
    std::vector<float> internal;
    std::vector<float> proprioception;
    
    SensoryPercept()
        : vision(256, 0.0f), touch(8, 0.0f), internal(4, 0.0f), proprioception(6, 0.0f) {}
    
    size_t getVisionSize() const { return vision.size(); }
    size_t getTouchSize() const { return touch.size(); }
    size_t getInternalSize() const { return internal.size(); }
    size_t getProprioceptionSize() const { return proprioception.size(); }
    
    size_t getTotalSize() const {
        return vision.size() + touch.size() + internal.size() + proprioception.size();
    }
};

