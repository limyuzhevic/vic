#pragma once

#include <cstdint>
#include <string>

namespace nlm {
namespace constants {

// Neural network constants
namespace neuron {
    constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;  // nF
    constexpr float DEFAULT_TIME_CONSTANT = 20.0f;      // ms
    constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f; // mV
    constexpr float DEFAULT_THRESHOLD = -55.0f;         // mV
    constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;   // mV
    constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;    // nS
    constexpr float DEFAULT_INPUT_RESISTANCE = 100.0f;  // MOhm
    constexpr int DEFAULT_REFACTORY_PERIOD = 5;         // simulation steps
    constexpr float DEFAULT_ADAPTATION_RATE = 0.01f;    // spike-frequency adaptation
    constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;     // safety limit
    constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;   // safety limit
}

namespace synapse {
    constexpr float DEFAULT_WEIGHT = 0.5f;
    constexpr float MAX_WEIGHT = 10.0f;
    constexpr float MIN_WEIGHT = 0.0f;
    constexpr float DEFAULT_DELAY = 2;                  // simulation steps
    constexpr float MAX_DELAY = 100;                    // simulation steps
    constexpr float DEFAULT_CONDUCTANCE = 0.5f;         // nS
    constexpr float MAX_CONDUCTANCE = 10.0f;            // nS
    constexpr float MIN_CONDUCTANCE = 0.0f;             // nS
}

namespace simulation {
    constexpr float DEFAULT_TIMESTEP = 0.001f;         // 1ms
    constexpr float MAX_TIMESTEP = 0.1f;               // 100ms
    constexpr float MIN_TIMESTEP = 0.0001f;            // 0.1ms
    constexpr size_t DEFAULT_MAX_STEPS = 10000;
    constexpr float DEFAULT_SIMULATION_DURATION = 10.0f; // seconds
}

namespace plasticity {
    constexpr float DEFAULT_STDP_LTP = 0.02f;
    constexpr float DEFAULT_STDP_LTD = 0.015f;
    constexpr float DEFAULT_STDP_TAU = 20.0f;           // ms
    constexpr float DEFAULT_STDP_TRACE_DECAY = 0.9f;    // decay rate
    constexpr float DEFAULT_HEBBIAN_RATE = 0.01f;
    constexpr float DEFAULT_REWARD_MODULATION_RATE = 0.1f;
}

namespace development {
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    constexpr float DEFAULT_MATURATION_RATE = 0.001f;
    constexpr float DEFAULT_CRITICAL_PERIOD_START = 1.0f;   // seconds
    constexpr float DEFAULT_CRITICAL_PERIOD_END = 10.0f;    // seconds
}

namespace memory {
    constexpr size_t DEFAULT_WORKING_MEMORY_SIZE = 100;
    constexpr size_t DEFAULT_EPISODIC_MEMORY_CAPACITY = 1000;
    constexpr float DEFAULT_EPISODIC_MEMORY_RECALL_PROBABILITY = 0.8f;
    constexpr float DEFAULT_PATTERN_COMPLETION_THRESHOLD = 0.7f;
}

namespace neuromodulation {
    constexpr float DEFAULT_DOPAMINE_RELEASE = 0.1f;
    constexpr float DEFAULT_CURIOUSITY_RATE = 0.05f;
    constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.5f;
    constexpr float DEFAULT_PREDICTION_ERROR_RATE = 0.01f;
}

namespace experiments {
    constexpr size_t MIN_NEURON_COUNT = 10;
    constexpr size_t MAX_NEURON_COUNT = 100000;
    constexpr size_t MIN_REGION_COUNT = 1;
    constexpr size_t MAX_REGION_COUNT = 100;
    constexpr float MIN_CONNECTION_PROBABILITY = 0.0f;
    constexpr float MAX_CONNECTION_PROBABILITY = 1.0f;
    constexpr float MIN_INPUT_CURRENT = 0.0f;
    constexpr float MAX_INPUT_CURRENT = 1000.0f;
}

// Unit conversions
namespace units {
    constexpr float MS_TO_SECONDS = 0.001f;
    constexpr float SECONDS_TO_MS = 1000.0f;
    constexpr float MV_TO_V = 0.001f;
    constexpr float V_TO_MV = 1000.0f;
    constexpr float NS_TO_S = 0.000000001f;
    constexpr float S_TO_NS = 1000000000.0f;
    constexpr float HZ_TO_S = 1.0f;
    constexpr float S_TO_HZ = 1.0f;
}

// Error thresholds
namespace thresholds {
    constexpr float WEIGHT_CHANGE_DETECTION = 0.01f;
    constexpr float LEARNING_DETECTION = 0.001f;
    constexpr float SPIKE_DETECTION = 0.1f;
    constexpr float NOISE_THRESHOLD = 0.001f;
    constexpr float CONVERGENCE_THRESHOLD = 0.0001f;
}

} // namespace constants

// Enumeration types
enum class NeuronType {
    Sensory = 0,
    Internal = 1,
    Motor = 2,
    Excitatory = 3,
    Inhibitory = 4,
    Modulatory = 5
};

enum class SynapseType {
    Excitatory = 0,
    Inhibitory = 1,
    Neuromodulatory = 2,
    Gap = 3
};

enum class FiringState {
    Resting = 0,
    Active = 1,
    Refractory = 2
};

enum class PlasticityType {
    Hebbian = 0,
    STDP = 1,
    RewardModulated = 2,
    Structural = 3
};

enum class DevelopmentStage {
    Initial = 0,
    CriticalPeriod = 1,
    Maturation = 2,
    Adult = 3
};

// Type definitions for consistency
using NeuronId = uint32_t;
using SynapseId = uint32_t;
using RegionId = uint32_t;
using PopulationId = uint32_t;
using Timestamp = double;           // seconds
using TimestepDuration = double;    // seconds
using MembranePotential = float;    // mV
using FiringRate = float;           // Hz
using SynapticWeight = float;
using Delay = size_t;               // simulation steps
using SimulationStep = uint64_t;
using NeuronCount = size_t;
using ConnectionProbability = float;
using TimestampMS = uint64_t;

// Invalid type constants
constexpr NeuronId INVALID_NEURON_ID = static_cast<NeuronId>(-1);
constexpr SynapseId INVALID_SYNAPSE_ID = static_cast<SynapseId>(-1);
constexpr RegionId INVALID_REGION_ID = static_cast<RegionId>(-1);
constexpr PopulationId INVALID_POPULATION_ID = static_cast<PopulationId>(-1);
constexpr float INVALID_VALUE = -999999.0f;

} // namespace nlm
