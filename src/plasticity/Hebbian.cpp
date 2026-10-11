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
     * Real Hebbian learning implementation based on spike-timing covariance
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Where:
     *   ⟨pre⟩ = average firing rate of pre-synaptic neuron
     *   ⟨post⟩ = average firing rate of post-synaptic neuron  
     *   ⟨pre * post⟩ = average co-activity (correlation) between neurons
     *   η = learning rate (Hebbian parameter)
     * 
     * Biological inspiration:
     *   - Reflects synaptic weight changes based on correlated activity
     *   - Implements "neurons that fire together, wire together"
     *   - Prevents runaway potentiation through covariance normalization
     *   
     * Advantages over simple correlation counting:
     *   - Accounts for firing rates (baseline activity)
     *   - True covariance captures statistical relationship
     *   - More biologically plausible
     *   - Better at distinguishing correlated vs coincidental activity
     */
     
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate average firing rates
    float preRate = static_cast<float>(preSpikes.size()) / static_cast<float>(preSpikes.size() + 1);
    float postRate = static_cast<float>(postSpikes.size()) / static_cast<float>(postSpikes.size() + 1);
    
    // Calculate average co-activity (spike pairs within time window)
    size_t coactivityCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(std::abs(postTime - preTime));
            // Count spikes within biological correlation window (e.g., 50ms)
            if (dt < 50.0f) {
                ++coactivityCount;
            }
        }
    }
    float avgCoactivity = static_cast<float>(coactivityCount) / 
                         (static_cast<float>(preSpikes.size()) * static_cast<float>(postSpikes.size()) + 1);
    
    // Calculate covariance: ⟨pre*post⟩ - ⟨pre⟩⟨post⟩
    float covariance = avgCoactivity - (preRate * postRate);
    
    // Apply Hebbian learning rule with covariance
    float delta = pImpl->learningRate * covariance;
    
    // Apply with bounds and minimum threshold to prevent insignificant updates
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
