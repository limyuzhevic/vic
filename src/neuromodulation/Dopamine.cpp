#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float targetLevel;
    float peak;
    float decayRate;
    float releaseRate;
    float motorExcitationFactor;
    float learningRate;
    float d1ReceptorEffect;
    float d2ReceptorEffect;
    float lastRewardTime;
    float rewardHistory[10];
    int historyIndex;
    float eligibilityTrace;
    
    static constexpr float DEFAULT_BASELINE = 0.1f;
    static constexpr float DEFAULT_PEAK = 2.0f;
    static constexpr float DEFAULT_DECAY_RATE = 0.15f;
    static constexpr float DEFAULT_RELEASE_RATE = 1.5f;
    static constexpr float DEFAULT_MOTOR_EXCITATION = 0.5f;
    static constexpr float DEFAULT_LEARNING_RATE = 0.01f;
    static constexpr float DEFAULT_D1_EFFECT = 0.8f;
    static constexpr float DEFAULT_D2_EFFECT = 0.7f;
    static constexpr size_t HISTORY_SIZE = 10;
    
    Impl() : level(DEFAULT_BASELINE), baseline(DEFAULT_BASELINE), targetLevel(DEFAULT_BASELINE),
             peak(DEFAULT_PEAK), decayRate(DEFAULT_DECAY_RATE), releaseRate(DEFAULT_RELEASE_RATE),
             motorExcitationFactor(DEFAULT_MOTOR_EXCITATION), learningRate(DEFAULT_LEARNING_RATE),
             d1ReceptorEffect(DEFAULT_D1_EFFECT), d2ReceptorEffect(DEFAULT_D2_EFFECT),
             lastRewardTime(0.0f), eligibilityTrace(0.0f), historyIndex(0) {
        for (int i = 0; i < HISTORY_SIZE; ++i) {
            rewardHistory[i] = 0.0f;
        }
    }
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Dopamine system initialized");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->targetLevel = pImpl->level;
}

float Dopamine::getPlasticityFactor() const {
    float d1Effect = pImpl->d1ReceptorEffect * pImpl->level;
    float d2Effect = pImpl->d2ReceptorEffect * (1.0f - pImpl->level);
    float netEffect = d1Effect - d2Effect;
    return 0.5f + netEffect * 0.5f;
}

void Dopamine::update(TimestepDuration dt) {
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, 
                              pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    } else {
        pImpl->level = std::min(pImpl->targetLevel,
                              pImpl->level + pImpl->decayRate * 0.5f * static_cast<float>(dt));
    }
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - 0.01f * static_cast<float>(dt));
}

float Dopamine::getBaselineLevel() const {
    return pImpl->baseline;
}

void Dopamine::setTargetLevel(float target) {
    pImpl->targetLevel = std::clamp(target, 0.0f, 1.0f);
}

bool Dopamine::isActive() const {
    return pImpl->level > pImpl->baseline + 0.05f;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->targetLevel = pImpl->baseline;
    pImpl->eligibilityTrace = 0.0f;
}

void Dopamine::signalReward(float reward) {
    if (reward > 0.0f) {
        pImpl->rewardHistory[pImpl->historyIndex] = reward;
        pImpl->historyIndex = (pImpl->historyIndex + 1) % Impl::HISTORY_SIZE;
        pImpl->lastRewardTime = 0.0f;
        
        float burstMagnitude = std::min(pImpl->peak, reward * pImpl->releaseRate);
        pImpl->level = std::max(pImpl->level + burstMagnitude, pImpl->targetLevel);
        
        pImpl->eligibilityTrace = 1.0f;
        
        NLM_LOG_INFO("Dopamine burst: reward " + std::to_string(reward) + " -> level: " + std::to_string(pImpl->level));
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    if (error > 0.0f) {
        float pepIncrease = error * pImpl->releaseRate;
        pImpl->level = std::min(pImpl->peak, pImpl->level + pepIncrease);
        pImpl->eligibilityTrace = std::max(pImpl->eligibilityTrace, error * 0.1f);
        
        NLM_LOG_INFO("Dopamine RPE: positive error " + std::to_string(error) + " -> level: " + std::to_string(pImpl->level));
    } else if (error < 0.0f) {
        float pepDecrease = std::abs(error) * pImpl->releaseRate * 0.5f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pepDecrease);
        
        NLM_LOG_INFO("Dopamine RPE: negative error " + std::to_string(error) + " -> level: " + std::to_string(pImpl->level));
    }
}

float Dopamine::getMotorExcitationFactor() const {
    return pImpl->motorExcitationFactor;
}

void Dopamine::setMotorExcitationFactor(float factor) {
    pImpl->motorExcitationFactor = std::clamp(factor, 0.0f, 1.0f);
}

void Dopamine::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, rate);
}

float Dopamine::getLearningRate() const {
    return pImpl->learningRate;
}

void Dopamine::setReceptorAffinity(float d1, float d2) {
    pImpl->d1ReceptorEffect = std::clamp(d1, 0.0f, 1.0f);
    pImpl->d2ReceptorEffect = std::clamp(d2, 0.0f, 1.0f);
}

float Dopamine::getD1ReceptorEffect() const {
    return pImpl->d1ReceptorEffect;
}

float Dopamine::getD2ReceptorEffect() const {
    return pImpl->d2ReceptorEffect;
}

} // namespace nlm
