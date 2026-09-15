#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionErrorSignal;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             predictionErrorSignal(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

void Reward::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Reward system initialized");
}

const char* Reward::getName() const {
    return "Reward";
}

float Reward::getLevel() const {
    return pImpl->level;
}

void Reward::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Reward::getPlasticityFactor() const {
    // Reward directly modulates plasticity - higher reward = stronger learning
    return 0.5f + 0.5f * pImpl->level;
}

void Reward::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Reward::signal(float reward, float predictionError) {
    // Combined reward and prediction error signaling
    pImpl->predictionErrorSignal = predictionError;
    float totalSignal = reward + std::abs(predictionError) * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + totalSignal * pImpl->releaseRate);
}

float Reward::getPredictionErrorSignal() const {
    return pImpl->predictionErrorSignal;
}

} // namespace nlm