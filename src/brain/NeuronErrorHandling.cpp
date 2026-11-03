#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "Neuron.hpp"
#include <stdexcept>
#include <limits>

namespace nlm {

class NeuronValidationError : public std::runtime_error {
public:
    NeuronValidationError(const std::string& message) : std::runtime_error(message) {}
};

class SynapseValidationError : public std::runtime_error {
public:
    SynapseValidationError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

// Enhanced Neuron validation methods

void Neuron::validate() const {
    if (!pImpl) {
        throw NeuronValidationError("Neuron: Null implementation pointer");
    }
    
    const auto& state = pImpl->state;
    
    // Validate membrane potential range (reasonable biological bounds)
    if (state.membranePotential < -100.0f || state.membranePotential > 50.0f) {
        NLM_LOG_WARNING("Neuron " + std::to_string(pImpl->id.get()) + 
                       ": Unusual membrane potential " + 
                       std::to_string(state.membranePotential) + " mV");
    }
    
    // Validate threshold consistency
    if (state.threshold <= state.restingPotential) {
        throw NeuronValidationError("Neuron " + std::to_string(pImpl->id.get()) + 
                                   ": Threshold must be above resting potential");
    }
    
    // Validate firing rate
    if (state.firingRate < 0.0f || state.firingRate > 1000.0f) {
        NLM_LOG_WARNING("Neuron " + std::to_string(pImpl->id.get()) + 
                       ": Unusual firing rate " + 
                       std::to_string(state.firingRate) + " Hz");
    }
    
    // Validate spike history
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        throw NeuronValidationError("Neuron " + std::to_string(pImpl->id.get()) + 
                                   ": Spike history exceeds maximum size");
    }
}

void Neuron::validateInputParameters(float current, const std::string& operation) const {
    if (std::isnan(current)) {
        throw NeuronValidationError("Neuron " + std::to_string(pImpl->id.get()) + 
                                   ": " + operation + " - NaN current value");
    }
    
    if (std::isinf(current)) {
        throw NeuronValidationError("Neuron " + std::to_string(pImpl->id.get()) + 
                                   ": " + operation + " - Infinite current value");
    }
    
    const float maxReasonableCurrent = 1000.0f; // mV - very high but biologically plausible
    if (std::abs(current) > maxReasonableCurrent) {
        NLM_LOG_WARNING("Neuron " + std::to_string(pImpl->id.get()) + 
                       ": " + operation + 
                       " - Extremely large current " + std::to_string(current) + " mV");
    }
}

void Neuron::safeInjectCurrent(MembranePotential current) {
    validateInputParameters(current, "injectCurrent");
    injectCurrent(current);
    
    // Update total current for tracking
    pImpl->totalCurrent += current;
    
    // Clamp total current to prevent runaway
    if (pImpl->totalCurrent > 10000.0f) pImpl->totalCurrent = 10000.0f;
    if (pImpl->totalCurrent < -10000.0f) pImpl->totalCurrent = -10000.0f;
}

void Neuron::stepLIFProtected(Timestamp currentTime, TimestepDuration dt) {
    // Validate parameters
    if (dt <= 0.0) {
        throw NeuronValidationError("Neuron " + std::to_string(pImpl->id.get()) + 
                                   ": Invalid timestep " + std::to_string(dt));
    }
    
    if (std::isnan(dt) || std::isinf(dt)) {
        throw NeuronValidationError("Neuron " + std::to_string(pImpl->id.get()) + 
                                   ": Invalid timestep " + std::to_string(dt));
    }
    
    // Perform the step
    bool fired = stepLIF(currentTime, dt);
    
    // Post-step validation
    validate();
    
    if (fired) {
        NLM_LOG_DEBUG("Neuron " + std::to_string(pImpl->id.get()) + 
                     " fired at time " + std::to_string(currentTime));
    }
}

} // namespace nlm
