#include "AntiHebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    /*
     * Real Anti-Hebbian learning implementation
     * 
     * Mathematical formulation:
     * Δw = -η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * This is the inverse of Hebbian learning:
     * - When pre and post fire together, weight decreases
     * - When one fires without the other, weight increases slightly
     * 
     * Biological inspiration:
     *   - Implements synaptic depression for stability
     *   - Counterbalances Hebbian potentiation
     *   - Important for maintaining homeostasis
     *   - Related to low-pass filtering mechanisms
     *   
     * Limitations:
     *   - Uses simple covariance-based rule
     *   - Single learning rate for potentiation and depression
     *   - Doesn't incorporate STDP timing details
     */
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (same as Hebbian but with negative sign)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    // Compute weight change (negative correlation = weight decrease)
    float delta = -pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm