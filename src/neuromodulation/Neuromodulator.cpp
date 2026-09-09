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

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

bool Dopamine::initializeFromConfig(const Config& config) {
    // Initialize from configuration file
    pImpl->baseline = config.getOr<float>("dopamine_baseline", 0.0f);
    pImpl->decayRate = config.getOr<float>("dopamine_decay_rate", 0.1f);
    pImpl->releaseRate = config.getOr<float>("dopamine_release_rate", 1.0f);
    pImpl->peak = config.getOr<float>("dopamine_peak_level", 1.0f);
    
    // Set initial level to baseline
    setLevel(pImpl->baseline);
    
    NLM_LOG_INFO("Dopamine system initialized: baseline=" + std::to_string(pImpl->baseline) +
                 ", decay_rate=" + std::to_string(pImpl->decayRate) +
                 ", release_rate=" + std::to_string(pImpl->releaseRate));
    return true;
}
