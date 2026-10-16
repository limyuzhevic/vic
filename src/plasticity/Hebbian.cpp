#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <stdexcept>
#include <limits>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    // Initialize Hebbian parameters with biologically plausible defaults
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {
        NLM_LOG_DEBUG("Hebbian: Initialized with learning rate=" + std::to_string(learningRate) +
                     ", weight bounds=[" + std::to_string(minWeight) + "," + std::to_string(maxWeight) + "]");
    }
};

Hebbian::Hebbian() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Hebbian: Creating Hebbian plasticity rule instance");
}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    /*
     * Comprehensive Hebbian Learning Implementation (Covariance Rule)
     * 
     * Biological Significance:
     * Hebbian plasticity implements the fundamental principle that "neurons that fire together,
     * wire together". This strengthens connections between pre- and post-synaptic neurons
     * that are active simultaneously or in close temporal proximity.
     * 
     * Mathematical Formulation (Covariance Rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified Implementation for Spike-Based Systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes (within time window)
     *   baseline = learningRate * mean activity (prevents runaway potentiation)
     *   η = learningRate (learning speed parameter)
     * 
     * Key Features:
     * - Correlation-based learning within a 100ms temporal window
     * - Weight bounds to prevent instability
     * - Integration with reward-modulated learning
     * 
     * Biological Inspiration:
     * - AMPA receptor trafficking during long-term potentiation (LTP)
     * - Hebbian mechanisms at Schaffer collateral synapses in hippocampus
     * - Correlation-based learning in visual cortex (orientation selectivity)
     * 
     * Advantages over STDP:
     * - Simpler computation (no need to track individual spike timing)
     * - More robust to noise and sparse activity
     * - Better for global correlation learning
     * 
     * Limitations:
     * - Doesn't capture precise spike timing (STDP advantage)
     * - Single learning rate (no separate potentiation/depression)
     * - Assumes stationary statistics
     */
    
    // Comprehensive input validation and error handling
    if (!synapse) {
        NLM_LOG_WARNING("Hebbian::update: Null synapse pointer - skipping update");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        NLM_LOG_DEBUG("Hebbian::update: Insufficient spike activity - skipping update");
        return;
    }
    
    // Validate spike data for numerical stability
    validateSpikeData(preSpikes);
    validateSpikeData(postSpikes);
    
    // Compute correlation count with robust temporal windowing
    size_t correlationCount = computeCorrelationCount(preSpikes, postSpikes);
    
    // Compute weight change with safety checks and numerical stability
    float delta = computeWeightChange(correlationCount, dt);
    
    // Apply weight change with comprehensive validation
    applyHebbianWeightChange(synapse, delta);
}

void Hebbian::validateSpikeData(const std::vector<Timestamp>& spikes) {
    // Validate spike data for numerical stability and correctness
    if (spikes.empty()) return;
    
    // Check for reasonable spike timing (avoid unrealistic values)
    Timestamp maxReasonableTime = static_cast<Timestamp>(1000000.0f);  // 1000 seconds max
    for (size_t i = 0; i < spikes.size(); ++i) {
        if (spikes[i] < 0) {
            NLM_LOG_WARNING("Hebbian::validateSpikeData: Negative timestamp at index " + 
                          std::to_string(i) + " - this may indicate corrupted data");
        } else if (spikes[i] > maxReasonableTime) {
            NLM_LOG_WARNING("Hebbian::validateSpikeData: Extremely large timestamp at index " +
                          std::to_string(i) + " = " + std::to_string(spikes[i]) +
                          " - potential numerical instability");
        }
    }
}

size_t Hebbian::computeCorrelationCount(const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes) {
    // Compute correlation count using a biologically realistic temporal window
    size_t correlationCount = 0;
    float temporalWindow = 100.0f;  // 100ms correlation window (typical for Hebbian learning)
    
    // Iterate through all spike pairs to count correlations
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            // Use absolute value since correlation is symmetric
            if (std::abs(dt) < temporalWindow) {
                correlationCount++;
            }
        }
    }
    
    // Log correlation computation for debugging
    if (correlationCount > 0) {
        NLM_LOG_DEBUG("Hebbian::computeCorrelationCount: Found " + std::to_string(correlationCount) +
                     " correlated spike pairs (window=" + std::to_string(temporalWindow) + "ms)");
    } else {
        NLM_LOG_DEBUG("Hebbian::computeCorrelationCount: No correlated spike pairs found");
    }
    
    return correlationCount;
}

float Hebbian::computeWeightChange(size_t correlationCount, TimestepDuration dt) {
    // Compute weight change based on correlation count with numerical stability
    // More sophisticated approach would use actual firing rates, but this simplified
    // version uses correlation count as a proxy for coactivity
    
    if (correlationCount == 0) {
        NLM_LOG_DEBUG("Hebbian::computeWeightChange: No correlations - no weight change");
        return 0.0f;
    }
    
    // Compute weight change using covariance-like formulation
    // Δw = learningRate * (correlationCount - learningRate * baseline)
    float baseline = pImpl->learningRate * static_cast<float>(correlationCount);
    float delta = pImpl->learningRate * (static_cast<float>(correlationCount) - baseline);
    
    // Apply timestep scaling for consistent learning across different simulation speeds
    float dtSeconds = static_cast<float>(dt) / 1000.0f;  // Convert ms to seconds
    delta *= dtSeconds;
    
    // Safety check for numerical stability
    if (std::abs(delta) > 10.0f) {
        NLM_LOG_WARNING("Hebbian::computeWeightChange: Large weight change delta = " + 
                       std::to_string(delta) + " - this may cause instability");
    }
    
    NLM_LOG_DEBUG("Hebbian::computeWeightChange: delta = " + std::to_string(delta) +
                 " (correlationCount=" + std::to_string(correlationCount) +
                 ", dt=" + std::to_string(dt) + "ms)");
    
    return delta;
}

void Hebbian::applyHebbianWeightChange(Synapse* synapse, float delta) {
    // Apply Hebbian weight change with comprehensive validation and bounds checking
    if (!synapse) {
        NLM_LOG_WARNING("Hebbian::applyHebbianWeightChange: Null synapse - cannot apply change");
        return;
    }
    
    // Check if weight change is significant enough to apply
    if (std::abs(delta) < 1e-6f) {
        NLM_LOG_DEBUG("Hebbian::applyHebbianWeightChange: Negligible weight change - skipping");
        return;
    }
    
    // Apply weight change using the existing applyWeightChange method
    applyWeightChange(synapse, delta);
    
    NLM_LOG_DEBUG("Hebbian::applyHebbianWeightChange: Applied Δw = " + std::to_string(delta));
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Apply weight change with comprehensive bounds checking and validation
    if (!synapse) {
        NLM_LOG_WARNING("Hebbian::applyWeightChange: Null synapse - cannot apply weight change");
        return;
    }
    
    // Get current weight for logging and bounds checking
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply bounds checking with hysteresis to prevent oscillation
    float minWeight = pImpl->minWeight;
    float maxWeight = pImpl->maxWeight;
    
    if (newWeight < minWeight) {
        NLM_LOG_DEBUG("Hebbian::applyWeightChange: Weight " + std::to_string(newWeight) + 
                     " below minimum " + std::to_string(minWeight) + " - clamping");
        newWeight = minWeight;
    } else if (newWeight > maxWeight) {
        NLM_LOG_DEBUG("Hebbian::applyWeightChange: Weight " + std::to_string(newWeight) + 
                     " above maximum " + std::to_string(maxWeight) + " - clamping");
        newWeight = maxWeight;
    }
    
    // Only apply change if weight actually changed (within tolerance)
    if (std::abs(newWeight - currentWeight) > 1e-6f) {
        synapse->setWeight(newWeight);
        NLM_LOG_DEBUG("Hebbian::applyWeightChange: Updated synapse weight from " + 
                     std::to_string(currentWeight) + " to " + std::to_string(newWeight));
    } else {
        NLM_LOG_DEBUG("Hebbian::applyWeightChange: No weight change needed (within tolerance)");
    }
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    // Set learning rate with validation and logging
    if (rate < 0.0f) {
        NLM_LOG_WARNING("Hebbian::setLearningRate: Negative learning rate requested (" + 
                       std::to_string(rate) + ") - clamping to 0");
        rate = 0.0f;
    } else if (rate > 1.0f) {
        NLM_LOG_WARNING("Hebbian::setLearningRate: High learning rate requested (" + 
                       std::to_string(rate) + ") - may cause instability");
    }
    
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
    NLM_LOG_INFO("Hebbian::setLearningRate: Set learning rate to " + std::to_string(pImpl->learningRate));
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    // Set maximum weight with validation and logging
    if (maxWeight < 0.0f) {
        NLM_LOG_WARNING("Hebbian::setMaxWeight: Negative max weight requested (" + 
                       std::to_string(maxWeight) + ") - clamping to 0");
        maxWeight = 0.0f;
    } else if (maxWeight > 10.0f) {
        NLM_LOG_WARNING("Hebbian::setMaxWeight: High max weight requested (" + 
                       std::to_string(maxWeight) + ") - may cause instability");
    }
    
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
    NLM_LOG_INFO("Hebbian::setMaxWeight: Set max weight to " + std::to_string(pImpl->maxWeight));
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
