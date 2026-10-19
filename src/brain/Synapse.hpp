#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <cassert>
#include <limits>
#include <string>

namespace nlm {

// Validation level enumeration (reusing from Neuron)
enum class ValidationLevel {
    None = 0,
    Warning = 1,
    Strict = 2
};

// Validation helper functions (reusing from Neuron)
class ValidationHelper {
public:
    static constexpr float SYNAPTIC_WEIGHT_MIN = -1.0f;
    static constexpr float SYNAPTIC_WEIGHT_MAX = 1.0f;
    static constexpr Delay DELAY_MIN = 0;  // Zero delay possible
    static constexpr Delay DELAY_MAX = 100; // Max 100ms delay (100 steps at 1ms timestep)
    static constexpr float EFFICACY_MIN = 0.0f;
    static constexpr float EFFICACY_MAX = 2.0f;
    static constexpr float SHORT_TERM_DEPRESSION_MIN = 0.0f;
    static constexpr float SHORT_TERM_DEPRESSION_MAX = 1.0f;
    static constexpr float SHORT_TERM_FACILITATION_MIN = 0.0f;
    static constexpr float SHORT_TERM_FACILITATION_MAX = 2.0f;
    
    // Validate synaptic weight bounds
    static bool validateWeight(SynapticWeight weight, ValidationLevel level, const char* context) {
        bool valid = (weight >= SYNAPTIC_WEIGHT_MIN && weight <= SYNAPTIC_WEIGHT_MAX);
        if (!valid) {
            std::string msg = std::string("Synaptic weight out of bounds: ") + std::to_string(weight) +
                             " (valid range: [" + std::to_string(SYNAPTIC_WEIGHT_MIN) +
                             ", " + std::to_string(SYNAPTIC_WEIGHT_MAX) + "])" +
                             " in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
        }
        return valid;
    }
    
    // Validate delay bounds
    static bool validateDelay(Delay delay, ValidationLevel level, const char* context) {
        bool valid = (delay >= DELAY_MIN && delay <= DELAY_MAX);
        if (!valid) {
            std::string msg = std::string("Synaptic delay out of bounds: ") + std::to_string(delay) +
                             " steps (valid range: [" + std::to_string(DELAY_MIN) +
                             ", " + std::to_string(DELAY_MAX) + " steps])" +
                             " in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
        }
        return valid;
    }
    
    // Validate efficacy bounds
    static bool validateEfficacy(float efficacy, ValidationLevel level, const char* context) {
        bool valid = (efficacy >= EFFICACY_MIN && efficacy <= EFFICACY_MAX);
        if (!valid) {
            std::string msg = std::string("Synaptic efficacy out of bounds: ") + std::to_string(efficacy) +
                             " (valid range: [" + std::to_string(EFFICACY_MIN) +
                             ", " + std::to_string(EFFICACY_MAX) + "])" +
                             " in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
        }
        return valid;
    }
    
    // Validate short-term plasticity parameters
    static bool validateShortTermDepression(float depression, ValidationLevel level, const char* context) {
        bool valid = (depression >= SHORT_TERM_DEPRESSION_MIN && depression <= SHORT_TERM_DEPRESSION_MAX);
        if (!valid) {
            std::string msg = std::string("Short-term depression out of bounds: ") + std::to_string(depression) +
                             " (valid range: [" + std::to_string(SHORT_TERM_DEPRESSION_MIN) +
                             ", " + std::to_string(SHORT_TERM_DEPRESSION_MAX) + "])" +
                             " in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
        }
        return valid;
    }
    
    // Validate short-term facilitation parameters
    static bool validateShortTermFacilitation(float facilitation, ValidationLevel level, const char* context) {
        bool valid = (facilitation >= SHORT_TERM_FACILITATION_MIN && facilitation <= SHORT_TERM_FACILITATION_MAX);
        if (!valid) {
            std::string msg = std::string("Short-term facilitation out of bounds: ") + std::to_string(facilitation) +
                             " (valid range: [" + std::to_string(SHORT_TERM_FACILITATION_MIN) +
                             ", " + std::to_string(SHORT_TERM_FACILITATION_MAX) + "])" +
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
    
    // Validate synapse ID
    static bool validateSynapseId(SynapseId id, ValidationLevel level, const char* context) {
        bool valid = (id.value < UINT64_MAX); // Not the invalid ID
        if (!valid) {
            std::string msg = std::string("Invalid synapse ID in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
        }
        return valid;
    }
    
    // Validate source and destination neurons
    static bool validateConnection(NeuronId source, NeuronId dest, ValidationLevel level, const char* context) {
        bool valid = true;
        
        if (source.value == UINT64_MAX) {
            std::string msg = std::string("Invalid source neuron ID in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
            valid = false;
        }
        
        if (dest.value == UINT64_MAX) {
            std::string msg = std::string("Invalid destination neuron ID in " + std::string(context);
            switch (level) {
                case ValidationLevel::None: break;
                case ValidationLevel::Warning: NLM_LOG_WARNING(msg); break;
                case ValidationLevel::Strict: NLM_LOG_ERROR(msg); assert(false && msg.c_str()); break;
            }
            valid = false;
        }
        
        if (source.value == dest.value && level != ValidationLevel::None) {
            NLM_LOG_WARNING("Source and destination neurons are the same in " + std::string(context));
        }
        
        return valid;
    }
    
    // Compile-time validation for constants
    static constexpr void compileTimeValidation() {
        static_assert(SYNAPTIC_WEIGHT_MIN < SYNAPTIC_WEIGHT_MAX, "Synaptic weight bounds invalid");
        static_assert(DELAY_MIN <= DELAY_MAX, "Delay bounds invalid");
        static_assert(EFFICACY_MIN <= EFFICACY_MAX, "Efficacy bounds invalid");
        static_assert(SHORT_TERM_DEPRESSION_MIN <= SHORT_TERM_DEPRESSION_MAX, "Short-term depression bounds invalid");
        static_assert(SHORT_TERM_FACILITATION_MIN <= SHORT_TERM_FACILITATION_MAX, "Short-term facilitation bounds invalid");
    }
};

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Validation configuration
    void setValidationLevel(ValidationLevel level);
    ValidationLevel getValidationLevel() const;
    
    // Validation helper methods
    bool validateWeight(SynapticWeight weight) const;
    bool validateDelay(Delay delay) const;
    bool validateEfficacy(float efficacy) const;
    bool validateShortTermDepression(float depression) const;
    bool validateShortTermFacilitation(float facilitation) const;
    bool validateNeuronId(NeuronId id) const;
    bool validateSynapseId() const;
    bool validateConnection() const;
    bool validateState() const;
    
    // Identity
    SynapseId getId() const;
    
    // Connection
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType() const;
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement real synaptic dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
