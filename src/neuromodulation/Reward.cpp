#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    float valuePrediction;        // TD value prediction
    float valuePredictionError;   // Reward prediction error (RPE)
    float eligibilityTrace;       // For credit assignment
    float decayRate;              // TD learning rate
    float explorationBonus;      // For exploration encouragement
    float temporalDecay;          // Temporal decay factor
    
    struct RewardStats {
        float mean;
        float variance;
        float max;
        float min;
        float recentAverage;
        size_t sampleCount;
    } stats;
    
    std::vector<float> history;
    std::vector<float> valueHistory;
    std::vector<float> eligibilityHistory;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f)
           , valuePrediction(0.0f), valuePredictionError(0.0f)
           , eligibilityTrace(0.0f), decayRate(0.9f)
           , explorationBonus(0.0f), temporalDecay(0.95f)
           , stats{0.0f, 0.0f, -1e10f, 1e10f, 0.0f, 0} {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->history.push_back(delta);
    
    // Update basic statistics
    pImpl->stats.sampleCount++;
    float oldMean = pImpl->stats.mean;
    pImpl->stats.mean = oldMean + (delta - oldMean) / static_cast<float>(pImpl->stats.sampleCount);
    
    float diff = delta - oldMean;
    pImpl->stats.variance = pImpl->stats.variance + diff * (delta - pImpl->stats.mean);
    
    pImpl->stats.max = std::max(pImpl->stats.max, delta);
    pImpl->stats.min = std::min(pImpl->stats.min, delta);
    
    // Update running average for trend detection
    pImpl->stats.recentAverage = (pImpl->stats.recentAverage * 0.9f) + (delta * 0.1f);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->valuePrediction = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->valuePredictionError = 0.0f;
}

void Reward::resetHistory() {
    pImpl->history.clear();
    pImpl->valueHistory.clear();
    pImpl->eligibilityHistory.clear();
    pImpl->stats.sampleCount = 0;
    pImpl->stats.mean = 0.0f;
    pImpl->stats.variance = 0.0f;
    pImpl->stats.max = -1e10f;
    pImpl->stats.min = 1e10f;
    pImpl->stats.recentAverage = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation based on observation analysis
    // This is a placeholder implementation that extracts potential reward signals
    // from observation data. In a real implementation, this would analyze:
    // - Progress toward goals
    // - Novelty/uncertainty
    // - Resource availability
    // - Prediction errors
    
    // For now, use the placeholder approach but with more sophisticated logic
    // Extract sensory input features
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) return 0.0f;
    
    const auto& data = sensoryInput->getData();
    
    // Simple reward heuristic based on input magnitude and consistency
    float baseReward = 0.0f;
    if (!data.empty()) {
        // Reward positive, meaningful inputs
        float magnitude = 0.0f;
        for (float val : data) {
            magnitude += std::abs(val);
        }
        magnitude /= static_cast<float>(data.size());
        
        // Reward if input is meaningful (not near zero)
        baseReward = std::min(1.0f, magnitude * 10.0f);
        
        // Bonus for consistent positive inputs
        float positiveRatio = 0.0f;
        for (float val : data) {
            if (val > 0.1f) positiveRatio += 1.0f;
        }
        positiveRatio /= static_cast<float>(data.size());
        
        if (positiveRatio > 0.7f) {
            baseReward *= 1.5f; // Encourage positive patterns
        }
    }
    
    // Apply temporal discounting
    float discountedReward = baseReward * pImpl->temporalDecay;
    
    // Add exploration bonus if novelty is high (would use novelty signal)
    discountedReward += pImpl->explorationBonus * 0.2f;
    
    return discountedReward;
}

float Reward::computeTemporalDifference(float currentReward, float discountFactor, float learningRate) {
    // Compute temporal difference learning update
    float tdError = currentReward - pImpl->valuePrediction;
    
    // Update value prediction
    pImpl->valuePrediction += learningRate * tdError;
    
    // Update eligibility trace
    pImpl->eligibilityTrace = discountFactor * pImpl->eligibilityTrace + 1.0f;
    
    // Store prediction error and history
    pImpl->valuePredictionError = tdError;
    pImpl->history.push_back(currentReward);
    pImpl->valueHistory.push_back(pImpl->valuePrediction);
    pImpl->eligibilityHistory.push_back(pImpl->eligibilityTrace);
    
    return tdError;
}

void Reward::updateEligibilityTrace(float traceDecay) {
    pImpl->eligibilityTrace *= traceDecay;
}

float Reward::getPredictionError() const {
    return pImpl->valuePredictionError;
}

float Reward::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

float Reward::getValuePrediction() const {
    return pImpl->valuePrediction;
}

float Reward::getMeanReward() const {
    return pImpl->stats.sampleCount > 0 ? pImpl->stats.mean : 0.0f;
}

float Reward::getRewardVariance() const {
    return pImpl->stats.sampleCount > 0 ? pImpl->stats.variance / static_cast<float>(pImpl->stats.sampleCount) : 0.0f;
}

float Reward::getMaxReward() const {
    return pImpl->stats.max;
}

float Reward::getMinReward() const {
    return pImpl->stats.min;
}

float Reward::getRecentAverage() const {
    return pImpl->stats.recentAverage;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
    pImpl->valueHistory.clear();
    pImpl->eligibilityHistory.clear();
}

} // namespace nlm
