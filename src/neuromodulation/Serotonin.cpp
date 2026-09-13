#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float tonicLevel;
    float phasicLevel;
    float moodLevel;
    float impulseControl;
    float socialBehavior;
    
    // Behavioral modulation
    float socialSensitivity;
    float impulseThreshold;
    float behavioralBias;
    float rewardProcessing;
    
    // Time tracking
    float timeSinceLastSocialInteraction;
    float timeSinceLastReward;
    float lastImbalanceTime;
    
    // Temporal components
    float moodHistory;
    float impulseHistory;
    float socialHistory;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : level(0.0f), baseline(0.1f), tonicLevel(0.1f), phasicLevel(0.0f),
             moodLevel(0.5f), impulseControl(0.5f), socialBehavior(0.5f),
             socialSensitivity(1.0f), impulseThreshold(0.5f), behavioralBias(0.0f),
             rewardProcessing(0.5f), timeSinceLastSocialInteraction(0.0f),
             timeSinceLastReward(0.0f), lastImbalanceTime(0.0f),
             moodHistory(0.0f), impulseHistory(0.0f), socialHistory(0.0f)
    {
        std::random_device rd;
        rng.seed(rd());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and behavioral state
    // Inverted U-curve with mood: moderate levels optimal for learning
    float optimalMood = 0.5f;
    float moodDeviation = std::abs(pImpl->moodLevel - optimalMood);
    
    // Mood-related plasticity
    float moodPlasticity = 1.0f - moodDeviation * moodDeviation * 2.0f;
    
    // Impulse control enhances plasticity
    float impulsePlasticity = pImpl->impulseControl;
    
    // Social behavior modulates plasticity
    float socialPlasticity = pImpl->socialBehavior;
    
    // Combined plasticity factor
    float plasticity = moodPlasticity * impulsePlasticity * socialPlasticity;
    
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Update time tracking
    pImpl->timeSinceLastSocialInteraction += timeStep;
    pImpl->timeSinceLastReward += timeStep;
    
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline,
                           pImpl->level - 0.01f * timeStep);
    
    // Update phasic component
    pImpl->phasicLevel *= (1.0f - 0.05f * timeStep);
    
    // Update temporal histories
    pImpl->moodHistory *= 0.95f;
    pImpl->impulseHistory *= 0.95f;
    pImpl->socialHistory *= 0.95f;
    
    // Time-dependent behavioral adjustments
    if (pImpl->timeSinceLastSocialInteraction > 10.0f) {
        // Prolonged isolation increases impulsivity
        pImpl->impulseControl = std::max(0.1f, pImpl->impulseControl - 0.01f);
    } else {
        // Social interaction restores impulse control
        pImpl->impulseControl = std::min(1.0f, pImpl->impulseControl + 0.02f);
    }
}

void Serotonin::signalSocialFeedback(float socialReward) {
    // Signal social feedback (positive or negative)
    
    // Update mood based on social reward
    if (socialReward > 0.0f) {
        // Positive social feedback increases mood
        pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + socialReward * 0.3f);
        pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + socialReward * 0.2f);
        pImpl->phasicLevel = std::min(pImpl->phasicLevel + socialReward * 0.4f, 1.0f);
    } else {
        // Negative social feedback decreases mood
        pImpl->moodLevel = std::max(0.0f, pImpl->moodLevel + socialReward * 0.2f);
        pImpl->socialBehavior = std::max(0.1f, pImpl->socialBehavior + socialReward * 0.1f);
    }
    
    // Overall serotonin level responds to social experience
    pImpl->level = std::min(1.0f, pImpl->level + std::abs(socialReward) * 0.1f);
    
    // Reset social interaction timer
    pImpl->timeSinceLastSocialInteraction = 0.0f;
    
    // Update social history
    pImpl->socialHistory += std::abs(socialReward);
    
    NLM_LOG_DEBUG("5-HT social feedback: reward=%.3f, mood=%.3f, social=%.3f",
                  socialReward, pImpl->moodLevel, pImpl->socialBehavior);
}

void Serotonin::signalImbalance(float behavioralBias) {
    // Signal behavioral imbalance (e.g., too much exploration vs exploitation)
    
    pImpl->behavioralBias = behavioralBias;
    
    // Adjust impulse control based on behavioral bias
    if (behavioralBias > 0.5f) {
        // Excessive exploration bias reduces impulse control
        pImpl->impulseControl = std::max(0.1f, pImpl->impulseControl - 0.1f);
        // Impulses spike
        pImpl->phasicLevel = std::min(pImpl->phasicLevel + 0.3f, 1.0f);
    } else if (behavioralBias < -0.5f) {
        // Excessive exploitation bias increases impulsivity
        pImpl->impulseControl = std::min(1.0f, pImpl->impulseControl + 0.1f);
        // Suppression of impulses
        pImpl->phasicLevel = std::max(0.0f, pImpl->phasicLevel - 0.2f);
    }
    
    // Overall serotonin level responds to behavioral imbalance
    pImpl->level = std::min(1.0f, pImpl->level + std::abs(behavioralBias) * 0.15f);
    
    // Update impulse history
    pImpl->impulseHistory += std::abs(behavioralBias);
    
    NLM_LOG_DEBUG("5-HT behavioral imbalance: bias=%.3f, impulse_control=%.3f",
                  behavioralBias, pImpl->impulseControl);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulseControlLevel() const {
    return pImpl->impulseControl;
}

float Serotonin::getSocialBehaviorLevel() const {
    return pImpl->socialBehavior;
}

void Serotonin::setMoodBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

void Serotonin::setImpulseThreshold(float threshold) {
    pImpl->impulseThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Serotonin::setSocialSensitivity(float sensitivity) {
    pImpl->socialSensitivity = std::clamp(sensitivity, 0.1f, 2.0f);
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->phasicLevel = 0.0f;
    pImpl->moodLevel = 0.5f;
    pImpl->impulseControl = 0.5f;
    pImpl->socialBehavior = 0.5f;
    
    // Reset behavioral state
    pImpl->behavioralBias = 0.0f;
    pImpl->rewardProcessing = 0.5f;
    
    // Reset temporal components
    pImpl->moodHistory = 0.0f;
    pImpl->impulseHistory = 0.0f;
    pImpl->socialHistory = 0.0f;
    
    NLM_LOG_INFO("Serotonin system reset");
}

} // namespace nlm