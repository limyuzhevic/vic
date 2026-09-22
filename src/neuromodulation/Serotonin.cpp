// include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float moodFactor;
    float impulsivityLevel;
    float socialBehaviorModulator;
    float decayRate;
    
    Impl() : level(0.0f), moodFactor(0.5f), impulsivityLevel(0.5f), socialBehaviorModulator(0.5f), decayRate(0.1f) {}
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
    // 5-HT directly modulates mood and behavior
    pImpl->moodFactor = 0.3f + 0.7f * pImpl->level;
    pImpl->impulsivityLevel = std::max(0.1f, 1.0f - pImpl->level);  // Inverse relationship
    pImpl->socialBehaviorModulator = 0.2f + 0.8f * pImpl->level;
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT has complex effects on plasticity - optimal levels enhance, extremes impair
    float sero = pImpl->level;
    if (sero < 0.2f) {
        return 0.6f;  // Low levels = impaired learning, increased impulsivity
    } else if (sero < 0.8f) {
        return 1.0f + 0.2f * (sero - 0.2f) / 0.6f;  // Optimal range for learning
    } else {
        return 0.7f;  // High levels = reduced plasticity, enhanced social behavior
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Mood modulation effects last longer
    pImpl->moodFactor = std::clamp(pImpl->moodFactor * 0.98f + 0.02f, 0.1f, 1.0f);
}

void Serotonin::signalRewardOutcome(float reward, float predictionError) {
    // 5-HT responds to reward prediction errors and outcomes
    if (predictionError > 0) {
        // Positive prediction error (better than expected) increases 5-HT
        pImpl->level = std::min(1.0f, pImpl->level + predictionError * 0.3f);
    } else {
        // Negative prediction error (worse than expected) decreases 5-HT
        pImpl->level = std::max(0.0f, pImpl->level + predictionError * 0.2f);
    }
}

void Serotonin::enhanceSocialLearning(float socialCohesion) {
    // 5-HT enhances social learning and conformity
    pImpl->socialBehaviorModulator += socialCohesion * 0.05f;
}

float Serotonin::getMoodFactor() const {
    return pImpl->moodFactor;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

float Serotonin::getSocialBehaviorModulator() const {
    return pImpl->socialBehaviorModulator;
}

} // namespace nlm