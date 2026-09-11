#include "AntiHebbianRule.hpp"
#include "../../brain/Synapse.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float depressionThreshold;
    float minWeight;
    float maxWeight;
    float homeostaticTarget;
    float eligibilityTraceDecay;
    float neuromodulationSensitivity;
    
    Impl() : learningRate(0.01f), depressionThreshold(0.5f), minWeight(-1.0f),
             maxWeight(1.0f), homeostaticTarget(0.0f), eligibilityTraceDecay(0.95f),
             neuromodulationSensitivity(1.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {
    NLM_LOG_INFO("Anti-Hebbian rule initialized - implements homeostatic depression");
}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    /*
     * Anti-Hebbian Learning Rule Implementation
     * 
     * Mathematical formulation:
     * Δw = -η * (pre * post) if w > 0 (depression)
     * Δw = -η * α * (pre * post) if w ≤ 0 (weaker depression)
     * 
     * Biological basis:
     * - Implements homeostatic plasticity
     * - Prevents runaway excitation in neural circuits
     * - Stabilizes network activity
     * - Reflects synaptic scaling mechanisms
     * 
     * Key features:
     * - Weight-dependent depression strength
     * - Eligibility traces for delayed effects
     * - Neuromodulator integration for context-dependent plasticity
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float currentWeight = synapse->getWeight();
    
    // Compute coactivity (simplified spike correlation)
    size_t coactivity = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++coactivity;
            }
        }
    }
    
    float delta = 0.0f;
    
    // Apply anti-Hebbian rule with homeostatic constraints
    if (currentWeight > pImpl->depressionThreshold) {
        // Stronger depression for weights above threshold
        delta = -pImpl->learningRate * static_cast<float>(coactivity);
    } else {
        // Weaker depression for weights at or below threshold
        delta = -pImpl->learningRate * 0.5f * static_cast<float>(coactivity);
    }
    
    // Apply neuromodulation if available
    // TODO: Integrate with actual neuromodulator system
    float modulationFactor = 1.0f;  // Placeholder
    delta *= modulationFactor;
    
    // Apply homeostatic scaling
    float homeostaticCorrection = pImpl->homeostaticTarget - currentWeight;
    if (std::abs(homeostaticCorrection) > 0.01f) {
        float homeostaticDelta = -0.001f * homeostaticCorrection;
        delta += homeostaticDelta;
    }
    
    // Update eligibility trace
    // TODO: Implement proper eligibility trace system
    
    applyWeightChange(synapse, delta);
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply homeostasis to prevent weights from drifting too far
    if (std::abs(newWeight) > 1.0f) {
        float scaleFactor = 0.9f;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    } else {
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    }
    
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

void AntiHebbianRule::setDepressionThreshold(float threshold) {
    pImpl->depressionThreshold = std::clamp(threshold, 0.0f, 10.0f);
}

float AntiHebbianRule::getDepressionThreshold() const {
    return pImpl->depressionThreshold;
}

void AntiHebbianRule::setHomeostaticTarget(float target) {
    pImpl->homeostaticTarget = std::clamp(target, -1.0f, 1.0f);
}

float AntiHebbianRule::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

} // namespace nlm