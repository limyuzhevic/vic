#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <cassert>
#include <limits>

namespace nlm {

// Validation level enumeration
enum class ValidationLevel {
    None = 0,
    Warning = 1,
    Strict = 2
};

// Validation macros
#define VALIDATE_LE_ASSERT(expr, msg) assert(expr && msg)
#define VALIDATE_LE_WARNING(expr, msg) if (!(expr)) { NLM_LOG_WARNING("Validation failed: " << msg); }
#define VALIDATE_LE_STRICT(expr, msg) if (!(expr)) { NLM_LOG_ERROR("Validation failed: " << msg); assert(false && msg); }

// Validation helper functions
class ValidationHelper {
public:
    static constexpr float MEMBRANE_POTENTIAL_MIN = -100.0f;  // mV
    static constexpr float MEMBRANE_POTENTIAL_MAX = 50.0f;   // mV
    static constexpr float ADAPTATION_VARIABLE_MIN = -100.0f; // Typical range
    static constexpr float ADAPTATION_VARIABLE_MAX = 100.0f;  // Typical range
    static constexpr uint32_t REFRACTORY_PERIOD_MIN = 0;     // No refractory
    static constexpr uint32_t REFRACTORY_PERIOD_MAX = 1000;  // Max 1000 steps (~10s at 1ms timestep)
    static constexpr float LEAK_CONDUCTANCE_MIN = 0.0f;       // Can't be negative
    static constexpr float LEAK_CONDUCTANCE_MAX = 100.0f;     // Upper limit
    
    // Validate membrane potential bounds
    static bool validateMembranePotential(MembranePotential potential, ValidationLevel level, const char* context) {
        bool valid = (potential >= MEMBRANE_POTENTIAL_MIN && potential <= MEMBRANE_POTENTIAL_MAX);
        if (!valid) {
            std::string msg = std::string("Membrane potential out of bounds: ") + std::to_string(potential) +
                             " mV (valid range: [" + std::to_string(MEMBRANE_POTENTIAL_MIN) +
                             ", " + std::to_string(MEMBRANE_POTENTIAL_MAX) + "] mV) in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
        }
        return valid;
    }
    
    // Validate threshold > resting potential
    static bool validateThresholdVsResting(MembranePotential threshold, MembranePotential resting, ValidationLevel level, const char* context) {
        bool valid = (threshold > resting);
        if (!valid) {
            std::string msg = std::string("Threshold (" + std::to_string(threshold) + " mV) must be greater than resting potential (" +
                                         std::to_string(resting) + " mV) in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
        }
        return valid;
    }
    
    // Validate refractory period
    static bool validateRefractoryPeriod(uint32_t period, ValidationLevel level, const char* context) {
        bool valid = (period >= REFRACTORY_PERIOD_MIN && period <= REFRACTORY_PERIOD_MAX);
        if (!valid) {
            std::string msg = std::string("Refractory period out of bounds: ") + std::to_string(period) +
                             " steps (valid range: [" + std::to_string(REFRACTORY_PERIOD_MIN) +
                             ", " + std::to_string(REFRACTORY_PERIOD_MAX) + "] steps) in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
        }
        return valid;
    }
    
    // Validate adaptation variable bounds
    static bool validateAdaptationVariable(float adaptation, ValidationLevel level, const char* context) {
        bool valid = (adaptation >= ADAPTATION_VARIABLE_MIN && adaptation <= ADAPTATION_VARIABLE_MAX);
        if (!valid) {
            std::string msg = std::string("Adaptation variable out of bounds: ") + std::to_string(adaptation) +
                             " (valid range: [" + std::to_string(ADAPTATION_VARIABLE_MIN) +
                             ", " + std::to_string(ADAPTATION_VARIABLE_MAX) + "])" +
                             " in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
        }
        return valid;
    }
    
    // Validate leak conductance
    static bool validateLeakConductance(float conductance, ValidationLevel level, const char* context) {
        bool valid = (conductance >= LEAK_CONDUCTANCE_MIN && conductance <= LEAK_CONDUCTANCE_MAX);
        if (!valid) {
            std::string msg = std::string("Leak conductance out of bounds: ") + std::to_string(conductance) +
                             " nS (valid range: [" + std::to_string(LEAK_CONDUCTANCE_MIN) +
                             ", " + std::to_string(LEAK_CONDUCTANCE_MAX) + "] nS)" +
                             " in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
        }
        return valid;
    }
    
    // Validate neuron ID
    static bool validateNeuronId(NeuronId id, ValidationLevel level, const char* context) {
        bool valid = (id.value < UINT64_MAX); // Not the invalid ID
        if (!valid) {
            std::string msg = std::string("Invalid neuron ID in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
        }
        return valid;
    }
    
    // Compile-time validation for constants
    static constexpr void compileTimeValidation() {
        static_assert(MEMBRANE_POTENTIAL_MIN < MEMBRANE_POTENTIAL_MAX, "Membrane potential bounds invalid");
        static_assert(ADAPTATION_VARIABLE_MIN < ADAPTATION_VARIABLE_MAX, "Adaptation variable bounds invalid");
        static_assert(REFRACTORY_PERIOD_MIN <= REFRACTORY_PERIOD_MAX, "Refractory period bounds invalid");
        static_assert(LEAK_CONDUCTANCE_MIN <= LEAK_CONDUCTANCE_MAX, "Leak conductance bounds invalid");
    }

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics with validation
class Neuron {
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Validation configuration
    void setValidationLevel(ValidationLevel level);
    ValidationLevel getValidationLevel() const;
    
    // Validation helper methods
    bool validateMembranePotential(MembranePotential potential) const;
    bool validateThreshold(MembranePotential threshold) const;
    bool validateRefractoryPeriod(uint32_t period) const;
    bool validateAdaptationVariable(float adaptation) const;
    bool validateLeakConductance(float conductance) const;
    bool validateNeuronId() const;
    
    // Validate entire neuron state (constant time check)
    bool validateState() const;
    
    // Identity
    NeuronId getId() const;
    
    // Type
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State access
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane potential
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
