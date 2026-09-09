#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include "../environment/Observation.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float integrationFactor;
    float surpriseFactor;
    float noveltyThreshold;
    
    // Novelty detection
    std::vector<float> history;
    std::vector<float> lastPattern;
    std::vector<float> patternHistory;
    
    // Prediction error based novelty
    float predictionError;
    float surprise;
    float predictionAccumulator;
    
    // Change detection
    std::vector<float> changeHistory;
    float changeThreshold;
    
    // Exploration value
    float explorationValue;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , decayRate(0.1f)
        , integrationFactor(0.95f)
        , surpriseFactor(1.0f)
        , noveltyThreshold(0.3f)
        , predictionError(0.0f)
        , surprise(0.0f)
        , predictionAccumulator(0.0f)
        , changeThreshold(0.2f)
        , explorationValue(0.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection system initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::detectNovelty(const Observation& observation, 
                           const Observation& previousObservation) {
    // Extract features from observations and compare
    const SensoryInput* currentInput = observation.getSensoryInput();
    const SensoryInput* previousInput = previousObservation.getSensoryInput();
    
    if (!currentInput || !previousInput) {
        setLevel(0.0f);
        return;
    }
    
    // Convert observations to feature vectors
    std::vector<float> currentPattern;
    std::vector<float> previousPattern;
    
    // Simple feature extraction: use sensory input values directly
    const auto& currentValues = currentInput->getData();
    const auto& previousValues = previousInput->getData();
    
    currentPattern.assign(currentValues.begin(), currentValues.end());
    previousPattern.assign(previousValues.begin(), previousValues.end());
    
    // Detect novelty using pattern comparison
    detectNovelty(currentPattern, previousPattern);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        setLevel(0.0f);
        return;
    }
    
    // Compute prediction error (difference between predicted and actual)
    // In real implementation, this would use the brain's prediction system
    float predictionError = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    if (compareLen > 0) {
        // Compute mean squared difference as prediction error
        float sumSquaredDiff = 0.0f;
        for (size_t i = 0; i < compareLen; ++i) {
            float diff = currentPattern[i] - previousPattern[i];
            sumSquaredDiff += diff * diff;
        }
        predictionError = std::sqrt(sumSquaredDiff / compareLen);
        
        // Store prediction error
        pImpl->predictionError = predictionError;
        pImpl->predictionAccumulator = 
            pImpl->integrationFactor * pImpl->predictionAccumulator + 
            (1.0f - pImpl->integrationFactor) * predictionError;
    }
    
    // Compute surprise from prediction error
    pImpl->surprise = computeSurprise(predictionError);
    
    // Detect change (significant difference from baseline)
    float change = 0.0f;
    if (!pImpl->lastPattern.empty()) {
        change = std::abs(predictionError - pImpl->changeHistory.empty() ? 
                        0.0f : pImpl->changeHistory.back());
        pImpl->changeHistory.push_back(change);
        if (pImpl->changeHistory.size() > 100) {
            pImpl->changeHistory.erase(pImpl->changeHistory.begin());
        }
    }
    
    // Update novelty level based on prediction error and surprise
    float noveltyFromPE = std::min(1.0f, predictionError / pImpl->noveltyThreshold);
    float noveltyFromSurprise = pImpl->surprise * pImpl->surpriseFactor;
    float noveltyFromChange = std::min(1.0f, change / pImpl->changeThreshold);
    
    // Combine different sources of novelty
    pImpl->level = (noveltyFromPE + noveltyFromSurprise + noveltyFromChange) / 3.0f;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Update exploration value
    pImpl->explorationValue = getExplorationValue();
    
    // Store pattern for next comparison
    pImpl->lastPattern = currentPattern;
    pImpl->patternHistory.push_back(predictionError);
    if (pImpl->patternHistory.size() > 1000) {
        pImpl->patternHistory.erase(pImpl->patternHistory.begin());
    }
    
    // Add to novelty history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time
    if (dt > 0.0) {
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    }
    
    // Update prediction accumulator
    pImpl->predictionAccumulator = 
        pImpl->integrationFactor * pImpl->predictionAccumulator;
}

float Novelty::computeSurprise(float predictionError) const {
    // Surprise is higher when prediction error is large relative to expectations
    // Based on information theory: surprise = -log(probability of observation)
    // Higher prediction error means lower probability, thus higher surprise
    
    float normalizedPE = std::min(2.0f, predictionError);  // Cap at reasonable value
    
    // Surprise function: exponential growth with prediction error
    float surprise = 1.0f - std::exp(-pImpl->surpriseFactor * normalizedPE);
    
    return std::clamp(surprise, 0.0f, 1.0f);
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->patternHistory.clear();
    pImpl->changeHistory.clear();
    pImpl->predictionError = 0.0f;
    pImpl->surprise = 0.0f;
    pImpl->predictionAccumulator = 0.0f;
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::max(0.01f, threshold);
}

void Novelty::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.01f, rate);
}

void Novelty::setIntegrationFactor(float factor) {
    pImpl->integrationFactor = std::clamp(factor, 0.5f, 0.999f);
}

void Novelty::setSurpriseFactor(float factor) {
    pImpl->surpriseFactor = std::max(0.1f, factor);
}

float Novelty::getExplorationValue() const {
    // Exploration value based on novelty, surprise, and prediction error
    float noveltyComponent = pImpl->level;
    float surpriseComponent = pImpl->surprise;
    float predictionErrorComponent = std::min(1.0f, std::abs(pImpl->predictionError));
    
    // Weighted combination
    pImpl->explorationValue = 
        noveltyComponent * 0.4f + 
        surpriseComponent * 0.4f + 
        predictionErrorComponent * 0.2f;
    
    return pImpl->explorationValue;
}

float Novelty::getExplorationValue() const {
    return pImpl->explorationValue;
}

} // namespace nlm
