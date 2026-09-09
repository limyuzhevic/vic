#include "Novelty.hpp"
#include "../core/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float noveltyThreshold;
    float adaptationRate;
    float explorationDrive;
    std::vector<float> history;
    std::vector<float> lastPattern;
    std::vector<float> expectedPattern;
    std::vector<float> variance;
    float predictionError;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.05f), noveltyThreshold(0.3f),
             adaptationRate(0.1f), explorationDrive(0.0f), predictionError(0.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    if (!pImpl->brain) {
        NLM_LOG_WARNING("Brain not initialized for novelty detection");
        pImpl->level = 0.0f;
        return;
    }
    
    // Extract neural activity patterns from observations
    std::vector<float> currentPattern = extractNeuralPattern(observation);
    std::vector<float> previousPattern = extractNeuralPattern(previousObservation);
    
    // Update expected pattern using exponential moving average
    if (pImpl->expectedPattern.empty()) {
        pImpl->expectedPattern = currentPattern;
    } else {
        for (size_t i = 0; i < currentPattern.size() && i < pImpl->expectedPattern.size(); ++i) {
            pImpl->expectedPattern[i] = pImpl->expectedPattern[i] * 0.9f + currentPattern[i] * 0.1f;
        }
    }
    
    // Compute deviation from expected patterns
    float deviation = computePatternDeviation(currentPattern, pImpl->expectedPattern);
    
    // Adapt novelty threshold based on prediction error
    pImpl->noveltyThreshold = adaptThreshold(pImpl->noveltyThreshold, deviation);
    
    // Update novelty level
    pImpl->level = std::min(1.0f, deviation / pImpl->noveltyThreshold);
    
    // Update variance estimate
    updateVariance(currentPattern);
    
    // Store history
    pImpl->history.push_back(pImpl->level);
    pImpl->lastPattern = currentPattern;
    
    // Update exploration drive
    pImpl->explorationDrive = pImpl->level * (1.0f - pImpl->decayRate);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Update expected pattern if not initialized
    if (pImpl->expectedPattern.empty()) {
        pImpl->expectedPattern = currentPattern;
    }
    
    // Compute prediction error
    pImpl->predictionError = computePatternDeviation(currentPattern, pImpl->expectedPattern);
    
    // Update novelty based on prediction error
    pImpl->level = std::min(1.0f, std::abs(pImpl->predictionError) / pImpl->noveltyThreshold);
    
    // Adapt expected pattern
    for (size_t i = 0; i < currentPattern.size() && i < pImpl->expectedPattern.size(); ++i) {
        pImpl->expectedPattern[i] = pImpl->expectedPattern[i] * 0.9f + currentPattern[i] * 0.1f;
    }
    
    // Update variance
    updateVariance(currentPattern);
    
    // Store history
    pImpl->history.push_back(pImpl->level);
    pImpl->lastPattern = currentPattern;
    
    // Update exploration drive
    pImpl->explorationDrive = pImpl->level * (1.0f - pImpl->decayRate);
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply adaptation to expected pattern
    adaptExpectedPattern();
    
    // Update exploration drive
    pImpl->explorationDrive = std::max(0.0f, pImpl->explorationDrive - pImpl->decayRate * static_cast<float>(dt));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

float Novelty::getThreshold() const {
    return pImpl->noveltyThreshold;
}

void Novelty::setThreshold(float threshold) {
    pImpl->noveltyThreshold = std::max(0.01f, std::min(threshold, 2.0f));
}

// Helper methods
std::vector<float> Novelty::extractNeuralPattern(const Observation& observation) const {
    std::vector<float> pattern;
    
    // Extract neural activity from observation
    // This would interface with the brain's neural populations
    pattern.reserve(100); // Reserve space for typical neural patterns
    
    // For now, use placeholder implementation
    // In a real implementation, this would:
    // 1. Access brain neural populations
    // 2. Extract firing rates
    // 3. Normalize and return as feature vector
    
    for (size_t i = 0; i < 100; ++i) {
        pattern.push_back(0.5f); // Placeholder value
    }
    
    return pattern;
}

float Novelty::computePatternDeviation(const std::vector<float>& pattern1,
                                      const std::vector<float>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) {
        return 0.0f;
    }
    
    size_t compareLen = std::min(pattern1.size(), pattern2.size());
    float totalDiff = 0.0f;
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(pattern1[i] - pattern2[i]);
        totalDiff += diff;
    }
    
    // Normalize by pattern size and variance
    float avgDiff = totalDiff / compareLen;
    
    // Account for pattern variance to adapt threshold
    float variance = pImpl->variance.empty() ? 1.0f : pImpl->variance.back();
    variance = std::max(0.01f, variance);
    
    return avgDiff / variance;
}

void Novelty::updateVariance(const std::vector<float>& pattern) {
    if (pattern.empty()) return;
    
    // Compute variance of the pattern
    float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    float variance = 0.0f;
    
    for (float value : pattern) {
        float diff = value - mean;
        variance += diff * diff;
    }
    
    variance /= pattern.size();
    pImpl->variance.push_back(variance);
    
    // Keep only recent variance estimates
    if (pImpl->variance.size() > 100) {
        pImpl->variance.erase(pImpl->variance.begin());
    }
}

void Novelty::adaptExpectedPattern() {
    // Gradually adapt expected pattern to reflect new patterns
    // This represents the brain's ability to update its internal model
    for (float& value : pImpl->expectedPattern) {
        value *= 0.99f; // Gentle decay
    }
}

float Novelty::adaptThreshold(float currentThreshold, float deviation) {
    // Adapt novelty threshold based on recent prediction errors
    // High prediction errors lead to lower thresholds (more sensitive)
    // Low prediction errors lead to higher thresholds (less sensitive)
    
    float adaptationFactor = 0.1f;
    float errorFactor = std::min(1.0f, std::abs(deviation) / 2.0f);
    
    float newThreshold = currentThreshold * (1.0f - adaptationFactor) + 
                         (1.0f - errorFactor) * adaptationFactor;
    
    return std::max(0.01f, std::min(newThreshold, 2.0f));
}

} // namespace nlm
