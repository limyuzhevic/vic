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

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::signalPredictionError(float error) {
    // TODO PHASE 2: Implement prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error directly
    // Note: Prediction error typically includes both reward prediction error and curiosity prediction error
    // For now, use absolute value to ensure positive response to prediction errors
    float effectiveError = std::abs(error);
    pImpl->level = std::max(0.0f, pImpl->level + effectiveError * pImpl->releaseRate * 0.5f);
}

void Dopamine::signalNovelty(float novelty) {
    // TODO PHASE 2: Implement novelty signaling
    // PLACEHOLDER: Dopamine responds to novelty
    // Novelty drives exploration, so increase dopamine with novelty signals
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate * 0.3f);
}

void Dopamine::updateFromSources(float rewardPredictionError, float curiosity, float novelty, TimestepDuration dt) {
    // TODO PHASE 2: Implement unified neuromodulator update
    // Integrate multiple signals into a unified dopamine response
    
    // Base update (decay)
    update(dt);
    
    // Signal from reward prediction error (strong influence)
    signalRewardPredictionError(rewardPredictionError);
    
    // Signal from prediction error (medium influence)
    signalPredictionError(rewardPredictionError * 0.7f);  // Scale relative to RPE
    
    // Signal from novelty (exploration drive)
    signalNovelty(novelty);
    
    // Combine curiosity signal (modulates overall exploration tendency)
    if (curiosity > 0.0f) {
        // Curiosity drives learning and exploration
        float curiosityBoost = curiosity * 0.5f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + curiosityBoost * pImpl->releaseRate * 0.4f);
    }
}

} // namespace nlm