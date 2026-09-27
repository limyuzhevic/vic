#include "Serotonin.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>

namespace nlm {

struct Serotonin::Impl {
    float predictionErrorSensitivity;
    float rewardSensitivity;
    float statusSensitivity;
    
    // Behavioral state
    float impulsivityLevel;
    float deliberationTime;
    
    // Learning state
    float learningRateModifier;
    float decisionStability;
    float explorationDrive;
    float statusMotivation;
    
    // Physiological parameters
    float adaptationRate;
    float homeostaticRate;
    
    // Historical averages
    float avgPredictionError;
    float avgReward;
    float avgStatus;
    
    Impl() : predictionErrorSensitivity(0.6f)
        , rewardSensitivity(0.4f)
        , statusSensitivity(0.3f)
        , impulsivityLevel(0.5f)
        , deliberationTime(1.0f)
        , learningRateModifier(1.0f)
        , decisionStability(1.0f)
        , explorationDrive(0.5f)
        , statusMotivation(0.5f)
        , adaptationRate(0.05f)
        , homeostaticRate(0.01f)
        , avgPredictionError(0.0f)
        , avgReward(0.0f)
        , avgStatus(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl)
    , brain_(nullptr)
    , currentLevel_(0.0f)
    , learningRateModifier_(1.0f)
    , decisionStability_(1.0f)
    , explorationDrive_(0.5f)
    , statusMotivation_(0.5f)
    , currentStatus_(0.5f)
    , statusTrend_(0.0f)
    , impulsivityLevel_(0.5f)
    , deliberationTime_(1.0f)
{
}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    brain_ = brain;
    predictionErrorHistory_.clear();
    rewardHistory_.clear();
    rewardHistoryWithTime_.clear();
    predictionErrorHistoryWithTime_.clear();
    pastRewards_.clear();
    pastPredictionErrors_.clear();
    
    // Initialize historical averages
    pImpl->avgPredictionError = 0.0f;
    pImpl->avgReward = 0.0f;
    pImpl->avgStatus = 0.0f;
    
    NLM_LOG_INFO("Serotonin (5-HT) initialized - learning and decision-making system ready");
}

float Serotonin::getLevel() const {
    return currentLevel_;
}

void Serotonin::setLevel(float level) {
    currentLevel_ = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getLearningEnhancement() const {
    // Serotonin enhances learning proportionally to level
    return 1.0f + currentLevel_ * 1.2f;  // 1.0x to 2.2x enhancement
}

float Serotonin::getDecisionStability() const {
    // Serotonin promotes decision stability
    return 1.0f + currentLevel_ * 0.8f;  // 1.0x to 1.8x stability
}

void Serotonin::update(float predictionError, float reward, TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Record history
    predictionErrorHistory_.push_back(predictionError);
    rewardHistory_.push_back(reward);
    
    // Keep history at reasonable size
    const size_t MAX_HISTORY = 100;
    if (predictionErrorHistory_.size() > MAX_HISTORY) {
        predictionErrorHistory_.erase(predictionErrorHistory_.begin());
    }
    if (rewardHistory_.size() > MAX_HISTORY) {
        rewardHistory_.erase(rewardHistory_.begin());
    }
    
    // Update prediction error average
    if (!predictionErrorHistory_.empty()) {
        float sum = std::accumulate(predictionErrorHistory_.begin(), predictionErrorHistory_.end(), 0.0f);
        pImpl->avgPredictionError = sum / predictionErrorHistory_.size();
    }
    
    // Update reward average
    if (!rewardHistory_.empty()) {
        float sum = std::accumulate(rewardHistory_.begin(), rewardHistory_.end(), 0.0f);
        pImpl->avgReward = sum / rewardHistory_.size();
    }
    
    // Compute serotonergic response based on inputs
    float predictionErrorResponse = (predictionError - pImpl->avgPredictionError) * pImpl->predictionErrorSensitivity;
    float rewardResponse = (reward - pImpl->avgReward) * pImpl->rewardSensitivity;
    
    // Social status component (based on reward and prediction error)
    float statusChange = (rewardResponse - std::abs(predictionErrorResponse)) * pImpl->statusSensitivity;
    pImpl->statusTrend_ += statusChange;
    pImpl->statusTrend_ = std::max(-0.1f, std::min(0.1f, pImpl->statusTrend_)); // Clamp trend changes
    
    // Update current level based on integrated signals
    float integratedSignal = std::abs(predictionErrorResponse + rewardResponse + statusChange * 0.5f);
    setLevel(std::min(1.0f, currentLevel_ + integratedSignal * pImpl->adaptationRate * dtFloat));
    
    // Update behavioral parameters
    updateBehavioralParameters(dtFloat);
    
    // Update learning modulation
    learningRateModifier_ = computeLearningRateModifier();
    decisionStability_ = computeDecisionStability();
    explorationDrive_ = computeExplorationDrive();
    statusMotivation_ = computeStatusMotivation();
}

void Serotonin::signalPredictionError(float error) {
    predictionErrorHistory_.push_back(error);
    if (predictionErrorHistory_.size() > 100) {
        predictionErrorHistory_.erase(predictionErrorHistory_.begin());
    }
    
    // Large prediction errors trigger strong serotonergic responses
    if (std::abs(error) > 0.5f) {
        float response = std::min(1.0f, std::abs(error) * 0.8f);
        setLevel(std::min(1.0f, currentLevel_ + response * 0.3f));
        NLM_LOG_INFO("Serotonin: Large prediction error detected (" + std::to_string(error) + ")");
    }
}

void Serotonin::signalReward(float reward) {
    rewardHistory_.push_back(reward);
    if (rewardHistory_.size() > 100) {
        rewardHistory_.erase(rewardHistory_.begin());
    }
    
    // Positive rewards boost serotonin
    if (reward > 0.0f) {
        float response = std::min(0.5f, reward * 0.3f);
        setLevel(std::min(1.0f, currentLevel_ + response * 0.2f));
    } else if (reward < 0.0f) {
        // Negative rewards reduce serotonin (stress response)
        float response = std::min(0.5f, std::abs(reward) * 0.4f);
        setLevel(std::max(0.0f, currentLevel_ - response * 0.3f));
    }
}

void Serotonin::signalSocialStatus(float statusChange) {
    currentStatus_ += statusChange;
    currentStatus_ = std::clamp(currentStatus_, 0.0f, 1.0f);
    
    // Status changes affect serotonin levels
    float response = std::abs(statusChange) * 0.5f;
    setLevel(std::min(1.0f, currentStatus_ + response * 0.2f));
    
    NLM_LOG_INFO("Serotonin: Social status changed to " + std::to_string(currentStatus_));
}

float Serotonin::getLearningModulation(class Synapse* synapse) const {
    if (!synapse) return 1.0f;
    
    float modulation = learningRateModifier_;
    
    // Adapt learning based on recent prediction errors
    if (!predictionErrorHistory_.empty()) {
        float recentError = predictionErrorHistory_.back();
        modulation *= (1.0f + std::abs(recentError) * 0.5f);
    }
    
    return modulation;
}

float Serotonin::getBehavioralFlexibility() const {
    // Flexibility increases with serotonin level (up to a point)
    return 0.5f + currentLevel_ * 0.8f;  // 0.5x to 1.3x flexibility
}

float Serotonin::getImpulsivityReduction() const {
    // Serotonin reduces impulsivity
    return 1.0f + currentLevel_ * 1.2f;  // 1.0x to 2.2x reduction
}

float Serotonin::getRewardValuation(class SimulationStep step) const {
    // Reward valuation based on recent history
    if (rewardHistoryWithTime_.empty()) return 0.5f;
    
    float totalWeight = 0.0f;
    float weightedSum = 0.0f;
    
    for (const auto& entry : rewardHistoryWithTime_) {
        float age = static_cast<float>(step) - static_cast<float>(entry.first);
        float weight = std::exp(-age * 0.01f);  // Exponential decay
        totalWeight += weight;
        weightedSum += entry.second * weight;
    }
    
    if (totalWeight > 0.0f) {
        return weightedSum / totalWeight;
    }
    
    return 0.5f;
}

void Serotonin::reset() {
    currentLevel_ = 0.0f;
    learningRateModifier_ = 1.0f;
    decisionStability_ = 1.0f;
    explorationDrive_ = 0.5f;
    statusMotivation_ = 0.5f;
    currentStatus_ = 0.5f;
    statusTrend_ = 0.0f;
    impulsivityLevel_ = 0.5f;
    deliberationTime_ = 1.0f;
    
    predictionErrorHistory_.clear();
    rewardHistory_.clear();
    rewardHistoryWithTime_.clear();
    predictionErrorHistoryWithTime_.clear();
    pastRewards_.clear();
    pastPredictionErrors_.clear();
    
    pImpl->avgPredictionError = 0.0f;
    pImpl->avgReward = 0.0f;
    pImpl->avgStatus = 0.5f;
    
    NLM_LOG_INFO("Serotonin reset complete");
}

void Serotonin::updateBehavioralParameters(float dt) {
    // Update impulsivity based on current state
    float targetImpulsivity = 0.5f - currentLevel_ * 0.3f;  // Higher serotonin = lower impulsivity
    targetImpulsivity = std::clamp(targetImpulsivity, 0.1f, 0.9f);
    
    impulsivityLevel_ += (targetImpulsivity - impulsivityLevel_) * pImpl->adaptationRate * dt;
    
    // Update deliberation time
    float targetDeliberation = 0.5f + currentLevel_ * 0.8f;  // Higher serotonin = more deliberation
    targetDeliberation = std::clamp(targetDeliberation, 0.5f, 2.0f);
    
    deliberationTime_ += (targetDeliberation - deliberationTime_) * pImpl->adaptationRate * dt;
    
    // Homeostatic regulation
    float homeostaticDrift = (0.5f - currentLevel_) * pImpl->homeostaticRate * dt;
    setLevel(std::clamp(currentLevel_ + homeostaticDrift, 0.0f, 1.0f));
}

float Serotonin::computeLearningRateModifier() {
    // Learning rate modulation based on prediction errors
    float errorMagnitude = std::abs(pImpl->avgPredictionError);
    float baseLearning = 1.0f;
    
    // Optimal learning occurs at moderate prediction error levels
    if (errorMagnitude < 0.3f) {
        return baseLearning * (1.0f + errorMagnitude * 2.0f);
    } else if (errorMagnitude < 0.7f) {
        return baseLearning * 1.5f;
    } else {
        return baseLearning * (1.0f - (errorMagnitude - 0.7f) * 1.5f);
    }
}

float Serotonin::computeDecisionStability() {
    // Decision stability increases with serotonin level
    return 1.0f + currentLevel_ * 0.8f * (1.0f + currentStatus_ * 0.3f);
}

float Serotonin::computeExplorationDrive() {
    // Exploration drive is modulated by serotonin and recent rewards
    float rewardFactor = (pImpl->avgReward > 0.0f) ? std::max(0.0f, 1.0f - pImpl->avgReward * 2.0f) : 1.0f;
    return currentLevel_ * 0.6f * rewardFactor * (1.0f + statusMotivation_ * 0.2f);
}

float Serotonin::computeStatusMotivation() {
    // Status motivation increases with current status
    return currentStatus_ * (1.0f + statusTrend_);
}

} // namespace nlm