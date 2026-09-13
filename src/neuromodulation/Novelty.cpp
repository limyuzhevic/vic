#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;                    // Current novelty level
    float decayRate;                // Decay rate over time
    float surpriseThreshold;        // Threshold for significant surprise
    float adaptationRate;           // Rate of adaptation to changing statistics
    float bayesianTemperature;      // Controls sensitivity to prediction errors
    float contextGain;              // Weight on recent vs. long-term patterns
    
    std::vector<float> history;     // History of novelty levels
    std::vector<float> predictionHistory;  // History of predicted values
    std::vector<float> actualHistory;      // History of actual values
    std::vector<float> surpriseHistory;    // Historical surprise values
    std::vector<float> adaptationHistory;  // For tracking changing statistics
    std::vector<float> lastPattern;         // Last sensory pattern for comparison
    
    // Running statistics for change detection
    float runningMean;              // Mean of prediction errors
    float runningVariance;          // Variance of prediction errors
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.05f), surpriseThreshold(0.5f)
        , adaptationRate(0.01f), bayesianTemperature(2.0f), contextGain(0.7f)
        , runningMean(0.0f), runningVariance(0.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Biological novelty detection using Bayesian surprise
    // Model inspired by: Rao & Ballard (1999), "Predictive coding in the visual cortex"
    // Novelty is computed as Bayesian surprise: -log(P(observation|prediction))
    
    if (!pImpl->brain) return;
    
    // Extract features from observations (simplified: use observation values)
    const auto& prevData = previousObservation.getData();
    const auto& currentData = observation.getData();
    
    if (prevData.empty() || currentData.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute prediction error based on previous state
    float predictionError = 0.0f;
    size_t minSize = std::min(prevData.size(), currentData.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float diff = currentData[i] - prevData[i];
        predictionError += diff * diff;  // Sum of squared differences
    }
    
    predictionError = std::sqrt(predictionError / minSize);  // RMS error
    
    // Compute Bayesian surprise using Shannon information theory
    // Surprise = -log(P(data|prediction)) where P follows Gaussian distribution
    float variance = std::max(0.01f, pImpl->runningVariance);  // Prevent division by zero
    float surprise = 0.5f * std::log(2.0f * M_PI * variance) + 0.5f * (predictionError * predictionError) / variance;
    
    // Temperature parameter controls sensitivity
    float normalizedSurprise = std::exp(-pImpl->bayesianTemperature * surprise);
    
    // Update novelty level based on surprise
    pImpl->level = std::min(1.0f, normalizedSurprise * pImpl->contextGain);
    
    // Update statistics for changing environment detection
    updateStatistics(predictionError);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute prediction error between patterns (element-wise difference)
    float totalError = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float error = currentPattern[i] - previousPattern[i];
        totalError += error * error;  // Squared error for quadratic cost function
    }
    
    // Normalize by pattern size
    float avgError = totalError / compareLen;
    
    // Compute Bayesian surprise from prediction error
    // Surprise = -log(P(error|prediction)) where error ~ N(0, variance)
    float variance = std::max(0.01f, pImpl->runningVariance);
    float surprise = 0.5f * std::log(2.0f * M_PI * variance) + 0.5f * avgError / variance;
    
    // Temperature parameter controls sensitivity to surprising events
    float novelty = std::exp(-pImpl->bayesianTemperature * surprise);
    
    // Apply context gain (recent vs. long-term memory weighting)
    pImpl->level = std::min(1.0f, novelty * pImpl->contextGain);
    
    // Update running statistics for change detection
    updateStatistics(avgError);
    
    // Store in history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::updateStatistics(float predictionError) {
    // Update running statistics for change detection
    // Implements adaptive system identification for changing environments
    
    float alpha = 0.01f;  // Learning rate for online updates
    
    // Update running mean with exponential moving average
    pImpl->runningMean = (1.0f - alpha) * pImpl->runningMean + alpha * predictionError;
    
    // Update running variance
    float deviation = predictionError - pImpl->runningMean;
    pImpl->runningVariance = (1.0f - alpha) * pImpl->runningVariance + alpha * deviation * deviation;
    
    // Store history for later analysis
    pImpl->surpriseHistory.push_back(pImpl->level);
    if (pImpl->surpriseHistory.size() > 100) {
        pImpl->surpriseHistory.erase(pImpl->surpriseHistory.begin());
    }
    
    // Detect changing statistics - increase adaptation rate if variance is high
    if (pImpl->runningVariance > 0.1f) {
        // Environment is changing, increase adaptation
        pImpl->adaptationRate = std::min(0.05f, pImpl->adaptationRate * 1.1f);
    } else {
        // Environment is stable, decrease adaptation
        pImpl->adaptationHistory.push_back(pImpl->adaptationRate);
        if (pImpl->adaptationHistory.size() > 50) {
            pImpl->adaptationHistory.erase(pImpl->adaptationHistory.begin());
        }
        float avgAdaptation = 0.0f;
        for (float rate : pImpl->adaptationHistory) {
            avgAdaptation += rate;
        }
        avgAdaptation /= pImpl->adaptationHistory.size();
        pImpl->adaptationRate = std::max(0.001f, avgAdaptation * 0.95f);
    }
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty level over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update running statistics for environment adaptation
    updateStatistics(pImpl->level);
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
