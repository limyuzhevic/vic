#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Store brain reference for neuromodulation effects
    class Brain* brain;
    // Track reward history for learning
    std::vector<float> rewardHistory;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             brain(nullptr) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Implement real dopamine-modulated plasticity factor
    // Dopamine scales learning rates based on reward prediction error
    float learningModulation = 0.5f + 0.5f * pImpl->level;
    
    // Add activity-dependent plasticity modulation
    learningModulation *= (1.0f + pImpl->level * 0.5f);
    
    return std::clamp(learningModulation, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics
    // Time-dependent decay towards baseline with activity-dependent release
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add spontaneous activity-dependent fluctuations
    float fluctuation = std::sin(static_cast<float>(pImpl->rng.generate()) * 6.28f) * 0.05f * pImpl->level;
    pImpl->level += fluctuation;
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling with neural dynamics
    // Phasic burst on reward prediction error with synaptic learning
    float burstMagnitude = reward * pImpl->releaseRate;
    
    // Limit to peak level to prevent runaway excitation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    
    // Track reward history for learning
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling
    // Dopamine encodes prediction error as learning signal
    float errorSignal = error * pImpl->releaseRate;
    
    // Phasic response to prediction error
    // Positive error (better than expected) -> dopamine increase
    // Negative error (worse than expected) -> dopamine decrease
    pImpl->level = std::max(0.0f, pImpl->level + errorSignal);
    
    // Apply to synaptic plasticity in connected brain regions
    if (pImpl->brain && pImpl->brain->getSTDP()) {
        // Modulate STDP learning rates based on prediction error
        float plasticityMod = std::clamp(1.0f + error * 0.5f, 0.5f, 2.0f);
        pImpl->brain->getSTDP()->setLTPWeight(0.01f * plasticityMod);
        pImpl->brain->getSTDP()->setLTDWeight(0.012f * plasticityMod);
    }
}

void Dopamine::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized");
}

float Dopamine::getMemoryConsolidationFactor() const {
    // Dopamine modulates memory consolidation based on reward
    return 1.0f + pImpl->level * 0.5f;
}

float Dopamine::getAttentionModulationFactor() const {
    // Dopamine modulates attention based on reward value
    return 1.0f + pImpl->level * 0.8f;
}

} // namespace nlm

