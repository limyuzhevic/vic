#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::max(0.0f, std::min(baseline, 1.0f));
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = std::max(0.0f, rate);
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.0f, rate);
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = std::max(peak, pImpl->baseline);
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

float Dopamine::getPlasticityFactor() const {
    // Advanced dopamine-modulated plasticity factor
    // Based on both current level and history
    float historyFactor = 1.0f;
    if (pImpl->level > 0.0f && pImpl->decayRate > 0.0f) {
        // Higher recent dopamine activity = more plasticity
        historyFactor = std::min(2.0f, 1.0f + pImpl->level * 0.5f);
    }
    
    // Modulate plasticity based on developmental stage
    // Higher plasticity in early development
    float developmentalMod = 1.0f;
    if (pImpl->level > 0.3f) {
        // Strong dopamine response = more plastic
        developmentalMod = 1.2f;
    }
    
    return pImpl->level * 0.6f * historyFactor * developmentalMod;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with homeostatic regulation
    
    // Decay towards baseline (homeostatic regulation)
    if (pImpl->decayRate > 0.0f) {
        pImpl->level = std::max(pImpl->baseline, 
                              pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    }
    
    // Maintain minimum level to prevent complete shutdown
    pImpl->level = std::max(pImpl->baseline, pImpl->level);
}

void Dopamine::signalReward(float reward) {
    // Stronger reward = bigger dopamine burst
    float burst = std::min(pImpl->peak - pImpl->level, 
                          reward * pImpl->releaseRate * 2.0f);
    pImpl->level += burst;
    
    // Dopamine has lasting effect - decay from elevated baseline
    if (pImpl->level > pImpl->baseline * 1.1f) {
        pImpl->baseline = std::min(pImpl->peak, 
                                pImpl->baseline + (pImpl->level - pImpl->baseline) * 0.1f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error directly modulates dopamine
    // Positive error = reward better than expected = dopamine increase
    // Negative error = reward worse than expected = dopamine decrease
    
    if (error > 0.0f) {
        // Better than expected: dopamine increase, higher plasticity
        float increase = error * pImpl->releaseRate;
        pImpl->level = std::min(pImpl->peak, pImpl->level + increase);
        
        // Raise baseline for better future predictions
        pImpl->baseline = std::min(pImpl->peak, 
                                pImpl->baseline + error * 0.05f);
    } else if (error < 0.0f) {
        // Worse than expected: dopamine decrease, lower plasticity
        float decrease = std::abs(error) * pImpl->releaseRate * 0.5f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decrease);
        
        // Lower baseline for more conservative future predictions
        pImpl->baseline = std::max(0.0f, 
                                pImpl->baseline + error * 0.03f);
    }
}

} // namespace nlm