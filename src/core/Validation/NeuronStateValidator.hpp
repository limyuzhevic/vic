#pragma once

/**
 * Neuron State Validator
 * 
 * Comprehensive validation of individual neuron states for NLM brain simulation.
 * Validates membrane potential, refractory state, spike state, synaptic state,
 * and connection state consistency.
 * 
 * Features:
 * - Membrane potential validation with threshold and bounds checking
 * - Refractory state validation
 * - Spike state validation
 * - Synaptic state validation
 * - Connection state validation
 * - Real-time validation during simulation
 * - Thread-safe validation operations
 */

#include "Neuron.hpp"
#include "../core/Logger/Logger.hpp"
#include <mutex>
#include <vector>
#include <string>

namespace nlm {
namespace validation {

/**
 * Neuron validation result
 */
struct NeuronValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    size_t neuronsValidated;
    
    NeuronValidationResult()
        : isValid(true), validationTime(0.0), neuronsValidated(0) {}
};

/**
 * Neuron state validator
 */
class NeuronStateValidator {
public:
    NeuronStateValidator();
    ~NeuronStateValidator();
    
    // Disable copying, enable moving
    NeuronStateValidator(const NeuronStateValidator&) = delete;
    NeuronStateValidator& operator=(const NeuronStateValidator&) = delete;
    NeuronStateValidator(NeuronStateValidator&&) noexcept = default;
    NeuronStateValidator& operator=(NeuronStateValidator&&) noexcept = default;
    
    // Core validation operations
    NeuronValidationResult validate(const Neuron& neuron) const;
    NeuronValidationResult validate(const std::vector<const Neuron*>& neurons) const;
    
    // Configuration
    void setMembranePotentialBounds(double min, double max) {
        membranePotentialMin_ = min;
        membranePotentialMax_ = max;
    }
    
    void setRefractoryPeriodValidationEnabled(bool enabled) { 
        validateRefractoryPeriod_ = enabled; 
    }
    
    void setSpikeStateValidationEnabled(bool enabled) { 
        validateSpikeState_ = enabled; 
    }
    
    void setSynapticStateValidationEnabled(bool enabled) { 
        validateSynapticState_ = enabled; 
    }
    
    void setConnectionStateValidationEnabled(bool enabled) { 
        validateConnectionState_ = enabled; 
    }
    
    // Validation statistics
    size_t getTotalValidations() const { return totalValidations_; }
    double getAverageValidationTime() const { return totalValidationTime_ / totalValidations_; }
    size_t getErrorCount() const { return totalErrors_; }
    size_t getWarningCount() const { return totalWarnings_; }
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Debug and diagnostics
    std::string dump() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    
    double membranePotentialMin_ = -100.0;  // mV
    double membranePotentialMax_ = 50.0;   // mV
    bool validateRefractoryPeriod_ = true;
    bool validateSpikeState_ = true;
    bool validateSynapticState_ = true;
    bool validateConnectionState_ = true;
    
    size_t totalValidations_ = 0;
    double totalValidationTime_ = 0.0;
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    
    // Internal validation helpers
    NeuronValidationResult validateMembranePotential(const NeuronState& state) const;
    NeuronValidationResult validateRefractoryState(const NeuronState& state) const;
    NeuronValidationResult validateSpikeState(const NeuronState& state) const;
    NeuronValidationResult validateSynapticState(const Neuron& neuron) const;
    NeuronValidationResult validateConnectionState(const Neuron& neuron) const;
};

} // namespace validation
} // namespace nlm
