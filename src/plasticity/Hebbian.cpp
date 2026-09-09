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
     * Real Hebbian learning implementation with spike-coincidence detection
     * 
     * Mathematical formulation (Realistic spike-based Hebbian):
     * Δw = η * Σ_t (pre(t) * post(t - Δt))
     * Where:
     *   - pre(t) = pre-synaptic spike at time t (1 if spiked, 0 otherwise)
     *   - post(t - Δt) = post-synaptic spike Δt time after pre spike
     *   - η = learningRate (typically 0.001 - 0.01)
     *   - Δt = optimal spike timing window (10-30ms for biological realism)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-dependent Hebbian plasticity
     *   - Spike-timing dependent but broader window than STDP
     *   - Implemented at excitatory synapses in cortex
     *   
     * Learning window shape:
     *   - Peak at Δt = 0 (simultaneous spikes)
     *   - Decay with time lag
     *   - Can include both pre->post and post->pre components
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Optimized spike coincidence detection
    float totalDelta = 0.0f;
    float optimalWindow = 20.0f;  // ms - optimal spike timing window
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            // Spike coincidence within optimal window
            if (std::abs(dt) <= optimalWindow) {
                // Gaussian learning window centered at dt=0
                float weight = std::exp(-(dt * dt) / (2.0f * optimalWindow * optimalWindow));
                totalDelta += weight;
            }
        }
    }
    
    // Apply weight change based on spike coincidence
    float delta = pImpl->learningRate * totalDelta;
    
    // Apply with bounds and ensure biological realism
    if (std::abs(delta) > 1e-8f) {
        // Scale by current weight to prevent runaway potentiation
        float currentWeight = synapse->getWeight();
        float normalizedDelta = delta * (1.0f - std::abs(currentWeight) / (pImpl->maxWeight + 0.01f));
        applyWeightChange(synapse, normalizedDelta);
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
