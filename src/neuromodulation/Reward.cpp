#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include "../environment/Observation.hpp"
#include "../motor/Action.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

struct Reward::Impl {
    class Brain* brain;
    float currentValue;
    float accumulatedReward;
    float rewardPredictionError;
    float prediction;
    
    // Temporal integration
    float integrationFactor;
    float timeConstant;
    
    // Reward history
    std::vector<float> history;
    std::vector<float> predictionHistory;
    
    // Plasticity modulation
    float plasticityModulation;
    
    // Reward statistics
    float maxReward;
    float minReward;
    float averageReward;
    size_t sampleCount;
    
    // Exploration and intrinsic motivation
    float noveltyLevel;
    float curiosityLevel;
    float explorationValue;
    
    // Advanced reward components
    float intrinsicReward;
    float extrinsicReward;
    float predictionErrorDrivenReward;
    float explorationBonus;
    
    // Statistical tracking
    std::vector<float> rewardHistory;
    std::vector<float> errorHistory;
    float rewardMean;
    float rewardVariance;
    size_t updateCount;
    
    // Reward modulation parameters
    float rewardDiscountFactor;
    float errorWeight;
    float noveltyGain;
    float curiosityGain;
    
    Impl()
        : brain(nullptr)
        , currentValue(0.0f)
        , accumulatedReward(0.0f)
        , rewardPredictionError(0.0f)
        , prediction(0.0f)
        , integrationFactor(0.95f)
        , timeConstant(0.1f)
        , plasticityModulation(1.0f)
        , maxReward(0.0f)
        , minReward(0.0f)
        , averageReward(0.0f)
        , sampleCount(0)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , explorationValue(0.0f)
        , intrinsicReward(0.0f)
        , extrinsicReward(0.0f)
        , predictionErrorDrivenReward(0.0f)
        , explorationBonus(0.0f)
        , rewardMean(0.0f)
        , rewardVariance(0.0f)
        , updateCount(0)
        , rewardDiscountFactor(0.9f)
        , errorWeight(1.0f)
        , noveltyGain(0.5f)
        , curiosityGain(0.5f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

void Reward::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Reward system initialized");
}

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta, float timeStep) {
    // Temporal integration of reward with exponential decay
    if (timeStep > 0.0f) {
        // Apply exponential integration (leaky accumulator)
        float decayFactor = std::exp(-1.0f / pImpl->timeConstant * timeStep);
        pImpl->accumulatedReward = pImpl->accumulatedReward * decayFactor + delta;
    } else {
        pImpl->accumulatedReward += delta;
    }
    
    // Update current value with temporal weighting
    // Implements reward history integration
    pImpl->currentValue = pImpl->integrationFactor * pImpl->currentValue + 
                         (1.0f - pImpl->integrationFactor) * pImpl->accumulatedReward;
    
    // Update history
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update statistics for reward modulation
    pImpl->maxReward = std::max(pImpl->maxReward, pImpl->currentValue);
    pImpl->minReward = std::min(pImpl->minReward, pImpl->currentValue);
    pImpl->averageReward = pImpl->averageReward * (pImpl->sampleCount / (pImpl->sampleCount + 1.0f)) + 
                         pImpl->currentValue / (pImpl->sampleCount + 1.0f);
    pImpl->sampleCount++;
    
    // Compute reward prediction error with prediction
    // Use last prediction or zero if not available
    float prediction = pImpl->prediction;
    computeRewardPredictionError(prediction);
    
    // Update intrinsic motivation components
    updateIntrinsicMotivation();
    
    // Update statistical measures
    updateRewardStatistics();
}

float Reward::computeReward(const Observation& observation,
                           const Action& action,
                           Brain* brain) const {
    // Real reward computation from observation and action outcome
    // Integrates with brain's value prediction system
    
    float reward = 0.0f;
    
    // Check if observation is valid
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput || !brain) {
        return 0.0f;
    }
    
    // Get sensory input data
    const auto& values = sensoryInput->getData();
    
    // Check if action is valid
    if (action.getType() == ActionType::None) {
        return 0.0f;
    }
    
    // Compute reward based on action outcome and state transitions
    if (!values.empty()) {
        // Normalize sensory input to [0,1] range
        float minVal = *std::min_element(values.begin(), values.end());
        float maxVal = *std::max_element(values.begin(), values.end());
        float range = maxVal - minVal;
        if (range > 0.0f) {
            for (float& v : const_cast<std::vector<float>&>(values)) {
                v = (v - minVal) / range;
            }
        }
        
        // Compute average normalized input
        float avgInput = 0.0f;
        for (float v : values) {
            avgInput += v;
        }
        avgInput /= values.size();
        
        // Get prediction from brain's value function if available
        float prediction = 0.0f;
        if (brain->getPredictionSystem()) {
            prediction = brain->getPredictionSystem()->getCurrentPrediction();
        }
        
        // Get neuromodulation levels for integrated reward computation
        float novelty = 0.0f, curiosity = 0.0f;
        if (brain->getNovelty()) {
            novelty = brain->getNovelty()->getLevel();
        }
        if (brain->getCuriosity()) {
            curiosity = brain->getCuriosity()->getLevel();
        }
        
        // Action-specific reward computation
        // Forward movement toward high-value states
        if (action.getType() == ActionType::MoveForward) {
            // Reward for moving toward better states
            reward = avgInput * 0.6f + prediction * 0.4f;
            
            // Add bonus if moving from low to high value
            if (avgInput > 0.7f && prediction < 0.3f) {
                reward += 0.3f;
            }
        } 
        // Turning movements for exploration/bias adjustment
        else if (action.getType() == ActionType::TurnLeft || 
                action.getType() == ActionType::TurnRight) {
            // Exploration bonus weighted by uncertainty
            float uncertainty = 1.0f - std::abs(avgInput - 0.5f);
            reward = avgInput * 0.3f + uncertainty * 0.2f;
        }
        // Waiting has opportunity cost
        else if (action.getType() == ActionType::Wait) {
            reward = -0.05f;  // Small penalty for waiting
        }
        
        // Integrate with intrinsic motivation systems
        // Add exploration bonus based on novelty and curiosity
        float explorationBase = (novelty * pImpl->noveltyGain + curiosity * pImpl->curiosityGain) * 0.1f;
        reward += explorationBase;
        
        // Apply reward constraints
        reward = std::clamp(reward, -1.0f, 2.0f);
        
        // Decompose reward into components
        pImpl->extrinsicReward = std::max(0.0f, reward);  // Positive extrinsic rewards
        pImpl->intrinsicReward = (novelty * 0.3f + curiosity * 0.7f) * 0.5f; // Intrinsic motivation
        
        // Prediction error driven component
        pImpl->predictionErrorDrivenReward = std::abs(reward - prediction) * 0.2f;
    }
    
    return reward;
}

void Reward::computeRewardPredictionError(float predictedReward) {
    // Store prediction
    pImpl->prediction = predictedReward;
    pImpl->predictionHistory.push_back(predictedReward);
    if (pImpl->predictionHistory.size() > 100) {
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
    }
    
    // Compute reward prediction error using delta rule
    // RPE = actual - predicted
    float actual = pImpl->currentValue;
    pImpl->rewardPredictionError = actual - predictedReward;
    
    // Apply temporal difference learning (TD error)
    // TD error = (current reward + discount * future value) - predicted value
    float tdError = pImpl->currentValue + pImpl->accumulatedReward * pImpl->rewardDiscountFactor - predictedReward;
    
    // Apply learning rate for prediction update
    float learningRate = 0.15f;
    pImpl->prediction = predictedReward + learningRate * tdError;
    
    // Update plasticity modulation based on RPE
    // Larger absolute RPE means more plasticity for learning
    pImpl->plasticityModulation = 1.0f + std::abs(tdError) * 0.8f;
    pImpl->plasticityModulation = std::clamp(pImpl->plasticityModulation, 0.5f, 3.0f);
    
    // Store error for statistical tracking
    pImpl->errorHistory.push_back(pImpl->rewardPredictionError);
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
}

float Reward::getRewardPredictionError() const {
    return pImpl->rewardPredictionError;
}

void Reward::updateIntrinsicMotivation() {
    // Update intrinsic motivation components based on reward dynamics
    // Novelty and curiosity for exploration
    
    // Novelty from reward history variance
    float variance = 0.0f;
    if (pImpl->history.size() > 5) {
        float mean = pImpl->averageReward;
        float sumSquaredDiff = 0.0f;
        for (size_t i = 0; i < pImpl->history.size(); ++i) {
            float diff = pImpl->history[i] - mean;
            sumSquaredDiff += diff * diff;
        }
        variance = sumSquaredDiff / pImpl->history.size();
    }
    pImpl->noveltyLevel = std::min(1.0f, variance * 10.0f);
    
    // Curiosity from prediction error
    pImpl->curiosityLevel = std::min(1.0f, std::abs(pImpl->rewardPredictionError) * 2.0f);
    
    // Combined exploration value
    pImpl->explorationValue = pImpl->noveltyLevel * pImpl->noveltyGain + 
                             pImpl->curiosityLevel * pImpl->curiosityGain;
    
    // Exploration bonus for discovery
    pImpl->explorationBonus = (pImpl->noveltyLevel + pImpl->curiosityLevel) * 0.3f;
}

void Reward::updateRewardStatistics() {
    // Update statistical measures of reward
    if (pImpl->updateCount == 0) {
        pImpl->rewardMean = pImpl->currentValue;
        pImpl->rewardVariance = 0.0f;
    } else {
        // Update mean using incremental formula
        pImpl->rewardMean += (pImpl->currentValue - pImpl->rewardMean) / (pImpl->updateCount + 1);
        
        // Update variance
        if (pImpl->updateCount > 0) {
            pImpl->rewardVariance += (pImpl->currentValue - pImpl->rewardMean) * 
                                  (pImpl->currentValue - pImpl->rewardMean) - 
                                  pImpl->rewardVariance / pImpl->updateCount;
            if (pImpl->rewardVariance < 0.0f) pImpl->rewardVariance = 0.0f;
        }
    }
    
    pImpl->updateCount++;
}

float Reward::getNoveltyLevel() const {
    return pImpl->noveltyLevel;
}

float Reward::getCuriosityLevel() const {
    return pImpl->curiosityLevel;
}

float Reward::getExplorationValue() const {
    return pImpl->explorationValue;
}

float Reward::getIntrinsicReward() const {
    return pImpl->intrinsicReward;
}

float Reward::getExtrinsicReward() const {
    return pImpl->extrinsicReward;
}

float Reward::getPredictionErrorDrivenReward() const {
    return pImpl->predictionErrorDrivenReward;
}

float Reward::getExplorationBonus() const {
    return pImpl->explorationBonus;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->rewardPredictionError = 0.0f;
    pImpl->prediction = 0.0f;
    pImpl->history.clear();
    pImpl->predictionHistory.clear();
    pImpl->plasticityModulation = 1.0f;
    pImpl->maxReward = 0.0f;
    pImpl->minReward = 0.0f;
    pImpl->averageReward = 0.0f;
    pImpl->sampleCount = 0;
    pImpl->noveltyLevel = 0.0f;
    pImpl->curiosityLevel = 0.0f;
    pImpl->explorationValue = 0.0f;
    pImpl->intrinsicReward = 0.0f;
    pImpl->extrinsicReward = 0.0f;
    pImpl->predictionErrorDrivenReward = 0.0f;
    pImpl->explorationBonus = 0.0f;
    pImpl->rewardMean = 0.0f;
    pImpl->rewardVariance = 0.0f;
    pImpl->updateCount = 0;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
    pImpl->errorHistory.clear();
    pImpl->predictionHistory.clear();
}

void Reward::setIntegrationFactor(float factor) {
    pImpl->integrationFactor = std::clamp(factor, 0.5f, 0.999f);
}

void Reward::setTimeConstant(float tc) {
    pImpl->timeConstant = std::max(0.01f, tc);
}

void Reward::setRewardDiscountFactor(float factor) {
    pImpl->rewardDiscountFactor = std::clamp(factor, 0.5f, 0.99f);
}

void Reward::setErrorWeight(float weight) {
    pImpl->errorWeight = std::max(0.1f, weight);
}

void Reward::setNoveltyGain(float gain) {
    pImpl->noveltyGain = std::max(0.1f, gain);
}

void Reward::setCuriosityGain(float gain) {
    pImpl->curiosityGain = std::max(0.1f, gain);
}

float Reward::getPlasticityModulation() const {
    return pImpl->plasticityModulation;
}

float Reward::getMaxReward() const {
    return pImpl->maxReward;
}

float Reward::getMinReward() const {
    return pImpl->minReward;
}

float Reward::getAverageReward() const {
    return pImpl->averageReward;
}

float Reward::getRewardMean() const {
    return pImpl->rewardMean;
}

float Reward::getRewardVariance() const {
    return pImpl->rewardVariance;
}

size_t Reward::getUpdateCount() const {
    return pImpl->updateCount;
}

} // namespace nlm
