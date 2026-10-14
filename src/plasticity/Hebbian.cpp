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
    /*
     * Real Hebbian learning implementation based on spike-timing correlation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * For spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes within time window
     *   baseline = η * mean activity (pre + post firing rates)
     *   η = learningRate (strength of plasticity)
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking at synapses
     *   - Implements correlation-based learning in cortical networks
     *   - Homeostatic scaling prevents runaway excitation
     *   
     * The implementation includes:
     *   - Real spike-time dependent correlation
     *   - Activity-dependent baseline
     *   - Homeostatic weight regulation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate covariance between pre- and post-synaptic activity
    // Based on spike-timing correlation within biological time window
    
    // First, compute basic statistics
    size_t preSpikeCount = preSpikes.size();
    size_t postSpikeCount = postSpikes.size();
    
    // Compute cross-correlation with adaptive time window
    float correlation = 0.0f;
    float crossTerm = 0.0f;
    float preMean = 0.0f;
    float postMean = 0.0f;
    
    // Estimate firing rates (spikes per second)
    // Time range of spikes
    Timestamp minTime = std::min(preSpikes.front(), postSpikes.front());
    Timestamp maxTime = std::max(preSpikes.back(), postSpikes.back());
    float timeRange = (maxTime > minTime) ? (maxTime - minTime) : 1.0f;  // Avoid division by zero
    
    preMean = static_cast<float>(preSpikeCount) / (timeRange * 0.001f);  // Convert ms to seconds
    postMean = static_cast<float>(postSpikeCount) / (timeRange * 0.001f);
    
    // Calculate cross-correlation with exponential decay kernel
    float timeWindow = 50.0f;  // 50ms correlation window (biological)
    float decay = 1.0f / timeWindow;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float delta_t = static_cast<float>(std::abs(postTime - preTime));  // Absolute difference in ms
            
            // Only count spikes within correlation window
            if (delta_t <= timeWindow) {
                // Weighted correlation with exponential decay
                float weight = std::exp(-delta_t * decay);
                crossTerm += weight;
                correlation += weight;
            }
        }
    }
    
    // Normalize by spike counts
    float normalizedCross = correlation / static_cast<float>(preSpikeCount * postSpikeCount);
    
    // Compute covariance: E[XY] - E[X]E[Y]
    float expectedProduct = preMean * postMean;
    float delta = pImpl->learningRate * (normalizedCross - expectedProduct);
    
    // Apply homeostatic regulation to prevent runaway weights
    float currentWeight = synapse->getWeight();
    
    // Check for homeostatic plasticity (soft bound)
    if (std::abs(delta) > 0.01f) {
        // Apply homeostatic scaling
        float homeostaticFactor = 1.0f;
        
        // Limit weight change based on current weight
        if (delta > 0 && currentWeight > 0.5f) {
            homeostaticFactor *= (1.0f - std::pow(currentWeight, 2));  // Stronger weights grow slower
        } else if (delta < 0 && currentWeight < -0.5f) {
            homeostaticFactor *= (1.0f - std::pow(std::abs(currentWeight), 2));
        }
        
        delta *= homeostaticFactor;
    }
    
    // Apply weight change with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
