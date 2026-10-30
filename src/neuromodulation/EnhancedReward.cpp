#include "EnhancedReward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct EnhancedReward::Impl {
    float level;
    float baseline;
    float targetLevel;
    float predictedValue;
    float actualValue;
    float predictionError;
    float temporalDifference;
    std::vector<float> rewardHistory;
    size_t historyIndex;
    float discountFactor;
    float rewardSensitivity;
    float eligibilityTrace;
    float learningRate;
    
    static constexpr float DEFAULT_BASELINE = 0.0f;
    static constexpr float DEFAULT_PREDICTED_VALUE = 0.5f;
    static constexpr float DEFAULT_DISCOUNT_FACTOR = 0.95f;
    static constexpr float DEFAULT_REWARD_SENSITIVITY = 1.0f;
    static constexpr float DEFAULT_LEARNING_RATE = 0.01f;
    static constexpr size_t HISTORY_SIZE = 100;
    
    Impl()
        : level(DEFAULT_BASELINE)
        , baseline(DEFAULT_BASELINE)
        , targetLevel(DEFAULT_BASELINE)
        , predictedValue(DEFAULT_PREDICTED_VALUE)
        , actualValue(0.0f)
        , predictionError(0.0f)
        , temporalDifference(0.0f)
        , discountFactor(DEFAULT_DISCOUNT_FACTOR)
        , rewardSensitivity(DEFAULT_REWARD_SENSITIVITY)
        , eligibilityTrace(0.0f)
        , learningRate(DEFAULT_LEARNING_RATE)
        , historyIndex(0) {
        rewardHistory.resize(HISTORY_SIZE, 0.0f);
    }
};

EnhancedReward::EnhancedReward() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Enhanced reward system initialized");
}

EnhancedReward::~EnhancedReward() = default;

const char* EnhancedReward::getName() const {
    return "EnhancedReward";
}

float EnhancedReward::getLevel() const {
    return pImpl->level;
}

void EnhancedReward::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->targetLevel = pImpl->level;
}

float EnhancedReward::getPlasticityFactor() const {
    float tdEffect = pImpl->temporalDifference * 0.5f;
    float errorEffect = std::abs(pImpl->predictionError) * 0.3f;
    float eligibilityEffect = pImpl->eligibilityTrace * 0.2f;
    
    float totalEffect = tdEffect + errorEffect + eligibilityEffect;
    
    float plasticityFactor = 1.0f + totalEffect;
    
    return std::max(0.5f, std::min(2.0f, plasticityFactor));
}

void EnhancedReward::update(TimestepDuration dt) {
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - 0.05f * static_cast<float>(dt));
    
    if (pImpl->level > pImpl->targetLevel) {
        pImpl->level = std::max(pImpl->targetLevel, 
                              pImpl->level - 0.1f * static_cast<float>(dt));
    } else {
        pImpl->level = std::min(pImpl->targetLevel,
                              pImpl->level + 0.1f * static_cast<float>(dt));
    }
}

float EnhancedReward::getBaselineLevel() const {
    return pImpl->baseline;
}

void EnhancedReward::setTargetLevel(float target) {
    pImpl->targetLevel = std::clamp(target, 0.0f, 1.0f);
}

bool EnhancedReward::isActive() const {
    return pImpl->level > pImpl->baseline + 0.01f;
}

void EnhancedReward::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->targetLevel = pImpl->baseline;
    pImpl->eligibilityTrace = 0.0f;
    std::fill(pImpl->rewardHistory.begin(), pImpl->rewardHistory.end(), 0.0f);
}

void EnhancedReward::updatePrediction(float newPrediction, float reward, TimestepDuration dt) {
    float oldPrediction = pImpl->predictedValue;
    
    pImpl->predictionError = reward - oldPrediction;
    
    pImpl->temporalDifference = (reward + pImpl->discountFactor * newPrediction) - oldPrediction;
    
    pImpl->predictedValue = oldPrediction + pImpl->learningRate * pImpl->predictionError;
    pImpl->actualValue = reward;
    
    pImpl->eligibilityTrace = std::abs(pImpl->predictionError);
    
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % Impl::HISTORY_SIZE;
    
    NLM_LOG_INFO("Enhanced reward update: prediction " + std::to_string(oldPrediction) + 
                " -> " + std::to_string(pImpl->predictedValue) + 
                " | reward " + std::to_string(reward) + 
                " | error " + std::to_string(pImpl->predictionError));
}

float EnhancedReward::getPredictionError() const {
    return pImpl->predictionError;
}

float EnhancedReward::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

void EnhancedReward::setPredictedValue(float value) {
    pImpl->predictedValue = std::clamp(value, 0.0f, 1.0f);
}

float EnhancedReward::getPredictedValue() const {
    return pImpl->predictedValue;
}

void EnhancedReward::setActualValue(float value) {
    pImpl->actualValue = std::clamp(value, 0.0f, 1.0f);
}

float EnhancedReward::getActualValue() const {
    return pImpl->actualValue;
}

void EnhancedReward::recordReward(float reward) {
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % Impl::HISTORY_SIZE;
}

float EnhancedReward::getRecentRewardAverage(int window) const {
    if (pImpl->rewardHistory.empty()) return 0.0f;
    
    size_t start = pImpl->historyIndex;
    if (start + window > pImpl->rewardHistory.size()) {
        start = 0;
    }
    
    size_t end = std::min(start + window, pImpl->rewardHistory.size());
    
    float sum = 0.0f;
    size_t count = 0;
    
    for (size_t i = start; i < end; ++i) {
        sum += pImpl->rewardHistory[i];
        ++count;
    }
    
    return count > 0 ? sum / count : 0.0f;
}

void EnhancedReward::setDiscountFactor(float factor) {
    pImpl->discountFactor = std::clamp(factor, 0.0f, 1.0f);
}

float EnhancedReward::getDiscountFactor() const {
    return pImpl->discountFactor;
}

void EnhancedReward::setRewardSensitivity(float sensitivity) {
    pImpl->rewardSensitivity = std::max(0.0f, sensitivity);
}

float EnhancedReward::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

} // namespace nlm
