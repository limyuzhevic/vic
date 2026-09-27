#include "NeuronStateValidator.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

namespace nlm {
namespace validation {

class NeuronStateValidator::Impl {
public:
    double membranePotentialMin_ = -100.0;
    double membranePotentialMax_ = 50.0;
    bool validateRefractoryPeriod_ = true;
    bool validateSpikeState_ = true;
    bool validateSynapticState_ = true;
    bool validateConnectionState_ = true;
    size_t totalValidations = 0;
    double totalValidationTime = 0.0;
    size_t totalErrors = 0;
    size_t totalWarnings = 0;
    mutable std::mutex mutex;
};

NeuronStateValidator::NeuronStateValidator() : pImpl(std::make_unique<Impl>()) {}
NeuronStateValidator::~NeuronStateValidator() = default;

NeuronValidationResult NeuronStateValidator::validate(const Neuron& neuron) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NeuronValidationResult result;
    const auto& state = neuron.getState();
    
    // Validate membrane potential
    if (state.membranePotential < pImpl->membranePotentialMin_) {
        result.errors.push_back("Membrane potential below minimum bound");
        result.isValid = false;
    }
    else if (state.membranePotential > pImpl->membranePotentialMax_) {
        result.errors.push_back("Membrane potential above maximum bound");
        result.isValid = false;
    }
    
    // Validate threshold consistency
    if (state.threshold <= state.restingPotential) {
        result.errors.push_back("Threshold must be above resting potential");
        result.isValid = false;
    }
    
    // Validate refractory state
    if (pImpl->validateRefractoryPeriod_) {
        if (state.refractoryRemaining > state.refractoryPeriod) {
            result.errors.push_back("Refractory remaining exceeds refractory period");
            result.isValid = false;
        }
        
        if (state.firingState == FiringState::Refractory && state.refractoryRemaining == 0) {
            result.errors.push_back("Neuron in refractory state but refractory remaining is zero");
            result.isValid = false;
        }
    }
    
    // Validate spike state
    if (pImpl->validateSpikeState_) {
        bool hasSpike = (state.firingState == FiringState::Active) || (state.firingState == FiringState::Refractory);
        bool spikeHistoryConsistent = (state.lastSpikeTime >= 0.0f) == hasSpike;
        
        if (hasSpike && state.lastSpikeTime < 0.0f) {
            result.errors.push_back("Neurons in firing state should have valid spike time");
            result.isValid = false;
        }
        
        if (!hasSpike && state.lastSpikeTime >= 0.0f) {
            result.errors.push_back("Neurons not firing should have invalid spike time");
            result.isValid = false;
        }
    }
    
    // Validate synaptic state
    if (pImpl->validateSynapticState_) {
        // Basic synaptic input validation - would be more detailed in actual implementation
        result.warnings.push_back("Synaptic state validation placeholder");
    }
    
    // Validate connection state
    if (pImpl->validateConnectionState_) {
        // This would check neuron connectivity in actual implementation
        result.warnings.push_back("Connection state validation placeholder");
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    result.validationTime = duration.count() / 1000.0;
    result.neuronsValidated = 1;
    
    pImpl->totalValidations++;
    pImpl->totalValidationTime += result.validationTime;
    pImpl->totalErrors += result.errors.size();
    pImpl->totalWarnings += result.warnings.size();
    
    return result;
}

NeuronValidationResult NeuronStateValidator::validate(const std::vector<const Neuron*>& neurons) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NeuronValidationResult result;
    
    for (const auto* neuron : neurons) {
        if (!neuron) {
            result.errors.push_back("Null neuron pointer in validation batch");
            result.isValid = false;
            continue;
        }
        
        auto neuronResult = validate(*neuron);
        result.errors.insert(result.errors.end(), 
                           std::make_move_iterator(neuronResult.errors.begin()),
                           std::make_move_iterator(neuronResult.errors.end()));
        result.warnings.insert(result.warnings.end(),
                             std::make_move_iterator(neuronResult.warnings.begin()),
                             std::make_move_iterator(neuronResult.warnings.end()));
    }
    
    result.neuronsValidated = neurons.size();
    
    // Summary statistics
    if (result.neuronsValidated > 0) {
        double errorRate = static_cast<double>(result.errors.size()) / result.neuronsValidated;
        if (errorRate > 0.5) {
            result.warnings.push_back("High error rate in neuron batch validation");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    result.validationTime = duration.count() / 1000.0;
    
    pImpl->totalValidations++;
    pImpl->totalValidationTime += result.validationTime;
    pImpl->totalErrors += result.errors.size();
    pImpl->totalWarnings += result.warnings.size();
    
    return result;
}

void NeuronStateValidator::lock() const {
    pImpl->mutex.lock();
}

void NeuronStateValidator::unlock() const {
    pImpl->mutex.unlock();
}

bool NeuronStateValidator::tryLock() const {
    return pImpl->mutex.try_lock();
}

std::string NeuronStateValidator::dump() const {
    std::stringstream ss;
    ss << "NeuronStateValidator:" << std::endl;
    ss << "  Total validations: " << pImpl->totalValidations << std::endl;
    ss << "  Average validation time: " << (pImpl->totalValidations > 0 ? 
        pImpl->totalValidationTime / pImpl->totalValidations : 0.0) << "s" << std::endl;
    ss << "  Errors: " << pImpl->totalErrors << std::endl;
    ss << "  Warnings: " << pImpl->totalWarnings << std::endl;
    ss << "  Membrane potential bounds: [" << pImpl->membranePotentialMin_ << ", " << pImpl->membranePotentialMax_ << "] mV" << std::endl;
    ss << "  Refractory period validation: " << (pImpl->validateRefractoryPeriod_ ? "enabled" : "disabled") << std::endl;
    ss << "  Spike state validation: " << (pImpl->validateSpikeState_ ? "enabled" : "disabled") << std::endl;
    ss << "  Synaptic state validation: " << (pImpl->validateSynapticState_ ? "enabled" : "disabled") << std::endl;
    ss << "  Connection state validation: " << (pImpl->validateConnectionState_ ? "enabled" : "disabled") << std::endl;
    return ss.str();
}

} // namespace validation
} // namespace nlm
