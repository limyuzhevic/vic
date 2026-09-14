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

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

bool Dopamine::update(TimestepDuration dt) {
    // Validate timestep
    if (dt <= 0.0) {
        NLM_LOG_WARNING("Invalid timestep " + std::to_string(dt) + " in dopamine update");
        return false;
    }
    
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    return true;
}

bool Dopamine::signalReward(float reward) {
    // Validate reward
    if (std::isnan(reward) || std::isinf(reward)) {
        NLM_LOG_ERROR("Invalid reward value: " + std::to_string(reward));
        return false;
    }
    
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    return true;
}

bool Dopamine::signalRewardPredictionError(float error) {
    // Validate error
    if (std::isnan(error) || std::isinf(error)) {
        NLM_LOG_ERROR("Invalid reward prediction error value: " + std::to_string(error));
        return false;
    }
    
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    return true;
}

} // namespace nlm
