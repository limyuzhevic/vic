#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float expectedValue;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), expectedValue(0.0f) {}
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
    // Real dopamine-modulated plasticity factor
    // Based on computational models: dopamine gain-modulates synaptic learning
    // f_DA = 1 + gain * (DA - baseline) where gain > 0
    
    float gain = 2.0f; // Plasticity gain factor
    return 1.0f + gain * (pImpl->level - pImpl->baseline);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with biological constraints
    // Combines tonic and phasic components
    
    if (dt > 0.0) {
        // Tonic decay: background dopamine level
        float tonicDecay = pImpl->decayRate * static_cast<float>(dt);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - tonicDecay);
        
        // Ensure minimum baseline level
        pImpl->level = std::max(pImpl->level, pImpl->baseline);
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with phasic burst dynamics
    // Phasic dopamine burst proportional to reward prediction error
    
    float predictionError = reward - pImpl->expectedValue;
    
    // Phasic burst: larger for unexpected rewards
    float burstAmplitude = std::min(std::max(predictionError * 2.0f, 0.0f), 1.0f);
    
    // Temporal dynamics: fast rise, slower decay
    pImpl->level = std::min(pImpl->level + burstAmplitude * 2.0f, pImpl->peak);
    
    // Update expected value
    pImpl->expectedValue = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling for learning
    // Medial shell responds to prediction errors (Zhang et al., 1994)
    
    // Phasic dip for negative prediction errors (omission)
    // Phasic burst for positive prediction errors (surprise)
    if (error < 0.0f) {
        // Negative prediction error: dip in dopamine
        pImpl->level = std::max(pImpl->baseline, pImpl->level - std::abs(error) * 0.5f);
    } else {
        // Positive prediction error: burst in dopamine
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * 1.5f);
    }
}

void Dopamine::setExpectedValue(float expected) {
    pImpl->expectedValue = expected;
}

float Dopamine::getExpectedValue() const {
    return pImpl->expectedValue;
}

} // namespace nlm

