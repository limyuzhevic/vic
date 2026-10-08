#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    float currentLevel;
    float accumulatedSocialLearning;
    float impulsivity;
    float mood;
    float plasticityFactor;
    
    // Historical data for social learning
    std::vector<float> moodHistory;
    std::vector<float> socialExperienceHistory;
    
    // Serotonin dynamics
    float moodRecoveryRate;
    float socialLearningRate;
    float impulsivityModulation;
    
    // Temporal dynamics
    TimestepDuration timeSinceLastChange;
    
    Impl() 
        : currentLevel(0.5f)
        , accumulatedSocialLearning(0.0f)
        , impulsivity(0.5f)
        , mood(0.5f)
        , plasticityFactor(1.0f)
        , moodRecoveryRate(0.1f)
        , socialLearningRate(0.05f)
        , impulsivityModulation(1.0f)
        , timeSinceLastChange(0.0f)
    {
    }
};

Serotonin::Serotonin() : pImpl(std::make_unique<Impl>()) {
}

Serotonin::~Serotonin() = default;

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and social learning
    // High positive mood and social learning enhances plasticity
    // Negative mood reduces plasticity (more stable learning)
    float baseFactor = 1.0f;
    
    // Mood-dependent modulation
    float moodModulation = 0.5f + 0.5f * pImpl->mood;  // Range [0.5, 1.0]
    
    // Social learning accumulation enhances plasticity
    float socialModulation = 1.0f + pImpl->accumulatedSocialLearning * 0.3f;  // Range [1.0, 1.3]
    
    // Impulsivity affects stability vs exploration
    float impulseModulation = 0.8f + 0.4f * pImpl->impulsivity;  // Range [0.8, 1.2]
    
    // Combined effect
    float factor = baseFactor * moodModulation * socialModulation * impulseModulation;
    
    // Clamp to reasonable range
    return std::clamp(factor, 0.2f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->timeSinceLastChange += dt;
    
    // Natural recovery toward baseline levels
    if (pImpl->mood > 0.5f) {
        pImpl->mood = std::max(0.5f, pImpl->mood - pImpl->moodRecoveryRate * dt);
    } else {
        pImpl->mood = std::min(0.5f, pImpl->mood + pImpl->moodRecoveryRate * dt);
    }
    
    // Decay accumulated social learning over time
    pImpl->accumulatedSocialLearning *= (1.0f - 0.001f * dt);
    
    // Impulsivity fluctuates with social experiences
    pImpl->impulsivity = std::clamp(pImpl->impulsivity, 0.1f, 1.0f);
    
    // Add to history
    pImpl->moodHistory.push_back(pImpl->mood);
    if (pImpl->moodHistory.size() > 1000) {
        pImpl->moodHistory.erase(pImpl->moodHistory.begin());
    }
}

void Serotonin::signalMoodChange(float moodDelta) {
    // Apply mood change and clamp to valid range
    pImpl->mood = std::clamp(pImpl->mood + moodDelta, 0.0f, 1.0f);
    
    // Mood change affects impulsivity
    if (moodDelta > 0.0f) {
        // Positive mood increases impulsivity slightly
        pImpl->impulsivity = std::min(1.0f, pImpl->impulsivity + 0.1f * moodDelta);
    } else {
        // Negative mood decreases impulsivity
        pImpl->impulsivity = std::max(0.1f, pImpl->impulsivity + 0.05f * moodDelta);
    }
    
    NLM_LOG_INFO(std::string("Serotonin: Mood change to ") + std::to_string(pImpl->mood));
}

void Serotonin::signalSocialExperience(float socialReward) {
    // Social experiences affect serotonin accumulation
    if (socialReward > 0.0f) {
        pImpl->accumulatedSocialLearning += socialReward * pImpl->socialLearningRate;
        
        // Positive social experiences boost mood
        pImpl->mood = std::min(1.0f, pImpl->mood + socialReward * 0.2f);
    } else {
        // Negative social experiences reduce mood but also build resilience
        pImpl->mood = std::max(0.0f, pImpl->mood + socialReward * 0.3f);
        // Negative experiences can increase future resilience
        pImpl->accumulatedSocialLearning += std::abs(socialReward) * 0.01f;
    }
    
    pImpl->socialExperienceHistory.push_back(socialReward);
    if (pImpl->socialExperienceHistory.size() > 1000) {
        pImpl->socialExperienceHistory.erase(pImpl->socialExperienceHistory.begin());
    }
    
    NLM_LOG_INFO(std::string("Serotonin: Social experience reward = ") + std::to_string(socialReward));
}

void Serotonin::signalRewardPrediction(float predictedReward, float actualReward) {
    float predictionError = actualReward - predictedReward;
    
    // Prediction errors affect serotonin levels significantly
    if (predictionError > 0.1f) {
        // Positive prediction error (better than expected) increases serotonin
        pImpl->mood = std::min(1.0f, pImpl->mood + predictionError * 0.3f);
    } else if (predictionError < -0.1f) {
        // Negative prediction error (worse than expected) decreases serotonin
        pImpl->mood = std::max(0.0f, pImpl->mood + predictionError * 0.2f);
    }
    
    // Update impulsivity based on prediction accuracy
    if (std::abs(predictionError) > 0.2f) {
        // Large prediction errors increase impulsivity (explore more)
        pImpl->impulsivity = std::min(1.0f, pImpl->impulsivity + 0.05f);
    } else {
        // Accurate predictions reduce impulsivity (exploit)
        pImpl->impulsivity = std::max(0.1f, pImpl->impulsivity - 0.02f);
    }
}

} // namespace nlm
