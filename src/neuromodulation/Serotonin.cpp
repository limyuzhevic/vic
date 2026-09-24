#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodWeight;
    float rewardResponse;
    float inhibitionLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             moodWeight(1.0f), rewardResponse(0.0f), inhibitionLevel(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood and reward processing
    // Higher serotonin promotes stability and reduces impulsivity
    if (pImpl->level < 0.3f) {
        return 0.8f - pImpl->level * 0.2f;  // Lower plasticity when low
    } else {
        return 0.5f + pImpl->level * 0.1f;  // Moderate plasticity
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: Slower decay, responds to reward prediction errors
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Natural decline after reward responses
    if (pImpl->rewardResponse > 0.0f) {
        pImpl->rewardResponse -= dt * 0.01f;
    }
}

void Serotonin::signalReward(float reward) {
    // Serotonin responds to reward prediction errors (inverse dopamine)
    // Low reward or unexpected lack of reward increases serotonin
    float response = -reward * pImpl->releaseRate;  // Negative response to reward
    pImpl->level = std::max(pImpl->level + response, pImpl->baseline);
    pImpl->rewardResponse = std::abs(response);
}

void Serotonin::signalPunishment(float punishment) {
    // Strong response to punishment
    float response = punishment * pImpl->releaseRate * 2.0f;  // Stronger response
    pImpl->level = std::min(pImpl->peak, pImpl->level + response);
}

void Serotonin::modulateMood(float mood) {
    // Serotonin modulates mood and emotional state
    float moodEffect = mood * pImpl->moodWeight;
    pImpl->level = std::max(pImpl->baseline, pImpl->level + moodEffect * 0.1f);
}

void Serotonin::reduceImpulsivity() {
    // Serotonin reduces impulsive behavior
    float inhibition = pImpl->level * 0.2f;
    if (inhibition > 0.05f) {
        // In a full implementation, this would be passed to action selection
        NLM_LOG_DEBUG("Serotonin reduced impulsivity by " + std::to_string(inhibition));
    }
}

} // namespace nlm