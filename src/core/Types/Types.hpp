#pragma once

// Enhanced type system for NLM brain simulation
// Provides comprehensive type definitions, constants, and utilities

#include <cstdint>
#include <type_traits>
#include <string_view>
#include <optional>
#include <limits>
#include <cmath>
#include <algorithm>

namespace nlm {

// ==================== CORE TYPE ALIASES ====================

// Strong type aliases for better type safety
using NeuronId = uint64_t;
using SynapseId = uint64_t;
using RegionId = uint64_t;
using PopulationId = uint64_t;

// Simulation parameters with appropriate precision
using SimulationStep = uint64_t;              // Discrete step counter
using Timestamp = double;                     // Continuous time in seconds
using TimestepDuration = double;              // Duration between steps (seconds)
using SynapticWeight = float;                  // Synaptic efficacy [-1.0, 1.0]
using MembranePotential = float;               // Membrane voltage (mV)
using FiringRate = float;                      // Neural firing frequency (Hz)
using Delay = uint32_t;                        // Synaptic delay (simulation steps)

// Performance-critical types
using NeuronIndex = uint64_t;
using SynapseIndex = uint64_t;
using PopulationIndex = uint64_t;
using RegionIndex = uint64_t;

// ==================== CONSTANTS AND MATHEMATICAL DEFINITIONS ====================

namespace Constants {
    // Biological constraints and typical values
    namespace Neuron {
        constexpr float MEMBRANE_CAPACITANCE = 1.0f;      // nF
        constexpr float TIME_CONSTANT = 20.0f;           // ms
        constexpr float RESTING_POTENTIAL = -70.0f;      // mV
        constexpr float RESET_POTENTIAL = -70.0f;        // mV
        constexpr float DEFAULT_THRESHOLD = -55.0f;      // mV
        constexpr float THRESHOLD_RANGE_MIN = -65.0f;     // mV
        constexpr float THRESHOLD_RANGE_MAX = -45.0f;     // mV
        constexpr float LEAK_CONDUCTANCE = 10.0f;        // nS
        constexpr float ADAPTATION_MAX = 10.0f;          // spike-frequency adaptation
    }
    
    namespace Synapse {
        constexpr SynapticWeight MIN_WEIGHT = -1.0f;      // Strong inhibition
        constexpr SynapticWeight MAX_WEIGHT = 1.0f;       // Strong excitation
        constexpr SynapticWeight DEFAULT_EXCITATORY = 0.2f;
        constexpr SynapticWeight DEFAULT_INHIBITORY = -0.2f;
        constexpr Delay DEFAULT_DELAY = 1;               // 1ms at 1kHz timestep
        constexpr float EFFICACY_MAX = 2.0f;
        constexpr float EFFICACY_MIN = 0.1f;
    }
    
    namespace Plasticity {
        // STDP parameters
        constexpr float STDP_LTP_DEFAULT = 0.01f;         // Potentiation amplitude
        constexpr float STDP_LTD_DEFAULT = 0.012f;        // Depression amplitude
        constexpr float STDP_TIME_CONSTANT = 20.0f;       // Time window (ms)
        constexpr float STDP_MIN_DELTA = 1e-6f;           // Convergence threshold
        
        // Hebbian parameters
        constexpr float HEBBIAN_LEARNING_RATE = 0.01f;    // Learning rate
        constexpr float HEBBIAN_CORRELATION_WINDOW = 100.0f; // 100ms correlation window
        
        // Structural plasticity
        constexpr float SYNAPTOGENESIS_RATE = 0.0001f;    // New synapses per neuron per step
        constexpr float PRUNING_RATE = 0.00001f;          // Synapse removal rate
        
        // Eligibility traces
        constexpr float ELIGIBILITY_DECAY = 0.999f;       // Per-step decay
        constexpr float REWARD_STRENGTH = 0.1f;           // Reward modulation strength
    }
    
    namespace Memory {
        constexpr size_t WORKING_MEMORY_CAPACITY_FACTOR = 10;  // % of total neurons
        constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;
        constexpr float EPISODIC_CONSOLIDATION_THRESHOLD = 0.3f;
        constexpr size_t REPLAY_INTERVAL = 100;                // Steps between replays
        constexpr size_t CONSOLIDATION_INTERVAL = 1000;        // Steps between consolidations
    }
    
    namespace Neuromodulation {
        constexpr float DOPAMINE_PEAK = 1.0f;                // Reward prediction error peak
        constexpr float DOPAMINE_BASELINE = 0.0f;            // Baseline level
        constexpr float CURIOUSITY_THRESHOLD = 0.5f;         // Novelty detection threshold
        constexpr float NOVELTY_DECAY = 0.95f;               // Novelty decay per step
    }
    
    // Numerical precision
    constexpr float EPSILON = 1e-6f;                       // Floating point epsilon
    constexpr float FLOAT_MIN = std::numeric_limits<float>::min();
    constexpr float FLOAT_MAX = std::numeric_limits<float>::max();
    
    // Simulation limits
    constexpr size_t MAX_NEURONS = 10000;                 // Maximum neurons per region
    constexpr size_t MAX_SYNAPSES = 100000;               // Maximum synapses per region
    constexpr SimulationStep MAX_STEPS = 1000000;         // Maximum simulation steps
}

// ==================== ENUM CLASSES ====================

enum class NeuronType : uint8_t {
    Excitatory = 0,
    Inhibitory = 1,
    Modulatory = 2,
    Sensory = 3,
    Motor = 4,
    Internal = 5,
    Glial = 6,
    StemCell = 7,
    Specialized = 8,
    Interneuron = 9
};

enum class SynapseType : uint8_t {
    Excitatory = 0,
    Inhibitory = 1,
    Modulatory = 2,
    Electrical = 3,
    GapJunction = 4,
    Metabotropic = 5,
    Neuromodulatory = 6,
    Stochastic = 7
};

enum class PlasticityRule : uint8_t {
    STDP = 0,
    Hebbian = 1,
    RewardModulated = 2,
    Structural = 3,
    Homeostatic = 4,
    SynapticTagging = 5,
    TraceBased = 6
};

enum class DevelopmentalStage : uint8_t {
    Initial = 0,
    CriticalPeriod = 1,
    Maturation = 2,
    Adult = 3,
    Aging = 4,
    Senescence = 5
};

enum class FiringState : uint8_t {
    Resting = 0,
    Active = 1,
    Refractory = 2,
    Inhibited = 3,
    Bursting = 4,
    Adapting = 5,
    Spiking = 6,
    Quiescent = 7
};

enum class LogLevel : uint8_t {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// ==================== STRUCTURES AND UNIONS ====================

struct PlasticityFlags {
    bool stdp : 1;           // Spike-timing dependent plasticity
    bool hebbian : 1;        // Hebbian co-activation learning
    bool reward_modulated : 1; // Reward prediction error modulation
    bool structural : 1;     // Structural changes (synaptogenesis/pruning)
    bool homeostatic : 1;    // Homeostatic regulation
    bool eligibility : 1;    // Eligibility trace for delayed learning
    bool enabled : 1;        // General plasticity enable/disable
    
    constexpr PlasticityFlags() noexcept : stdp(false), hebbian(false), reward_modulated(false),
                                           structural(false), homeostatic(false), eligibility(false), enabled(false) {}
    
    // Check if any plasticity mechanism is enabled
    constexpr bool hasAnyPlasticity() const noexcept {
        return stdp || hebbian || reward_modulated || structural || homeostatic;
    }
    
    // Check if learning requires eligibility traces
    constexpr bool requiresEligibilityTrace() const noexcept {
        return reward_modulated || eligibility;
    }
};

struct NeuronState {
    MembranePotential membranePotential;    // Current membrane voltage (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;      // Post-spike reset value (mV)
    MembranePotential leakConductance;     // Leak conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    uint64_t spikeCount;                   // Total spike count since initialization
    
    constexpr NeuronState() noexcept : membranePotential(Constants::Neuron::RESTING_POTENTIAL),
                                       restingPotential(Constants::Neuron::RESTING_POTENTIAL),
                                       threshold(Constants::Neuron::DEFAULT_THRESHOLD),
                                       resetPotential(Constants::Neuron::RESET_POTENTIAL),
                                       leakConductance(Constants::Neuron::LEAK_CONDUCTANCE),
                                       firingRate(0.0f),
                                       firingState(FiringState::Resting),
                                       refractoryRemaining(0),
                                       refractoryPeriod(Constants::Neuron::REFRACTORY_PERIOD_DEFAULT),
                                       adaptationVariable(0.0f),
                                       lastSpikeTime(-1.0f),
                                       spikeCount(0) {}
    
    // Check if neuron is ready to fire
    constexpr bool shouldFire() const noexcept {
        return firingState == FiringState::Active && 
               membranePotential >= threshold &&
               refractoryRemaining == 0;
    }
    
    // Check if neuron is in refractory period
    constexpr bool isRefractory() const noexcept {
        return refractoryRemaining > 0;
    }
    
    // Get time since last spike
    constexpr float timeSinceLastSpike(Timestamp currentTime) const noexcept {
        if (lastSpikeTime < 0.0f) return 0.0f;
        return currentTime - lastSpikeTime;
    }
};

struct SynapseState {
    SynapticWeight weight;                 // Current synaptic weight
    SynapticWeight weightChange;           // Weight change accumulated this step
    float efficacy;                        // Use-dependent synaptic efficacy
    float shortTermDepression;             // Depression factor (Tsodyks-Markram)
    float shortTermFacilitation;           // Facilitation factor (Tsodyks-Markram)
    float eligibilityTrace;                // For reward-modulated learning
    Timestamp lastPreSpike;                // Time of last presynaptic spike
    Timestamp lastPostSpike;               // Time of last postsynaptic spike
    uint32_t spikeCount;                   // Total spike activity count
    
    constexpr SynapseState() noexcept : weight(0.0f), weightChange(0.0f),
                                        efficacy(1.0f), shortTermDepression(1.0f),
                                        shortTermFacilitation(0.0f), eligibilityTrace(0.0f),
                                        lastPreSpike(-1.0f), lastPostSpike(-1.0f),
                                        spikeCount(0) {}
    
    // Check if synapse is effective (can transmit signals)
    constexpr bool isEffective() const noexcept {
        return efficacy > 0.1f && shortTermDepression > 0.01f;
    }
    
    // Calculate instantaneous synaptic current
    constexpr float getInstantaneousCurrent(float preSpikeWeight) const noexcept {
        float effectiveWeight = weight * efficacy * shortTermDepression;
        return effectiveWeight * preSpikeWeight;
    }
};

// ==================== MATHEMATICAL UTILITIES ====================

namespace Math {
    // Safe numerical operations with bounds checking
    template<typename T>
    constexpr T safeClamp(T value, T min, T max) noexcept {
        return std::max(min, std::min(max, value));
    }
    
    // Exponential decay function
    constexpr float expDecay(float value, float timeConstant, float dt) noexcept {
        if (timeConstant <= 0.0f) return 0.0f;
        return value * std::exp(-dt / timeConstant);
    }
    
    // Linear interpolation
    template<typename T>
    constexpr T lerp(T a, T b, float t) noexcept {
        return a + (b - a) * t;
    }
    
    // Smoothstep interpolation (smoother than linear)
    constexpr float smoothstep(float edge0, float edge1, float x) noexcept {
        float t = (x - edge0) / (edge1 - edge0);
        t = std::clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
    
    // Calculate membrane potential change with LIF dynamics
    constexpr float membranePotentialChange(float v, float vRest, float tau, 
                                          float dt, float current) noexcept {
        if (tau <= 0.0f) return v;
        float leak = (vRest - v) / tau;
        float synaptic = current / Constants::Neuron::MEMBRANE_CAPACITANCE;
        return v + dt * 1000.0f * (leak + synaptic);  // Scale time to ms
    }
    
    // Calculate firing rate from spike times
    static float calculateFiringRate(const float* spikeTimes, size_t count, float duration) noexcept {
        if (duration <= 0.0f || count < 2) return 0.0f;
        return static_cast<float>(count) / duration;
    }
    
    // Calculate synaptic weight change with bounds
    static float clampWeightChange(float currentWeight, float delta, 
                                   float minWeight = Constants::Synapse::MIN_WEIGHT,
                                   float maxWeight = Constants::Synapse::MAX_WEIGHT) noexcept {
        float newWeight = currentWeight + delta;
        return safeClamp(newWeight, minWeight, maxWeight);
    }
}

// ==================== INVALID ID SENTINELS ====================

constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

// ==================== UTILITY FUNCTIONS ====================

// Convert neuron type to descriptive string
const char* neuronTypeToString(NeuronType type) noexcept;

// Convert synapse type to descriptive string
const char* synapseTypeToString(SynapseType type) noexcept;

// Convert plasticity rule to descriptive string
const char* plasticityRuleToString(PlasticityRule rule) noexcept;

// Convert developmental stage to descriptive string
const char* developmentalStageToString(DevelopmentalStage stage) noexcept;

// Convert firing state to descriptive string
const char* firingStateToString(FiringState state) noexcept;

// Convert log level to descriptive string
const char* logLevelToString(LogLevel level) noexcept;

// Format floating point number with precision
std::string formatFloat(float value, int precision = 3) noexcept;

// Calculate distance between neurons (placeholder for spatial implementations)
static float calculateDistance(NeuronId a, NeuronId b, float maxDistance = 1000.0f) noexcept {
    // Placeholder implementation - actual implementations would use spatial coordinates
    return maxDistance * (static_cast<float>(rand()) / RAND_MAX);
}

// Check if neurons are directly connected
static bool areNeuronsConnected(NeuronId source, NeuronId target) noexcept {
    // Placeholder - actual implementation would query synapse database
    return false;
}

// Calculate synaptic delay based on distance and transmission speed
static Delay calculateSynapticDelay(float distance, float transmissionSpeed = 0.5f) noexcept {
    // distance in arbitrary units, transmission speed units per ms
    float delayMs = distance / transmissionSpeed;
    return static_cast<Delay>(std::round(delayMs));
}

} // namespace nlm

// Include the original type definitions for backward compatibility
#include "core/Types/Types.hpp"