#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float burstFactor;
    float predictionErrorInfluence;
    float tonicLevel;
    float tonicDecay;
    float adaptationTime;
    
    Impl() 
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , burstFactor(1.5f)
        , predictionErrorInfluence(1.0f)
        , tonicLevel(baseline)
        , tonicDecay(0.01f)
        , adaptationTime(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates STDP: positive errors enhance LTP, negative errors enhance LTD
    // Use tanh to center around 0, then scale to [0, 1]
    float errorModulation = std::tanh(pImpl->level / 0.5f);
    return 0.5f + 0.5f * (errorModulation + 1.0f) * 0.5f;
}

void Dopamine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Tonic component: slow decay towards baseline
    pImpl->tonicLevel = std::max(pImpl->baseline, pImpl->tonicLevel - pImpl->tonicDecay * dtFloat);
    
    // Phasic component: rapid decay of bursts
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Adapt over time
    pImpl->adaptationTime += dtFloat;
    if (pImpl->adaptationTime > 1000.0f) {
        pImpl->adaptationTime = 0.0f;
        pImpl->tonicLevel = pImpl->baseline;
    }
}

void Dopamine::signalSpike() {
    pImpl->level = std::min(pImpl->peak, pImpl->level * pImpl->burstFactor);
    pImpl->adaptationTime = 0.0f;
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->tonicLevel = pImpl->baseline;
    pImpl->adaptationTime = 0.0f;
}

void Dopamine::signalReward(float reward) {
    // Phasic burst proportional to reward magnitude
    float burst = reward * pImpl->releaseRate * pImpl->burstFactor;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Store reward for plasticity eligibility
    pImpl->adaptationTime = 0.0f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error: positive for better-than-expected, negative for worse
    // This is the core signal for reinforcement learning
    float errorSignal = error * pImpl->predictionErrorInfluence;
    pImpl->level = std::max(0.0f, pImpl->level + errorSignal * pImpl->burstFactor);
    
    // Integrate with prediction error system
    pImpl->adaptationTime = 0.0f;
}

void Dopamine::setPredictionError(float error) {
    // Direct integration of prediction error signal
    pImpl->predictionErrorInfluence = 1.0f + std::abs(error);
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
    pImpl->tonicLevel = baseline;
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = rate;
}

void Dopamine::setPeakLevel(float level) {
    pImpl->peak = level;
}

void Dopamine::setBurstFactor(float factor) {
    pImpl->burstFactor = factor;
}

} // namespace nlm
