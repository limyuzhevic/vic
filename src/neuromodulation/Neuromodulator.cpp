// Complete implementation of Dopamine class with real neuromodulation
#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float timeConstant;
    float variance;
    float expectedValue;
    float history[100]; // Rolling history for prediction error
    size_t historyIndex;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             timeConstant(100.0f), variance(1.0f), expectedValue(0.0f), historyIndex(0) {
        std::fill(std::begin(history), std::end(history), 0.0f);
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity with more sophisticated dynamics
    float baselinePlasticity = 0.5f;
    float modulationStrength = 1.0f;
    
    // Non-linear relationship between dopamine and plasticity
    float normalizedDopamine = pImpl->level / pImpl->peak;
    
    // Close to optimum: tonic dopamine for learning
    if (normalizedDopamine < Constants::DA_CENTER_OPTIMUM) {
        // Low dopamine: reduced plasticity (inhibition)
        return baselinePlasticity * (1.0f - normalizedDopamine * Constants::DA_LOW_PLASTICITY);
    } else if (normalizedDopamine < Constants::DA_SURPRISE_THRESHOLD) {
        // Moderate dopamine: optimal learning
        return baselinePlasticity * Constants::DA_OPTIMAL_PLASTICITY;
    } else {
        // High dopamine: exploratory learning boost
        return baselinePlasticity * (1.0f + (normalizedDopamine - Constants::DA_SURPRISE_THRESHOLD) * Constants::DA_HIGH_PLASTICITY);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with Ornstein-Uhlenbeck process
    float delta = dt * pImpl->decayRate;
    
    // Stochastic decay towards baseline (mean-reverting process)
    float noise = (std::rand() / (RAND_MAX + 1.0f)) * 2.0f - 1.0f;
    noise *= pImpl->variance * sqrt(delta);
    
    pImpl->level += (pImpl->baseline - pImpl->level) * delta + noise;
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    // Update expected value based on recent history
    updateExpectedValue();
}

void Dopamine::signalReward(float reward) {
    // Complex reward signaling with temporal dynamics
    // Phasic dopamine response to unexpected rewards (prediction error)
    float predictionError = reward - pImpl->expectedValue;
    
    // Phasic burst: large unexpected reward
    if (predictionError > Constants::REWARD_PREDICTION_ERROR_THRESHOLD) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + Constants::PHASIC_BURST_MAGNITUDE * predictionError);
    }
    // Phasic dip: unexpected punishment
    else if (predictionError < -Constants::REWARD_PREDICTION_ERROR_THRESHOLD) {
        pImpl->level = std::max(0.0f, pImpl->level + Constants::PHASIC_DIP_MAGNITUDE * predictionError);
    }
    // Tonic modulation: expected reward
    else {
        pImpl->level = std::min(pImpl->peak, pImpl->level + Constants::TONIC_MODULATION * reward);
    }
    
    // Store in history for prediction error calculation
    storeInHistory(predictionError);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling with temporal dynamics
    // Dopamine encodes both signed prediction error and its variance
    
    // Primary prediction error signal
    pImpl->level = std::clamp(pImpl->level + error * Constants::PREDICTION_ERROR_GAIN, 
                           0.0f, pImpl->peak);
    
    // Second-order signal: prediction error variance for exploration
    float errorVariance = calculateErrorVariance(error);
    storeInHistory(error * error);
    
    // High variance increases exploratory drive
    if (errorVariance > Constants::EXPLORATION_THRESHOLD) {
        pImpl->level += Constants::EXPLORATION_BONUS * errorVariance;
        pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    }
    
    // Store for expected value update
    storeInHistory(error);
}

// Helper methods
void Dopamine::storeInHistory(float value) {
    pImpl->history[pImpl->historyIndex] = value;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 100;
}

void Dopamine::updateExpectedValue() {
    // Update expected value based on recent history
    float total = 0.0f;
    size_t count = 0;
    
    for (size_t i = 0; i < 100; ++i) {
        if (pImpl->history[i] != 0.0f) { // Only count non-zero entries
            total += pImpl->history[i];
            count++;
        }
    }
    
    if (count > 0) {
        pImpl->expectedValue = total / static_cast<float>(count);
    } else {
        pImpl->expectedValue = 0.0f;
    }
}

float Dopamine::calculateErrorVariance(float error) const {
    // Calculate variance of prediction errors over recent history
    float mean = 0.0f;
    float sumSquares = 0.0f;
    size_t count = 0;
    
    for (size_t i = 0; i < 100; ++i) {
        if (pImpl->history[i] != 0.0f) {
            mean += pImpl->history[i];
            sumSquares += pImpl->history[i] * pImpl->history[i];
            count++;
        }
    }
    
    if (count < 2) return 0.0f;
    
    mean /= static_cast<float>(count);
    float variance = (sumSquares / static_cast<float>(count)) - mean * mean;
    
    return std::max(0.0f, variance);
}

} // namespace nlm