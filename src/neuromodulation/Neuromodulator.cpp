#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Dopamine dynamics state
    float tonicLevel;      // Baseline tonic firing rate
    float phasicActive;    // Whether in phasic burst (0-1)
    float timeSinceLastSpill;
    
    // Reward prediction error history for credit assignment
    float lastPredictionError;
    float predictionErrorLearningRate;
    
    // Implementation of dopamine neuron dynamics
    // Based on biological models (Surikov & Tsodyks, 2008)
    Impl() 
        : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , tonicLevel(0.05f), phasicActive(0.0f), timeSinceLastSpill(0.0f)
        , lastPredictionError(0.0f), predictionErrorLearningRate(0.01f) {}
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

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->phasicActive = (level > pImpl->baseline * 1.5f) ? 1.0f : 0.0f;
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity: D1 receptors enhance LTP, D2 receptors enhance LTD
    // High dopamine increases overall plasticity but biases direction
    float plasticityFactor = 1.0f;
    
    if (pImpl->level > pImpl->baseline) {
        float excess = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
        
        // D1-like effect: enhance long-term potentiation (LTP)
        float d1Effect = 1.0f + excess * 0.5f;
        
        // D2-like effect: enhance long-term depression (LTD) 
        float d2Effect = 1.0f + excess * 0.3f;
        
        // Net effect depends on which receptors are active
        // Simplified: moderate-to-high dopamine favors LTP
        plasticityFactor = d1Effect * 0.7f + d2Effect * 0.3f;
    }
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Two-component model: tonic and phasic
    // Tonic: decay toward baseline (slow)
    if (pImpl->level > pImpl->baseline) {
        float decay = pImpl->decayRate * timeStep;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    }
    
    // Phasic: decay from peak (faster for bursts)
    if (pImpl->phasicActive > 0.0f) {
        float phasicDecay = (pImpl->decayRate * 2.0f) * timeStep;
        pImpl->phasicActive = std::max(0.0f, pImpl->phasicActive - phasicDecay);
        if (pImpl->phasicActive <= 0.0f) {
            pImpl->level = pImpl->baseline;
        }
    }
    
    // Update time since last event
    pImpl->timeSinceLastSpill += timeStep;
}

float Dopamine::getBaselineLevel() const {
    return pImpl->baseline;
}

bool Dopamine::isActive() const {
    return (pImpl->level > pImpl->baseline * 1.1f) || (pImpl->phasicActive > 0.0f);
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine burst response to unexpected reward (reward prediction error > 0)
    // Based on Schultz et al. (1997) dopamine neuron responses
    float rewardMagnitude = std::min(reward, 1.0f);
    
    // Immediate burst
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardMagnitude * pImpl->releaseRate * 2.0f);
    pImpl->phasicActive = 1.0f;
    pImpl->timeSinceLastSpill = 0.0f;
    
    // Update prediction error (positive for better-than-expected reward)
    pImpl->lastPredictionError = std::max(0.0f, reward - pImpl->baseline);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phasic response to reward prediction error (Schultz et al. model)
    // Positive error (better than expected): dopamine burst
    // Negative error (worse than expected): dopamine dip/opponent response
    
    float errorMagnitude = std::min(std::abs(error), 1.0f);
    
    if (error > 0) {
        // Positive prediction error: burst of dopamine
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorMagnitude * pImpl->releaseRate * 1.5f);
        pImpl->phasicActive = 1.0f;
        pImpl->timeSinceLastSpill = 0.0f;
    } else {
        // Negative prediction error: dip (inhibition)
        // Model opponent process: dip below baseline
        pImpl->level = std::max(0.0f, pImpl->level - errorMagnitude * pImpl->releaseRate * 0.8f);
        pImpl->phasicActive = 0.0f; // Not in burst state
    }
    
    pImpl->lastPredictionError = error;
}

void Dopamine::setBaselineLevel(float level) {
    pImpl->baseline = std::clamp(level, 0.0f, 0.5f);
    pImpl->tonicLevel = pImpl->baseline;
}

void Dopamine::configure(float baseline, float peak, float decayRate, float releaseRate) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 0.5f);
    pImpl->peak = std::max(pImpl->baseline + 0.1f, std::clamp(peak, pImpl->baseline + 0.1f, 2.0f));
    pImpl->decayRate = std::clamp(decayRate, 0.01f, 1.0f);
    pImpl->releaseRate = std::clamp(releaseRate, 0.5f, 3.0f);
}

} // namespace nlm
