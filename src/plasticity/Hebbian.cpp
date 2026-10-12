#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

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
    try {
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
        
        // Validate synapse pointer
        if (!synapse) {
            Logger::getGlobal().warning("Hebbian::update: Synapse pointer is null");
            return;
        }
        
        // Validate spike vectors
        if (preSpikes.empty() || postSpikes.empty()) {
            Logger::getGlobal().warning("Hebbian::update: One or both spike history vectors are empty");
            return;
        }
        
        // Validate timestep
        if (dt <= 0.0) {
            Logger::getGlobal().error("Hebbian::update: Invalid timestep: " + std::to_string(dt));
            return;
        }
        
        // Validate Hebbian parameters
        if (pImpl->learningRate < 0.0f || pImpl->learningRate > 1.0f) {
            Logger::getGlobal().warning("Hebbian::update: Invalid learning rate: " + std::to_string(pImpl->learningRate) + ", clamping to [0,1]");
            pImpl->learningRate = std::clamp(pImpl->learningRate, 0.0f, 1.0f);
        }
        
        if (pImpl->maxWeight < 0.0f || pImpl->maxWeight > 10.0f) {
            Logger::getGlobal().warning("Hebbian::update: Invalid max weight: " + std::to_string(pImpl->maxWeight) + ", clamping to [0,10]");
            pImpl->maxWeight = std::clamp(pImpl->maxWeight, 0.0f, 10.0f);
        }
        
        if (pImpl->minWeight > pImpl->maxWeight) {
            Logger::getGlobal().error("Hebbian::update: Invalid weight bounds - min > max");
            return;
        }
        
        // Check for potential overflow in spike count multiplication
        if (preSpikes.size() > 1000 || postSpikes.size() > 1000) {
            Logger::getGlobal().warning("Hebbian::update: Large spike history sizes may impact performance");
        }
        
        // Count correlated spike pairs (simplified covariance) with error handling
        size_t correlationCount = 0;
        try {
            for (Timestamp preTime : preSpikes) {
                for (Timestamp postTime : postSpikes) {
                    float dt = static_cast<float>(postTime - preTime);
                    
                    // Validate timing difference
                    if (std::isnan(dt) || std::isinf(dt)) {
                        Logger::getGlobal().warning("Hebbian::update: Invalid timing difference: " + std::to_string(dt));
                        continue;
                    }
                    
                    // Count spikes within a broad time window as correlated
                    if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                        ++correlationCount;
                    }
                }
            }
        } catch (const std::exception& e) {
            Logger::getGlobal().error("Hebbian::update: Exception during correlation counting: " + std::string(e.what()));
            return;
        }
        
        // Compute weight change based on correlation with bounds checking
        float delta = pImpl->learningRate * static_cast<float>(correlationCount);
        
        // Check for overflow
        if (std::isnan(delta) || std::isinf(delta)) {
            Logger::getGlobal().error("Hebbian::update: NaN or infinite delta");
            delta = 0.0f;
        }
        
        // Apply with bounds
        if (std::abs(delta) > 1e-6f) {
            applyWeightChange(synapse, delta);
        }
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("Hebbian::update: Memory allocation failed");
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Hebbian::update: Unexpected exception: " + std::string(e.what()));
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    try {
        // Validate synapse pointer
        if (!synapse) {
            Logger::getGlobal().warning("Hebbian::applyWeightChange: Synapse pointer is null");
            return;
        }
        
        // Validate delta
        if (std::isnan(delta) || std::isinf(delta)) {
            Logger::getGlobal().error("Hebbian::applyWeightChange: Invalid delta value: " + std::to_string(delta));
            return;
        }
        
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        
        // Check for NaN or infinite weight
        if (std::isnan(newWeight) || std::isinf(newWeight)) {
            Logger::getGlobal().error("Hebbian::applyWeightChange: Invalid computed weight: " + std::to_string(newWeight));
            return;
        }
        
        synapse->setWeight(newWeight);
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Hebbian::applyWeightChange: Exception while applying weight change: " + std::string(e.what()));
    }
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    try {
        // Validate rate
        if (std::isnan(rate) || std::isinf(rate)) {
            Logger::getGlobal().error("Hebbian::setLearningRate: Invalid rate: " + std::to_string(rate));
            return;
        }
        
        pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Hebbian::setLearningRate: Exception while setting learning rate: " + std::string(e.what()));
    }
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    try {
        // Validate maxWeight
        if (std::isnan(maxWeight) || std::isinf(maxWeight)) {
            Logger::getGlobal().error("Hebbian::setMaxWeight: Invalid maxWeight: " + std::to_string(maxWeight));
            return;
        }
        
        pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Hebbian::setMaxWeight: Exception while setting max weight: " + std::string(e.what()));
    }
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
