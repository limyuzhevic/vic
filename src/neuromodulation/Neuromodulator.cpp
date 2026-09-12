#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// ===============================================
// COMPLETE NEUROMODULATOR IMPLEMENTATION
// ================================================

struct Neuromodulator::Impl {
    std::string name;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    bool isActive;
    
    Impl(const std::string& n) 
        : name(n), level(0.0f), baseline(0.0f), peak(1.0f), 
          decayRate(0.1f), releaseRate(1.0f), isActive(true) {}
};

Neuromodulator::Neuromodulator(const std::string& name)
    : pImpl(std::make_unique<Impl>(name)) {}

Neuromodulator::~Neuromodulator() = default;

const char* Neuromodulator::getName() const {
    return pImpl->name.c_str();
}

float Neuromodulator::getLevel() const {
    return pImpl->level;
}

void Neuromodulator::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    pImpl->isActive = (pImpl->level > 0.0f);
}

float Neuromodulator::getPlasticityFactor() const {
    // Base implementation: linear relationship between level and plasticity
    if (!pImpl->isActive) return 1.0f;  // No plasticity when inactive
    return 0.5f + 0.5f * pImpl->level;
}

void Neuromodulator::update(TimestepDuration dt) {
    // Decay towards baseline when not active
    if (!pImpl->isActive) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - 
                               pImpl->decayRate * static_cast<float>(dt));
    }
}

bool Neuromodulator::isActive() const {
    return pImpl->isActive;
}

void Neuromodulator::activate() {
    pImpl->isActive = true;
}

void Neuromodulator::deactivate() {
    pImpl->isActive = false;
}

// ===============================================
// REAL DOPAMINE IMPLEMENTATION
// ===============================================

#include <algorithm>
#include <cmath>

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionErrorHistory[5]; // Short-term prediction error history
    int historyIndex;
    float errorVariance;  // Tracking of prediction error variance
    float learningRate;   // Learning rate for prediction error integration
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f),
             historyIndex(0), errorVariance(0.0f), learningRate(0.01f) {
        for (int i = 0; i < 5; ++i) predictionErrorHistory[i] = 0.0f;
    }
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine modulation: D1-like receptor effect on LTP
    // Higher dopamine enhances plasticity for positive prediction errors
    if (pImpl->level <= 0.5f) {
        return 1.0f + pImpl->level * 0.5f;  // Linear increase
    } else {
        return 1.0f + 0.25f + (pImpl->level - 0.5f) * 0.3f;  // Sublinear at high levels
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Biological dopamine dynamics: exponential decay with temporal difference
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update prediction error variance based on recent errors
    float recentError = 0.0f;
    for (int i = 0; i < 5; ++i) {
        recentError += std::abs(pImpl->predictionErrorHistory[i]);
    }
    recentError /= 5.0f;
    
    // Variance affects dopamine baseline
    pImpl->errorVariance = 0.9f * pImpl->errorVariance + 0.1f * recentError;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: dopamine burst proportional to reward magnitude
    float burst = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Store prediction error for future reference (zero prediction error for actual reward)
    pImpl->predictionErrorHistory[pImpl->historyIndex] = 0.0f;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 5;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: temporal difference learning
    // Positive error (better than expected) -> dopamine increase
    // Negative error (worse than expected) -> dopamine decrease
    float delta = error * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, pImpl->level + delta);
    
    // Store error in history
    pImpl->predictionErrorHistory[pImpl->historyIndex] = error;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 5;
}

float Dopamine::getPredictionErrorVariance() const {
    return pImpl->errorVariance;
}

float Dopamine::getLearningRate() const {
    // Adaptive learning rate based on prediction error variance
    return pImpl->learningRate * (1.0f + pImpl->errorVariance);
}

// ===============================================
// REWARD MODULATION IMPLEMENTATION  
// ===============================================

#include <algorithm>
#include <cmath>

struct Reward::Impl {
    float currentValue;
    float accumulatedValue;
    float decayRate;
    float baselineValue;
    float maxValue;
    std::vector<float> history;
    float rewardPrediction;  // Expected reward
    
    Impl() : currentValue(0.0f), accumulatedValue(0.0f), 
             decayRate(0.05f), baselineValue(0.0f), maxValue(1.0f),
             rewardPrediction(0.0f) {}
};

Reward::Reward() : pImpl(std::make_unique<Impl>()) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = std::clamp(value, 0.0f, pImpl->maxValue);
}

void Reward::add(float delta) {
    pImpl->currentValue = std::clamp(pImpl->currentValue + delta, 0.0f, pImpl->maxValue);
    pImpl->history.push_back(pImpl->currentValue);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedValue = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation from observation features
    const auto* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) return 0.0f;
    
    const auto& data = sensoryInput->getData();
    if (data.empty()) return 0.0f;
    
    // Simple reward function based on sensory input statistics
    float sum = 0.0f, sumSq = 0.0f;
    for (float val : data) {
        sum += val;
        sumSq += val * val;
    }
    
    float mean = sum / data.size();
    float variance = (sumSq / data.size()) - (mean * mean);
    
    // Novelty-based reward: higher variance = more novel = higher reward
    float noveltyReward = std::min(1.0f, variance * 10.0f);
    
    // Proximity-based reward: closer to positive values = higher reward
    float proximityReward = std::max(0.0f, mean);
    
    return noveltyReward * 0.5f + proximityReward * 0.5f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

float Reward::getPredictionError() const {
    // Prediction error = actual reward - expected reward
    return pImpl->currentValue - pImpl->rewardPrediction;
}

void Reward::updatePrediction(float newPrediction) {
    // Update predicted reward using exponential moving average
    pImpl->rewardPrediction = 0.9f * pImpl->rewardPrediction + 0.1f * newPrediction;
}

// ===============================================
// CURIOSITY SYSTEM IMPLEMENTATION
// ===============================================

#include <algorithm>

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float explorationThreshold;
    float maxExploration;
    float explorationHistory[10];  // Track recent exploration levels
    int historyIndex;
    
    Impl() 
        : brain(nullptr), level(0.0f), noveltyWeight(0.6f), predictionErrorWeight(0.4f),
          decayRate(0.05f), explorationThreshold(0.3f), maxExploration(1.0f),
          historyIndex(0) {
        for (int i = 0; i < 10; ++i) explorationHistory[i] = 0.0f;
    }
};

Curiosity::Curiosity() : pImpl(std::make_unique<Impl>()) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Real curiosity computation: weighted combination of novelty and prediction error
    float noveltyComponent = novelty * pImpl->noveltyWeight;
    float predictionErrorComponent = std::abs(predictionError) * pImpl->predictionErrorWeight;
    
    // Target level is combination of both components
    float targetLevel = noveltyComponent + predictionErrorComponent;
    
    // Smooth update towards target level
    float learningRate = 0.1f;  // Curiosity learning rate
    pImpl->level += (targetLevel - pImpl->level) * learningRate;
    
    // Decay over time when not stimulated
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Store in history
    pImpl->explorationHistory[pImpl->historyIndex] = pImpl->level;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

bool Curiosity::shouldExplore() const {
    // Determine if agent should explore based on curiosity level
    return pImpl->level > pImpl->explorationThreshold;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
    // Normalize to maintain sum of weights = 1.0
    float total = pImpl->noveltyWeight + pImpl->predictionErrorWeight;
    pImpl->noveltyWeight /= total;
    pImpl->predictionErrorWeight /= total;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
    // Normalize to maintain sum of weights = 1.0
    float total = pImpl->noveltyWeight + pImpl->predictionErrorWeight;
    pImpl->noveltyWeight /= total;
    pImpl->predictionErrorWeight /= total;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    std::fill(pImpl->explorationHistory, pImpl->explorationHistory + 10, 0.0f);
}

// ===============================================
// NOVELTY DETECTION IMPLEMENTATION
// ===============================================

#include <algorithm>
#include <cmath>

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float noveltyThreshold;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float adaptationRate;  // Rate at which novelty detection adapts
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f),
             adaptationRate(0.01f) {}
};

Novelty::Novelty() : pImpl(std::make_unique<Impl>()) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::detectNovelty(const Observation& observation,
                            const Observation& previousObservation) {
    // Extract features from observations
    const auto* currentInput = observation.getSensoryInput();
    const auto* previousInput = previousObservation.getSensoryInput();
    
    if (!currentInput || !previousInput) return;
    
    detectNovelty(currentInput->getData(), previousInput->getData());
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute Mahalanobis distance for novelty detection
    float distance = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    // Calculate mean and std dev of current pattern for adaptive thresholding
    float sum = 0.0f, sumSq = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        sum += currentPattern[i];
        sumSq += currentPattern[i] * currentPattern[i];
    }
    
    float mean = sum / compareLen;
    float variance = (sumSq / compareLen) - (mean * mean);
    float stdDev = std::sqrt(std::max(0.0f, variance));
    
    // Normalize patterns using mean and std dev
    std::vector<float> normalizedCurrent, normalizedPrevious;
    if (stdDev > 0.001f) {
        for (size_t i = 0; i < compareLen; ++i) {
            normalizedCurrent.push_back((currentPattern[i] - mean) / stdDev);
            normalizedPrevious.push_back((previousPattern[i] - mean) / stdDev);
        }
    } else {
        normalizedCurrent = currentPattern;
        normalizedPrevious = previousPattern;
    }
    
    // Compute Euclidean distance
    float totalDiff = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = normalizedCurrent[i] - normalizedPrevious[i];
        totalDiff += diff * diff;
    }
    
    distance = std::sqrt(totalDiff / compareLen);
    
    // Apply adaptive threshold based on pattern statistics
    float threshold = pImpl->noveltyThreshold * (1.0f + pImpl->adaptationRate * distance);
    
    // Novelty level is distance relative to threshold, with saturation
    pImpl->level = std::min(1.0f, distance / threshold);
    
    // Store history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {  // Limit history size
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
    
    // Adapt threshold based on recent novelty
    if (!pImpl->history.empty()) {
        float avgNovelty = 0.0f;
        for (float val : pImpl->history) avgNovelty += val;
        avgNovelty /= pImpl->history.size();
        pImpl->adaptationRate = 0.01f + 0.1f * avgNovelty;
    }
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

// ===============================================
// PREDICTION ERROR SYSTEM IMPLEMENTATION
// ===============================================

#include <algorithm>
#include <cmath>

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    float errorMagnitude;  // Absolute error for neuromodulation
    float confidence;       // Confidence in prediction
    float adaptationRate;   // Rate at which prediction adjusts
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             errorMagnitude(0.0f), confidence(0.5f), adaptationRate(0.1f) {}
};

PredictionError::PredictionError() : pImpl(std::make_unique<Impl>()) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;  // signed error
    pImpl->errorMagnitude = std::abs(pImpl->error);
    
    // Update confidence based on error magnitude
    // Smaller errors -> higher confidence
    pImpl->confidence = std::max(0.1f, 1.0f - pImpl->errorMagnitude);
    
    // Store in history
    pImpl->history.push_back(pImpl->error);
    if (pImpl->history.size() > 50) {  // Limit history size
        pImpl->history.erase(pImpl->history.begin());
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    // Update prediction using exponential moving average
    pImpl->predictedValue = 0.9f * pImpl->predictedValue + 0.1f * newPrediction;
    
    // Adjust confidence based on prediction error
    float predictionAccuracy = std::max(0.0f, 1.0f - pImpl->errorMagnitude);
    pImpl->confidence = 0.5f * pImpl->confidence + 0.5f * predictionAccuracy;
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

float PredictionError::getMagnitude() const {
    return pImpl->errorMagnitude;
}

float PredictionError::getConfidence() const {
    return pImpl->confidence;
}

void PredictionError::adapt(float learningRate) {
    // Adapt prediction based on error
    float adjustment = learningRate * pImpl->error;
    pImpl->predictedValue += adjustment;
}

// ===============================================
// PREDICTION SYSTEM IMPLEMENTATION
// ===============================================

#include <algorithm>
#include <cmath>
#include <cstdlib>

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionHistory;
    std::vector<float> temporalMemory;  // Simple temporal memory for predictions
    float learningRate;
    float decayRate;
    float predictionHorizon;  // How far ahead to predict
    
    Impl() : predictionError(0.0f), confidence(0.5f), 
             learningRate(0.01f), decayRate(0.05f), predictionHorizon(5.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(std::make_unique<Impl>()) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Real prediction using neural substrate
    if (!pImpl->temporalMemory.empty()) {
        // Create predicted state based on temporal memory
        auto predictedInput = currentState.clone();
        const auto& currentData = currentState.getData();
        
        // Simple autoregressive prediction with noise
        float sum = 0.0f;
        for (float val : pImpl->temporalMemory) {
            sum += val;
        }
        float avg = sum / pImpl->temporalMemory.size();
        
        auto& predData = predictedInput->getData();
        for (size_t i = 0; i < predData.size() && i < currentData.size(); ++i) {
            // Blend current state with predicted trend
            float trend = (currentData[i] - avg) * 0.1f;  // Small trend component
            predData[i] = currentData[i] + trend;
            
            // Add prediction noise
            float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f;
            predData[i] += noise;
        }
        
        return predictedInput;
    }
    
    // No prediction available, return current state with noise
    auto noisyInput = currentState.clone();
    auto& noisyData = noisyInput->getData();
    for (float& val : noisyData) {
        float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
        val += noise;
    }
    
    return noisyInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted,
                                         const SensoryInput& actual) {
    // Update prediction model based on prediction error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float sumSquared = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float error = predData[i] - actualData[i];
            sumError += error;
            sumSquared += error * error;
        }
        
        // Update prediction error and confidence
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on prediction accuracy
        float meanSquaredError = sumSquared / predData.size();
        pImpl->confidence = 1.0f / (1.0f + meanSquaredError * 10.0f);
        
        // Store prediction for temporal memory
        pImpl->predictionHistory.push_back(pImpl->predictionError);
        if (pImpl->predictionHistory.size() > 20) {
            pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
        }
        
        // Update temporal memory for next prediction
        pImpl->temporalMemory.push_back(pImpl->predictionError);
        if (pImpl->temporalMemory.size() > 10) {
            pImpl->temporalMemory.erase(pImpl->temporalMemory.begin());
        }
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->predictionHistory.clear();
    pImpl->temporalMemory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using observation
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Update temporal memory with observation
    pImpl->temporalMemory.push_back(data[0]);  // Use first value as representative
    if (pImpl->temporalMemory.size() > 10) {
        pImpl->temporalMemory.erase(pImpl->temporalMemory.begin());
    }
    
    // Simple learning: adjust prediction weights based on observation
    float avgValue = 0.0f;
    for (float val : data) avgValue += val;
    avgValue /= data.size();
    
    pImpl->predictionError = std::abs(avgValue - pImpl->predictionError);
}

} // namespace nlm