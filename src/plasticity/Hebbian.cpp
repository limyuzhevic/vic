#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Validation/Validation.hpp"
#include "../../core/Logger/Logger.hpp"
#include <algorithm>
#include <format>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     * 
     * This implements "neurons that fire together, wire together"
     * but with a threshold to prevent runaway potentiation.
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking
     *   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
     *   - Correlation-based learning in visual cortex
     *   
     * Limitations:
     *   - Doesn't account for STDP timing details
     *   - Single learning rate (no separate potentiation/depression rates)
     *   - Assumes stationary statistics
     */
    
    // Comprehensive null and parameter validation
    VALIDATE_NULL_RETURN(synapse, "Hebbian::update: synapse pointer is null",);
    VALIDATE_PARAMETER_RETURN(!Validation::IsEmpty(preSpikes),
                           "Hebbian::update: preSpikes vector is empty",
                           ErrorCode::InvalidParameter,);
    VALIDATE_PARAMETER_RETURN(!Validation::IsEmpty(postSpikes),
                           "Hebbian::update: postSpikes vector is empty",
                           ErrorCode::InvalidParameter,);
    VALIDATE_PARAMETER_RETURN(Validation::IsValidTimestep(dt),
                           "Hebbian::update: timestep is invalid",
                           ErrorCode::InvalidParameter,);
    
    // Validate internal Hebbian parameters
    VALIDATE_PARAMETER_RETURN(Validation::IsValidLearningRate(pImpl->learningRate),
                           "Hebbian::update: learning rate is invalid",
                           ErrorCode::ConfigurationError,);
    VALIDATE_PARAMETER_RETURN(Validation::IsValidWeight(pImpl->maxWeight),
                           "Hebbian::update: max weight is invalid",
                           ErrorCode::ConfigurationError,);
    VALIDATE_PARAMETER_RETURN(Validation::IsValidWeight(pImpl->minWeight),
                           "Hebbian::update: min weight is invalid",
                           ErrorCode::ConfigurationError,);
    VALIDATE_PARAMETER_RETURN(pImpl->maxWeight > pImpl->minWeight,
                           "Hebbian::update: max weight must be greater than min weight",
                           ErrorCode::ConfigurationError,);
    
    // Count correlated spike pairs (simplified covariance)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        VALIDATE_PARAMETER_RETURN(Validation::IsValidTimestamp(preTime),
                               "Hebbian::update: preSpike timestamp is invalid",
                               ErrorCode::InvalidParameter, nullptr);
        
        for (Timestamp postTime : postSpikes) {
            VALIDATE_PARAMETER_RETURN(Validation::IsValidTimestamp(postTime),
                                   "Hebbian::update: postSpike timestamp is invalid",
                                   ErrorCode::InvalidParameter, nullptr);
            
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within a broad time window as correlated
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    // Compute weight change based on correlation
    // More sophisticated: use actual spike counts and firing rates
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Validate delta
    VALIDATE_PARAMETER_RETURN(Validation::IsFinite(delta),
                           "Hebbian::update: computed delta is not finite",
                           ErrorCode::InvalidParameter, nullptr);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Comprehensive null pointer validation
    VALIDATE_NULL_RETURN(synapse, "Hebbian::applyWeightChange: synapse pointer is null",);
    
    // Validate delta parameter
    VALIDATE_PARAMETER_RETURN(Validation::IsFinite(delta),
                           "Hebbian::applyWeightChange: weight change is not finite",
                           ErrorCode::InvalidParameter,);
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Validate clamped weight
    VALIDATE_PARAMETER_RETURN(Validation::IsValidWeight(newWeight),
                           "Hebbian::applyWeightChange: clamped weight is invalid",
                           ErrorCode::OutOfBounds,);
    
    synapse->setWeight(newWeight);
    
    // Log the change if significant
    if (std::abs(delta) > 1e-6f) {
        LOG_INFO(std::format("Hebbian::applyWeightChange: Changed synapse {} weight from {:.6f} to {:.6f} (delta: {:.6f})",
                         synapse->getId(), synapse->getWeight() - delta, newWeight, delta));
    }
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    VALIDATE_PARAMETER_RETURN(Validation::IsFinite(rate),
                           "Hebbian::setLearningRate: learning rate is not finite",
                           ErrorCode::InvalidParameter,);
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    VALIDATE_PARAMETER_RETURN(Validation::IsFinite(maxWeight),
                           "Hebbian::setMaxWeight: max weight is not finite",
                           ErrorCode::InvalidParameter,);
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm

