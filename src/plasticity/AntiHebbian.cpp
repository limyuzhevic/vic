#include "AntiHebbianRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float activityThreshold;  // For anti-Hebbian gating
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             activityThreshold(0.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    /*
     * Anti-Hebbian learning implementation
     * 
     * Mathematical formulation:
     * Δw = -η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Unlike Hebbian (which strengthens correlations), anti-Hebbian
     * weakens connections when pre and post neurons fire together.
     * 
     * Biological inspiration:
     *   - Contrast enhancement in visual cortex
     *   - Prevention of runaway excitation
     *   - Lateral inhibition mechanisms
     *   - Homeostatic regulation of neural activity
     *   
     * Implementation details:
     *   - Uses negative correlation count
     *   - Activity-dependent gating
     *   - Maintains weight bounds
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (simplified)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within a broad time window as correlated
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    // Anti-Hebbian weight change: negative correlation effect
    // More negative correlations = more weight reduction
    float delta = -pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Activity-dependent gating: only learn if neuron activity is significant
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    if (preActivity > pImpl->activityThreshold && postActivity > pImpl->activityThreshold) {
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

void AntiHebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float AntiHebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void AntiHebbianRule::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float AntiHebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

void AntiHebbianRule::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = std::clamp(threshold, 0.0f, 100.0f);
}

float AntiHebbianRule::getActivityThreshold() const {
    return pImpl->activityThreshold;
}

} // namespace nlm
