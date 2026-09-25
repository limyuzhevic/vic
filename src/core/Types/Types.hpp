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

// Simulation constants
namespace SimConstants {
    // Default timestep for real-time simulation (seconds)
    constexpr TimestepDuration DEFAULT_TIMESTEP = 0.001;  // 1ms
    
    // Default simulation steps for experiment runs
    constexpr SimulationStep DEFAULT_STEPS = 1000;
    
    // Default refractory period (steps)
    constexpr SimulationStep DEFAULT_REFECTORY_STEPS = 5;
    
    // Default refractory period (milliseconds)
    constexpr SimulationStep DEFAULT_REFECTORY_MS = 5;
    
    // Learning threshold for weight changes
    constexpr float WEIGHT_CHANGE_THRESHOLD = 0.01f;
    
    // Learning accumulation threshold for learning detection
    constexpr float LEARNING_ACCUMULATION_THRESHOLD = 0.001f;
    
    // Default neuron count for testing
    constexpr size_t DEFAULT_TEST_NEURON_COUNT = 20;
    
    // Default number of synapses for STDP test
    constexpr size_t DEFAULT_STDP_SYNAPSE_COUNT = 5;
    
    // Default number of trials for learning experiments
    constexpr size_t DEFAULT_LEARNING_TRIALS = 50;
    
    // Default number of neurons for current injection tests
    constexpr size_t DEFAULT_CURRENT_INJECTION_COUNT = 10;
    
    // Log progress every N steps
    constexpr SimulationStep LOG_INTERVAL = 100;
}

// Neuron simulation constants
namespace NeuronConstants {
    // Membrane capacitance (nF)
    constexpr MembranePotential MEMBRANE_CAPACITANCE = 1.0f;
    
    // Membrane time constant (ms)
    constexpr float TIME_CONSTANT = 20.0f;
    
    // Maximum spike history size
    constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Spike-frequency adaptation parameters
    constexpr float ADAPTATION_DECAY_RATE = 0.95f;  // per ms
    constexpr float ADAPTATION_EFFECT = 0.01f;     // mV per unit adaptation
    
    // Adaptation variable change per spike
    constexpr float ADAPTATION_INCREMENT = 1.0f;
    
    // Membrane potential clamp limits (mV)
    constexpr MembranePotential V_MIN = -100.0f;
    constexpr MembranePotential V_MAX = 50.0f;
    
    // Default firing rate initial value (Hz)
    constexpr FiringRate DEFAULT_FIRING_RATE = 0.0f;
    
    // Default adaptation variable initial value
    constexpr float DEFAULT_ADAPTATION = 0.0f;
    
    // Default last spike time (timestamp)
    constexpr float DEFAULT_LAST_SPIKE_TIME = -1.0f;
}

// Synapse simulation constants
namespace SynapseConstants {
    // Default synaptic weight
    constexpr SynapticWeight DEFAULT_WEIGHT = 1.0f;
    
    // Current injection for excitation (nA)
    constexpr MembranePotential EXCITATORY_CURRENT = 60.0f;
    
    // Current injection for inhibition (nA)
    constexpr MembranePotential INHIBITORY_CURRENT = 60.0f;
    
    // Current injection for strong tests (nA)
    constexpr MembranePotential STRONG_CURRENT = 50.0f;
    
    // Current injection for STDP potentiation (nA)
    constexpr MembranePotential STDP_POTENTIATION_CURRENT = 60.0f;
    
    // Default synaptic delay (steps)
    constexpr Delay DEFAULT_DELAY = 0;
    
    // Default synaptic type
    constexpr SynapseType DEFAULT_SYNAPSE_TYPE = SynapseType::Excitatory;
    
    // Default plasticity flags
    constexpr PlasticityFlags DEFAULT_PLASTICITY = PlasticityFlags();
}

// Neuromodulation constants
namespace NeuromodulationConstants {
    // Dopamine parameters for plasticity factor
    constexpr float DOPAMINE_BASELINE = 0.0f;
    constexpr float DOPAMINE_PEAK = 1.0f;
    constexpr float DOPAMINE_DECAY_RATE = 0.1f;  // per ms
    constexpr float DOPAMINE_RELEASE_RATE = 1.0f; // per reward unit
    
    // Default neuromodulation level
    constexpr float DEFAULT_NEUMODULATION_LEVEL = 0.0f;
    
    // Plasticity factor baseline
    constexpr float PLASTICITY_FACTOR_BASE = 0.5f;
    
    // Plasticity factor modulation strength
    constexpr float PLASTICITY_FACTOR_MODULATION = 0.5f;
}

// Memory constants
namespace MemoryConstants {
    // Default working memory capacity
    constexpr size_t WORKING_MEMORY_CAPACITY = 100;
    
    // Default working memory initial capacity
    constexpr size_t INITIAL_WORKING_MEMORY_CAPACITY = 100;
    
    // Default consolidation interval (steps)
    constexpr SimulationStep CONSOLIDATION_INTERVAL = 10;
}

// Motor system constants
namespace MotorConstants {
    // Default motor system planning depth
    constexpr size_t DEFAULT_PLANNING_DEPTH = 5;
    
    // Default motor system planning confidence
    constexpr float DEFAULT_PLANNING_CONFIDENCE = 0.5f;
    
    // Default curiosity level
    constexpr float DEFAULT_CURIOUSITY_LEVEL = 0.5f;
    
    // Default prediction error
    constexpr float DEFAULT_PREDICTION_ERROR = 0.0f;
}

// Development constants
namespace DevelopmentConstants {
    // Maturation parameters
    constexpr float MATURATION_PROGRESS_STEP = 0.01f;
    constexpr float MATURE_THRESHOLD = -55.0f;
    constexpr float MATURE_RESTING_POTENTIAL = -70.0f;
    constexpr float MATURE_TIME_CONSTANT = 20.0f;
}

// Plasticity constants
namespace PlasticityConstants {
    // STDP parameters
    constexpr float STDP_LTP_THRESHOLD = 0.0f;  // Post-synaptic firing after pre-synaptic
    constexpr float STDP_LTD_THRESHOLD = -0.01f; // Post-synaptic firing before pre-synaptic
    constexpr float STDP_MAX_WEIGHT_CHANGE = 0.05f;
    constexpr float STDP_NORMALIZATION_FACTOR = 0.01f;
    
    // Hebbian parameters
    constexpr float HEBBIAN_WEIGHT_INCREMENT = 0.01f;
    constexpr float HEBBIAN_WEIGHT_DECREMENT = 0.01f;
}

// Reward constants
namespace RewardConstants {
    // Reward computation parameters
    constexpr float REWARD_SCALING_FACTOR = 1.0f;
    constexpr float REWARD_HISTORY_SIZE = 100;
    
    // Reward thresholds
    constexpr float POSITIVE_REWARD_THRESHOLD = 1.0f;
    constexpr float NEGATIVE_REWARD_THRESHOLD = -1.0f;
}

// Experiment constants
namespace ExperimentConstants {
    // Phase 1: Basic connectivity
    constexpr size_t PHASE1_NEURONS = 10;
    constexpr size_t PHASE1_STEPS = 50;
    
    // Phase 2: Plasticity
    constexpr size_t PHASE2_INPUT_NEURONS = 20;
    constexpr size_t PHASE2_STEPS = 1000;
    
    // Phase 3: STDP
    constexpr size_t PHASE3_SYNAPSES = 5;
    constexpr size_t PHASE3_TRIALS = 50;
    
    // Phase 4: Learning
    constexpr size_t PHASE4_TRIALS = 100;
    constexpr float PHASE4_SUCCESS_THRESHOLD = 0.8f;
    
    // Phase 5: Development
    constexpr size_t PHASE5_DEVELOPMENT_STEPS = 500;
    
    // Phase 6: Integration
    constexpr size_t PHASE6_EXPERIMENTS = 5;
    constexpr size_t PHASE6_RUNS_PER_EXPERIMENT = 10;
}
