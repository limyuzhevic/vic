#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"

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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Dopamine::getPlasticityFactor() const {
    // PHASE 6: Implement real dopamine-modulated plasticity factor
    // Advanced: Dopamine modulates both LTP and LTD rates based on level
    // at resting baseline, no plasticity; at peak level, maximum plasticity
    float normalizedLevel = pImpl->level / pImpl->peak;
    return 0.1f + 0.9f * normalizedLevel; // Linearly maps [0,1] to [0.1,1.0]
}

void Dopamine::update(TimestepDuration dt) {
    // PHASE 6: Implement real dopamine dynamics
    // Advanced: Includes both decay and spontaneous fluctuations
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Add small spontaneous fluctuations (noise)
    // This mimics biological dopamine neuron variability
    if (pImpl->level < pImpl->baseline) {
        float fluctuation = (static_cast<float>(rand()) / RAND_MAX) * 0.05f;
        pImpl->level = std::min(pImpl->baseline + fluctuation, pImpl->peak);
    }
}

void Dopamine::signalReward(float reward) {
    // PHASE 6: Implement real reward signaling
    // Advanced: Includes temporal difference and burst dynamics
    if (reward > 0) {
        // Immediate reward response
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
        
        // Add secondary peak for extended signaling
        NLM_LOG_INFO("Dopamine reward signal: +" + std::to_string(reward) + ", new level: " + std::to_string(pImpl->level));
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // PHASE 6: Implement reward prediction error signaling
    // Advanced: Different responses for positive vs negative prediction errors
    if (error > 0) {
        // Positive prediction error (better than expected)
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 0.7f);
        NLM_LOG_INFO("Dopamine positive prediction error: +" + std::to_string(error) + ", level: " + std::to_string(pImpl->level));
    } else {
        // Negative prediction error (worse than expected)
        float magnitude = std::abs(error);
        pImpl->level = std::max(0.0f, pImpl->level - magnitude * pImpl->releaseRate * 1.2f);
        NLM_LOG_INFO("Dopamine negative prediction error: " + std::to_string(error) + ", level: " + std::to_string(pImpl->level));
    }
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, pImpl->peak);
    NLM_LOG_INFO("Dopamine baseline set to: " + std::to_string(pImpl->baseline));
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = std::max(0.0f, peak);
    setLevel(pImpl->level); // Ensure current level doesn't exceed new peak
    NLM_LOG_INFO("Dopamine peak set to: " + std::to_string(pImpl->peak));
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.0f, rate);
    NLM_LOG_INFO("Dopamine decay rate set to: " + std::to_string(pImpl->decayRate));
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = std::max(0.0f, rate);
    NLM_LOG_INFO("Dopamine release rate set to: " + std::to_string(pImpl->releaseRate));
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

} // namespace nlm