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

// Generic handle types for efficiency
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

// Simulation constants
namespace nlm {

// Time constants (in seconds)
constexpr TimestepDuration SIMULATION_TIMESTEP = 0.001;  // 1 ms timestep
constexpr Timestamp DEVELOPMENT_DURATION_INITIAL = 60.0;    // 60 seconds
constexpr Timestamp DEVELOPMENT_DURATION_CRITICAL = 300.0;   // 5 minutes  
constexpr Timestamp DEVELOPMENT_DURATION_MATURATION = 900.0;  // 15 minutes
constexpr Timestamp DEVELOPMENT_DURATION_ADULT = 900.0;      // After maturation

// Neural constants
constexpr MembranePotential NEURON_RESTING_POTENTIAL = -65.0f;
constexpr MembranePotential NEURON_THRESHOLD = -50.0f;
constexpr MembranePotential LIF_MEMBRANE_TIME_CONSTANT = 20.0f;
constexpr MembranePotential LIF_MEMBRANE_RESISTANCE = 10.0f;

// Plasticity constants
constexpr SynapticWeight STDP_LTP_WEIGHT = 0.02f;
constexpr SynapticWeight STDP_LTD_WEIGHT = 0.015f;
constexpr SynapticWeight STDP_TIME_CONSTANT = 20.0f;
constexpr SynapticWeight SYNAPTOGENESIS_RATE_INITIAL = 0.0001f;
constexpr SynapticWeight PRUNING_RATE_INITIAL = 0.00001f;

// Sensory constants
constexpr size_t SENSE_VISION_WIDTH = 16;
constexpr size_t SENSE_VISION_HEIGHT = 16;
constexpr size_t SENSE_VISION_TOTAL = SENSE_VISION_WIDTH * SENSE_VISION_HEIGHT;

constexpr size_t SENSE_TOUCH_SENSORS = 8;
constexpr size_t SENSE_INTERNAL_SENSORS = 4;
constexpr size_t SENSE_PROPRIOCEPTION_SENSORS = 6;

// Motor constants
constexpr size_t MOTOR_ACTIONS_COUNT = 6;

// Noise and exploration constants
constexpr float EXPLORATION_NOISE_STRENGTH = 0.1f;
constexpr float NOVELTY_DETECTION_THRESHOLD = 0.3f;
constexpr float NOVELTY_DECAY_RATE = 0.99f;

// Memory constants
constexpr size_t WORKING_MEMORY_TRACE_DECAY = 1000;
constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 100;
constexpr size_t SEMANTIC_MEMORY_MAX_CONCEPTS = 1000;

// Performance and logging
constexpr size_t MAX_SIMULATION_STEPS = 10000;
constexpr Timestamp SIMULATION_TIME_LIMIT = 0.0;  // Unlimited

// Reward and neuromodulation
constexpr float REWARD_DISCOUNT_FACTOR = 0.99f;
constexpr float DOPAMINE_BASELINE = 0.0f;
constexpr float DOPAMINE_PEAK = 1.0f;
constexpr float CURIOUSITY_NOVELTY_FACTOR = 2.0f;
constexpr float PREDICTION_ERROR_FACTOR = 0.5f;

// Structural plasticity
constexpr float PLASTICITY_MODIFIER_INITIAL = 1.0f;
constexpr float PLASTICITY_MODIFIER_CRITICAL = 0.8f;
constexpr float PLASTICITY_MODIFIER_MATURATION = 0.5f;
constexpr float PLASTICITY_MODIFIER_ADULT = 0.2f;

// Input scaling factors
constexpr float VISION_INPUT_SCALE = 5.0f;
constexpr float TOUCH_INPUT_SCALE = 8.0f;
constexpr float INTERNAL_INPUT_SCALE = 5.0f;
constexpr float PROPRIOCEPTION_INPUT_SCALE = 3.0f;

// Development stages
constexpr float DEVELOPMENT_STAGE_INITIAL = 0.0f;
constexpr float DEVELOPMENT_STAGE_CRITICAL = 60.0f;
constexpr float DEVELOPMENT_STAGE_MATURATION = 300.0f;
constexpr float DEVELOPMENT_STAGE_ADULT = 900.0f;

// I/O and file operations
constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
constexpr const char* CHECKPOINT_FILE_EXTENSION = ".bin";
constexpr const char* DEFAULT_LOG_FILE = "nlm.log";

// Platform and build
constexpr int CXX_STANDARD = 20;
constexpr bool ENABLE_VISUALIZATION = false;
constexpr float VISUALIZATION_UPDATE_RATE = 30.0f;

} // namespace nlm

// Add the constants to the existing namespace
