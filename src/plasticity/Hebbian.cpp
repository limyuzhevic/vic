#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Compute weight change using covariance rule: Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    // Simplified for spike-based systems: Δw = η * (coactivity - baseline)
    
    // Count correlated spike pairs (coactivity)
    size_t coactivity = 0;
    float totalPreRate = 0.0f;
    float totalPostRate = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        totalPreRate += 1.0f / (dt > 0 ? dt : 1.0f);
        for (Timestamp postTime : postSpikes) {
            float dtBetween = std::abs(postTime - preTime);
            // Count spikes within 50ms as correlated (Hebbian window)
            if (dtBetween < 50.0f) {
                coactivity++;
            }
        }
    }
    
    // Compute expected activity under independence
    float preRate = totalPreRate / static_cast<float>(preSpikes.size());
    float postRate = totalPostRate / static_cast<float>(postSpikes.size());
    float expectedCoactivity = preRate * postRate * dt * 50.0f; // Approximate
    
    // Compute Hebbian delta with covariance rule
    float correlation = static_cast<float>(coactivity) - expectedCoactivity;
    
    // Apply Hebbian update with learning rate
    float delta = pImpl->learningRate * correlation;
    
    // Apply with anti-Hebbian threshold to prevent runaway potentiation
    if (correlation > 0) {
        // Potentiation with soft bound
        float maxPotentiation = std::max(0.0f, pImpl->maxWeight - synapse->getWeight());
        delta = std::min(delta, maxPotentiation);
    } else if (correlation < 0) {
        // Depression with soft bound
        float maxDepression = std::min(0.0f, pImpl->minWeight - synapse->getWeight());
        delta = std::max(delta, maxDepression);
    }
    
    // Apply weight change
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
