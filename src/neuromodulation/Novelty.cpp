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
    float maximumNovelty;
    float learningRate;
    float temporalDecay;
    float featureSensitivity;
    
    // For statistical novelty detection
    std::vector<float> recentFeatures;
    float meanNovelty;
    float stdDevNovelty;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , decayRate(0.1f)
        , noveltyThreshold(0.3f)
        , maximumNovelty(1.0f)
        , learningRate(0.2f)
        , temporalDecay(0.95f)
        , featureSensitivity(1.0f)
        , meanNovelty(0.0f)
        , stdDevNovelty(0.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {
    NLM_LOG_INFO("Novelty detection system initialized with enhanced feature analysis");
}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->recentFeatures.clear();
    pImpl->recentFeatures.reserve(100);
    NLM_LOG_INFO("Novelty detection system initialized with enhanced feature analysis");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->maximumNovelty);
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Enhanced novelty detection from observations
    
    // Extract features from observations
    std::vector<float> currentFeatures = extractFeatures(observation);
    std::vector<float> previousFeatures = extractFeatures(previousObservation);
    
    // Compute novelty based on feature changes
    float novelty = computeFeatureNovelty(currentFeatures, previousFeatures);
    
    // Update novelty level with temporal dynamics
    pImpl->level = std::clamp(novelty, 0.0f, pImpl->maximumNovelty);
    
    // Store features for statistical analysis
    pImpl->recentFeatures.push_back(pImpl->level);
    if (pImpl->recentFeatures.size() > 100) {
        pImpl->recentFeatures.erase(pImpl->recentFeatures.begin());
    }
    
    // Update statistics
    updateStatistics();
    
    // Decay over time
    pImpl->level *= pImpl->temporalDecay;
    
    // Push to history
    pImpl->history.push_back(pImpl->level);
    
    // Keep history size manageable
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    pImpl->lastPattern = currentFeatures;
    
    NLM_LOG_DEBUG(std::string("Novelty detected: ") + std::to_string(novelty));
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Enhanced pattern matching with multiple similarity metrics
    float patternNovelty = computePatternNovelty(currentPattern, previousPattern);
    float statisticalNovelty = computeStatisticalNovelty(currentPattern);
    
    // Combine novelty measures
    float combinedNovelty = patternNovelty * 0.7f + statisticalNovelty * 0.3f;
    
    // Apply learning dynamics
    pImpl->level += (combinedNovelty - pImpl->level) * pImpl->learningRate;
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->maximumNovelty);
    
    // Store in history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    pImpl->lastPattern = currentPattern;
    
    // Update statistics
    pImpl->recentFeatures.push_back(pImpl->level);
    if (pImpl->recentFeatures.size() > 100) {
        pImpl->recentFeatures.erase(pImpl->recentFeatures.begin());
    }
    updateStatistics();
}

void Novelty::update(TimestepDuration dt) {
    // Time-based decay with adaptive rate
    float decay = pImpl->decayRate * static_cast<float>(dt);
    
    // Novelty decays over time, but slower for highly novel experiences
    float decayFactor = 1.0f - (pImpl->level * decay * 0.5f);
    pImpl->level *= decayFactor;
    
    // Update statistics over time
    updateStatistics();
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->recentFeatures.clear();
    pImpl->meanNovelty = 0.0f;
    pImpl->stdDevNovelty = 0.0f;
}

// Helper methods
std::vector<float> Novelty::extractFeatures(const Observation& observation) const {
    std::vector<float> features;
    
    // Extract various features from observation
    // This would depend on the specific observation structure
    
    // For now, extract some basic features
    features.reserve(32);
    
    // Placeholder feature extraction - real implementation would analyze observation
    // This could include visual features, spatial patterns, temporal changes, etc.
    
    for (size_t i = 0; i < 32; ++i) {
        // Generate a placeholder feature value based on observation content
        float feature = 0.0f;
        // In a real implementation, this would extract meaningful features
        features.push_back(feature);
    }
    
    return features;
}

float Novelty::computeFeatureNovelty(const std::vector<float>& current, 
                                     const std::vector<float>& previous) const {
    if (current.empty() || previous.empty() || current.size() != previous.size()) {
        return 0.0f;
    }
    
    float totalDifference = 0.0f;
    for (size_t i = 0; i < current.size(); ++i) {
        totalDifference += std::abs(current[i] - previous[i]);
    }
    
    // Normalize by number of features
    float avgDifference = totalDifference / current.size();
    
    // Apply sensitivity factor
    return std::min(avgDifference * pImpl->featureSensitivity, pImpl->maximumNovelty);
}

float Novelty::computePatternNovelty(const std::vector<float>& current,
                                     const std::vector<float>& previous) const {
    if (current.empty() || previous.empty()) {
        return 0.0f;
    }
    
    size_t compareLen = std::min(current.size(), previous.size());
    
    // Compute multiple difference metrics
    float euclideanDiff = 0.0f;
    float manhattanDiff = 0.0f;
    float correlationDiff = 0.0f;
    
    for (size_t i = 0; i < compareLen; ++i) {
        euclideanDiff += (current[i] - previous[i]) * (current[i] - previous[i]);
        manhattanDiff += std::abs(current[i] - previous[i]);
    }
    
    euclideanDiff = std::sqrt(euclideanDiff);
    
    // Normalize differences
    float normalizedEuclidean = euclideanDiff / std::sqrt(compareLen);
    float normalizedManhattan = manhattanDiff / compareLen;
    
    // Combine metrics
    float combined = (normalizedEuclidean + normalizedManhattan) * 0.5f;
    
    return std::min(combined, pImpl->maximumNovelty);
}

float Novelty::computeStatisticalNovelty(const std::vector<float>& pattern) const {
    if (pattern.empty()) {
        return 0.0f;
    }
    
    // Compute mean and standard deviation of pattern
    float sum = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
    float mean = sum / pattern.size();
    
    float sqSum = 0.0f;
    for (float value : pattern) {
        sqSum += (value - mean) * (value - mean);
    }
    float stdDev = std::sqrt(sqSum / pattern.size());
    
    // Statistical novelty is based on how different this pattern is from
    // the historical mean and standard deviation
    float statisticalNovelty = std::abs(mean - pImpl->meanNovelty) + 
                               std::abs(stdDev - pImpl->stdDevNovelty);
    
    return std::clamp(statisticalNovelty, 0.0f, pImpl->maximumNovelty);
}

void Novelty::updateStatistics() {
    if (pImpl->recentFeatures.empty()) {
        return;
    }
    
    // Update mean
    float sum = std::accumulate(pImpl->recentFeatures.begin(), 
                                pImpl->recentFeatures.end(), 0.0f);
    pImpl->meanNovelty = sum / pImpl->recentFeatures.size();
    
    // Update standard deviation
    float sqSum = 0.0f;
    for (float value : pImpl->recentFeatures) {
        sqSum += (value - pImpl->meanNovelty) * (value - pImpl->meanNovelty);
    }
    pImpl->stdDevNovelty = std::sqrt(sqSum / pImpl->recentFeatures.size());
}

} // namespace nlm
