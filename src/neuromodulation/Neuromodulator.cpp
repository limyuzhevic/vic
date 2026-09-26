#include "Neuromodulator.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl(std::shared_ptr<Config> config)
        : level(0.0f) {
        // Read baseline from config with fallback
        baseline = 0.0f;  // Default hardcoded value
        if (config) {
            // Try to get dopamine_baseline from config
            if (auto optBaseline = config->get<float>("dopamine_baseline")) {
                baseline = *optBaseline;
            }
        }
        peak = 1.0f;
        decayRate = 0.1f;
        releaseRate = 1.0f;
    }
};

Dopamine::Dopamine(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

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

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
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

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
