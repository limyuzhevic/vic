#include "Reward.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <unordered_map>

namespace nlm {

struct AdvancedRewardImpl {
    // Reward computation parameters
    float currentValue;                // Current instantaneous reward
    float accumulatedReward;           // Cumulative reward received
    float predictionError;             // Reward prediction error
    float prediction;                  // Predicted reward
    float learningRate;                // TD(0) learning rate
    float eligibilityTrace;            // Eligibility trace for reward modulation
    float temporalDiscount;            // Discount factor for future rewards
    float explorationBonus;            // Bonus for novel/exploratory actions
    
    // Historical tracking
    std::vector<float> valueHistory;   // Historical reward values
    std::vector<float> predictionHistory; // Historical predictions
    std::vector<float> errorHistory;   // Historical prediction errors
    
    // Feature-based reward components
    struct FeatureWeights {
        float predictionAccuracy;       // Weight for prediction accuracy
        float explorationBonus;        // Weight for exploration
        float intrinsicMotivation;      // Weight for intrinsic motivation
        float extrinsicValue;           // Weight for extrinsic rewards
        
        FeatureWeights() {
            predictionAccuracy = 0.3f;
            explorationBonus = 0.2f;
            intrinsicMotivation = 0.2f;
            extrinsicValue = 0.3f;
        }
    } featureWeights;
    
    // State tracking
    Timestamp lastRewardTime;          // Time of last reward
    Timestamp predictionUpdateTime;    // Time of last prediction update
    float totalExperience;             // Total simulated experience
    bool predictionInitialized;         // Whether prediction has been initialized
    
    // Biological parameters
    static constexpr float MIN_REWARD = -10.0f;       // Large negative rewards
    static constexpr float MAX_REWARD = 10.0f;        // Large positive rewards
    static constexpr float DEFAULT_PREDICTION = 0.0f;
    static constexpr float DEFAULT_LEARNING_RATE = 0.1f;
    static constexpr float DEFAULT_DISCOUNT = 0.95f;
    static constexpr float DEFAULT_EXPLORATION_BONUS = 0.1f;
    static constexpr float MIN_PREDICTION_ACCURACY = 0.0f;
    static constexpr float MAX_PREDICTION_ACCURACY = 1.0f;
    
    AdvancedRewardImpl() {
        currentValue = 0.0f;
        accumulatedReward = 0.0f;
        predictionError = 0.0f;
        prediction = DEFAULT_PREDICTION;
        learningRate = DEFAULT_LEARNING_RATE;
        eligibilityTrace = 0.0f;
        temporalDiscount = DEFAULT_DISCOUNT;
        explorationBonus = DEFAULT_EXPLORATION_BONUS;
        
        lastRewardTime = 0;
        predictionUpdateTime = 0;
        totalExperience = 0.0f;
        predictionInitialized = false;
        
        // Initialize history buffers
        valueHistory.reserve(1000);
        predictionHistory.reserve(1000);
        errorHistory.reserve(1000);
        valueHistory.push_back(currentValue);
    }
};

Reward::Reward() : pImpl(new AdvancedRewardImpl()) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = std::clamp(value, AdvancedRewardImpl::MIN_REWARD, AdvancedRewardImpl::MAX_REWARD);
    
    // Record in history
    if (pImpl->valueHistory.size() >= 1000) {
        pImpl->valueHistory.erase(pImpl->valueHistory.begin());
    }
    pImpl->valueHistory.push_back(pImpl->currentValue);
}

void Reward::add(float delta) {
    // Add reward increment with proper tracking
    pImpl->currentValue += delta;
    pImpl->accumulatedReward += delta;
    pImpl->lastRewardTime = getCurrentTime();
    
    // Clamp to valid range
    setValue(pImpl->currentValue);
    
    // Record in history
    if (pImpl->valueHistory.size() >= 1000) {
        pImpl->valueHistory.erase(pImpl->valueHistory.begin());
    }
    pImpl->valueHistory.push_back(pImpl->currentValue);
}

void Reward::reset() {
    currentValue = 0.0f;
    accumulatedReward = 0.0f;
    predictionError = 0.0f;
    prediction = DEFAULT_PREDICTION;
    eligibilityTrace = 0.0f;
    
    valueHistory.clear();
    predictionHistory.clear();
    errorHistory.clear();
    
    lastRewardTime = 0;
    predictionUpdateTime = 0;
    totalExperience = 0.0f;
    predictionInitialized = false;
    
    valueHistory.push_back(currentValue);
}

float Reward::computeReward(const Observation& observation) const {
    // Advanced reward computation from observation
    // Implements multiple reward sources with biologically realistic contributions
    
    // Feature-based reward computation
    float featureBasedReward = computeFeatureBasedReward(observation);
    
    // Intrinsic motivation component
    float intrinsicReward = computeIntrinsicReward(observation);
    
    // Prediction error component
    float predictionErrorReward = computePredictionErrorReward();
    
    // Exploration bonus
    float explorationReward = computeExplorationReward(observation);
    
    // Combine with weighted sum
    float totalReward = featureBasedReward * pImpl->featureWeights.predictionAccuracy +
                       intrinsicReward * pImpl->featureWeights.intrinsicMotivation +
                       predictionErrorReward * pImpl->featureWeights.predictionAccuracy +
                       explorationReward * pImpl->featureWeights.explorationBonus;
    
    // Add external reward component
    float externalReward = observation.getExternalValue();
    totalReward += externalReward * pImpl->featureWeights.extrinsicValue;
    
    // Apply temporal discounting
    float timeSinceLastReward = getCurrentTime() - pImpl->lastRewardTime;
    float timeDiscount = std::exp(-timeSinceLastReward * 0.001f / 100.0f); // 100ms time constant
    
    return totalReward * timeDiscount;
}

void Reward::updatePrediction(float newPrediction) {
    // Temporal difference learning for reward prediction
    // Implements TD(0) algorithm for reward prediction
    
    float timestep = getCurrentTime() - pImpl->predictionUpdateTime;
    if (timestep <= 0.0f) return;
    
    // Update prediction using TD(0)
    predictionError = newPrediction - prediction;
    prediction = prediction + learningRate * predictionError;
    
    // Update eligibility trace for reward-modulated learning
    eligibilityTrace = eligibilityTrace * std::exp(-temporalDiscount * timestep) +
                     1.0f * learningRate;
    
    // Record prediction history
    if (pImpl->predictionHistory.size() >= 1000) {
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
    }
    pImpl->predictionHistory.push_back(prediction);
    
    if (pImpl->errorHistory.size() >= 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    pImpl->errorHistory.push_back(predictionError);
    
    pImpl->predictionUpdateTime = getCurrentTime();
    predictionInitialized = true;
}

float Reward::getPredictionError() const {
    return predictionError;
}

float Reward::getPrediction() const {
    return prediction;
}

void Reward::setExplorationBonus(float bonus) {
    explorationBonus = std::clamp(bonus, 0.0f, 1.0f);
}

void Reward::setLearningRate(float rate) {
    learningRate = std::clamp(rate, 0.01f, 0.5f);
}

void Reward::setTemporalDiscount(float discount) {
    temporalDiscount = std::clamp(discount, 0.5f, 1.0f);
}

float Reward::getAccumulatedReward() const {
    return accumulatedReward;
}

float Reward::getPredictionAccuracy() const {
    if (pImpl->predictionHistory.empty()) return 0.0f;
    
    // Compute prediction accuracy as inverse of error magnitude
    float totalSquaredError = 0.0f;
    float totalValue = 0.0f;
    
    for (size_t i = 1; i < pImpl->errorHistory.size(); ++i) {
        float error = pImpl->errorHistory[i];
        float actual = pImpl->valueHistory[i];
        totalSquaredError += error * error;
        totalValue += actual * actual;
    }
    
    if (totalValue == 0.0f) return MIN_PREDICTION_ACCURACY;
    
    float mse = totalSquaredError / static_cast<float>(pImpl->errorHistory.size());
    float mae = sqrt(mse);
    
    return std::max(MIN_PREDICTION_ACCURACY, std::min(MAX_PREDICTION_ACCURACY, 1.0f - mae));
}

// Private helper methods
float Reward::computeFeatureBasedReward(const Observation& observation) const {
    // Compute reward based on observation features
    // In real implementation, this would extract features from the observation
    // and compute a value based on those features
    
    float reward = 0.0f;
    
    // Extract features from observation
    auto features = observation.getFeatures();
    
    // Compute feature-based contribution
    for (const auto& feature : features) {
        reward += feature.value * feature.weight;
    }
    
    // Add novelty bonus if observation is novel
    reward += getNoveltyBonus(observation);
    
    return reward;
}

float Reward::computeIntrinsicReward(const Observation& observation) const {
    // Intrinsic motivation component based on curiosity and exploration
    float intrinsicReward = 0.0f;
    
    // Based on observation complexity and novelty
    float complexity = observation.getComplexity();
    float novelty = observation.getNovelty();
    
    intrinsicReward += complexity * 0.5f;
    intrinsicReward += novelty * 0.5f;
    
    // Add surprise-based component
    intrinsicReward += computeSurprise(observation);
    
    return intrinsicReward;
}

float Reward::computePredictionErrorReward() const {
    // Reward prediction error as a reinforcement signal
    return predictionError * learningRate;
}

float Reward::computeExplorationReward(const Observation& observation) const {
    // Bonus for exploring novel states
    return explorationBonus * observation.getNovelty();
}

float Reward::getNoveltyBonus(const Observation& observation) const {
    // Compute novelty bonus based on observation comparison
    // This would typically use the Novelty system
    
    float novelty = observation.getNovelty();
    
    // Non-linear scaling of novelty bonus
    return explorationBonus * (1.0f - std::exp(-novelty * 5.0f));
}

float Reward::computeSurprise(const Observation& observation) const {
    // Compute surprise based on prediction error
    float prediction = observation.getPredictedValue();
    float actual = observation.getActualValue();
    
    float surprise = std::abs(prediction - actual);
    
    // Scale surprise appropriately
    return surprise * 0.1f;
}

Timestamp Reward::getCurrentTime() const {
    // Get current simulation time
    // In real implementation, this would interface with simulation clock
    return static_cast<Timestamp>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
}

} // namespace nlm
