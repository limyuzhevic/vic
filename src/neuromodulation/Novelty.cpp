#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    float learningRate;
    float minSimilarity;  // Minimum similarity to consider familiar
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f),
             learningRate(0.05f), minSimilarity(0.95f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized");
    reset();
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Compute novelty based on changes in observation features
    // Simple implementation: just set to a placeholder
    pImpl->level = 1.0f;
    pImpl->history.push_back(pImpl->level);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
    
    // Compute novelty using multiple metrics:
    // 1. Simple difference (Euclidean distance)
    // 2. Correlation coefficient
    // 3. Mean absolute change
    
    float noveltyScore = computeNoveltyScore(currentPattern, previousPattern);
    pImpl->level = std::min(1.0f, noveltyScore);
    
    // Apply learning to adapt to new patterns
    pImpl->noveltyThreshold *= (1.0f - pImpl->learningRate);
    
    pImpl->history.push_back(pImpl->level);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            float previousPatternAverage,
                            float previousPatternVariance) {
    if (currentPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute statistical novelty
    float currentMean = std::accumulate(currentPattern.begin(), 
                                        currentPattern.end(), 0.0f) / currentPattern.size();
    
    float currentVariance = 0.0f;
    for (float value : currentPattern) {
        currentVariance += (value - currentMean) * (value - currentMean);
    }
    currentVariance /= currentPattern.size();
    
    // Compute Mahalanobis distance (statistical novelty)
    float mahalanobis = std::sqrt(std::abs(currentMean - previousPatternAverage) / 
                                 (previousPatternVariance + 0.01f));
    
    // Also consider variance change
    float varianceChange = std::abs(currentVariance - previousPatternVariance) / 
                          (previousPatternVariance + 0.01f);
    
    // Combine metrics
    float noveltyScore = 0.5f * mahalanobis + 0.5f * varianceChange;
    pImpl->level = std::min(1.0f, noveltyScore);
    
    pImpl->history.push_back(pImpl->level);
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

void Novelty::reset() {
    pImpl->level = 0.0f;
    pImpl->history.clear();
    pImpl->lastPattern.clear();
}

float Novelty::computeNoveltyScore(const std::vector<float>& current,
                                   const std::vector<float>& previous) {
    if (current.size() != previous.size()) {
        // If patterns have different sizes, compute on minimum length
        size_t minSize = std::min(current.size(), previous.size());
        if (minSize == 0) return 0.0f;
        
        float totalDiff = 0.0f;
        for (size_t i = 0; i < minSize; ++i) {
            float diff = std::abs(current[i] - previous[i]);
            totalDiff += diff;
        }
        return totalDiff / minSize;
    }
    
    // Compute multiple novelty metrics
    
    // 1. Euclidean distance (simple difference)
    float euclidean = 0.0f;
    for (size_t i = 0; i < current.size(); ++i) {
        float diff = current[i] - previous[i];
        euclidean += diff * diff;
    }
    euclidean = std::sqrt(euclidean);
    
    // 2. Correlation coefficient
    float currentMean = std::accumulate(current.begin(), current.end(), 0.0f) / current.size();
    float previousMean = std::accumulate(previous.begin(), previous.end(), 0.0f) / previous.size();
    
    float currentStd = 0.0f, previousStd = 0.0f;
    for (size_t i = 0; i < current.size(); ++i) {
        currentStd += (current[i] - currentMean) * (current[i] - currentMean);
        previousStd += (previous[i] - previousMean) * (previous[i] - previousMean);
    }
    currentStd = std::sqrt(currentStd / current.size());
    previousStd = std::sqrt(previousStd / previous.size());
    
    float covariance = 0.0f;
    for (size_t i = 0; i < current.size(); ++i) {
        covariance += (current[i] - currentMean) * (previous[i] - previousMean);
    }
    covariance /= current.size();
    
    float correlation = 0.0f;
    if (currentStd > 0.0f && previousStd > 0.0f) {
        correlation = covariance / (currentStd * previousStd);
        correlation = std::abs(correlation);  // Use absolute value
    }
    
    // 3. Mean absolute change
    float meanAbsChange = 0.0f;
    for (size_t i = 0; i < current.size(); ++i) {
        meanAbsChange += std::abs(current[i] - previous[i]);
    }
    meanAbsChange /= current.size();
    
    // Combine metrics with adaptive weights
    float weight1 = 0.4f;  // Euclidean
    float weight2 = 0.3f;  // Correlation
    float weight3 = 0.3f;  // Mean absolute change
    
    // Normalize each metric to [0, 1]
    float euclideanNorm = std::min(1.0f, euclidean * 10.0f);  // Scale factor
    float correlationNorm = 1.0f - correlation;  // Higher novelty if less correlated
    float changeNorm = std::min(1.0f, meanAbsChange * 5.0f);  // Scale factor
    
    float novelty = weight1 * euclideanNorm + weight2 * correlationNorm + weight3 * changeNorm;
    
    return novelty;
}

} // namespace nlm
