#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float mood;
    float impulsivity;
    float socialBehavior;
    float rewardProcessing;
    float learningRate;
    
    Impl() : level(0.0f), mood(0.5f), impulsivity(0.5f), socialBehavior(0.0f), rewardProcessing(0.0f), learningRate(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}
Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin (5-HT)";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
    
    // 5-HT affects mood, impulsivity, and reward processing
    pImpl->mood = 0.5f + pImpl->level * 0.2f;
    pImpl->impulsivity = 0.5f - pImpl->level * 0.15f;
    pImpl->rewardProcessing = 1.0f + pImpl->level * 0.2f;
    pImpl->socialBehavior = pImpl->level * 0.3f;
    pImpl->learningRate = 1.0f + pImpl->level * 0.1f;
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT affects learning rate and reward processing
    return 1.0f + pImpl->level * 0.1f;
}

void Serotonin::update(TimestepDuration dt) {
    // Moderate decay of 5-HT signal
    pImpl->level *= 0.96f;
    
    // Apply mood and reward processing effects
    if (pImpl->level > 0.1f) {
        // 5-HT modulates mood and reward sensitivity
        // This would affect decision making and reward learning
        NLM_LOG_INFO("Serotonin level: " + std::to_string(pImpl->level) + 
                    ", mood: " + std::to_string(pImpl->mood) + 
                    ", impulsivity: " + std::to_string(pImpl->impulsivity) + 
                    ", social: " + std::to_string(pImpl->socialBehavior));
    }
}

void Serotonin::signalMoodChange(float moodShift) {
    // 5-HT modulates mood in response to positive/negative experiences
    setLevel(getLevel() + moodShift * 0.08f);
}

void Serotonin::signalRewardProcessing(float rewardValue) {
    // 5-HT modulates reward processing and learning
    setLevel(getLevel() + rewardValue * 0.02f);
    
    // Enhance reward prediction learning
    NLM_LOG_INFO("Serotonin processing reward: " + std::to_string(rewardValue) + 
                ", new level: " + std::to_string(getLevel()));
}

void Serotonin::signalSocialBehavior(float socialStrength) {
    // 5-HT modulates social behavior and bonding
    setLevel(getLevel() + socialStrength * 0.06f);
    
    // Enhance social learning and imitation
    NLM_LOG_INFO("Serotonin signaling social behavior with strength: " + std::to_string(socialStrength));
}

void Serotonin::signalLearningEvent(float predictionError, bool wasCorrect) {
    // 5-HT modulates learning based on prediction errors
    float learningSignal = wasCorrect ? 0.02f : -0.01f;
    setLevel(getLevel() + learningSignal * predictionError);
    
    NLM_LOG_INFO("Serotonin modulated learning: prediction error " + std::to_string(predictionError) + 
                ", was correct: " + std::to_string(wasCorrect) + 
                ", new level: " + std::to_string(getLevel()));
}

} // namespace nlm