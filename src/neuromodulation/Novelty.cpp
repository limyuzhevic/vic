#include "Novelty.hpp"
#include "../sensory/Observation.hpp"
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
    size_t historyLimit;
    float patternWeight;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f), 
             historyLimit(1000), patternWeight(0.7f) {}
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
    // Compute feature-based novelty using statistical differences
    if (!observation.isValid()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Extract multiple feature vectors from observation
    std::vector<float> currentPattern = extractObservationFeatures(observation);
    std::vector<float> previousPattern;
    
    if (pImpl->brain && previousObservation.isValid()) {
        previousPattern = extractObservationFeatures(previousObservation);
    } else {
        previousPattern = pImpl->lastPattern;
    }
    
    detectNovelty(currentPattern, previousPattern);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    if (currentPattern.size() != previousPattern.size()) {
        NLM_LOG_WARNING("Pattern size mismatch in novelty detection");
    }
    
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    // Multiple methods for novelty detection
    float differenceBasedNovelty = computeDifferenceNovelty(currentPattern, previousPattern, compareLen);
    float statisticalNovelty = computeStatisticalNovelty(currentPattern, compareLen);
    float contextualNovelty = computeContextualNovelty(currentPattern, compareLen);
    
    // Weighted combination
    pImpl->level = pImpl->patternWeight * differenceBasedNovelty + 
                   (1.0f - pImpl->patternWeight) * statisticalNovelty;
    
    pImpl->level = std::min(1.0f, pImpl->level + contextualNovelty * 0.1f);
    
    // Add to history with decay
    pImpl->history.push_back(pImpl->level);
    
    // Limit history size
    if (pImpl->history.size() > pImpl->historyLimit) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty with time constant
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level *= decayFactor;
    
    // Apply additional decay based on history average
    if (!pImpl->history.empty()) {
        float avgHistory = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f) / pImpl->history.size();
        if (pImpl->level < avgHistory * 0.5f) {
            pImpl->level *= 0.9f;  // Faster decay when below average
        }
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->lastPattern.clear();
}

// Helper methods
std::vector<float> Novelty::extractObservationFeatures(const Observation& observation) const {
    std::vector<float> features;
    
    // Extract sensory features
    auto sensoryData = observation.getSensoryData();
    features.reserve(sensoryData.size() + 10);
    
    for (float value : sensoryData) {
        features.push_back(value);
    }
    
    // Extract internal state features
    auto internalData = observation.getInternal();
    for (float value : internalData) {
        features.push_back(value * 0.1f);  // Scale internal signals
    }
    
    // Extract arousal features
    float arousal = observation.getArousal();
    features.push_back(arousal);
    
    // Extract attention features
    float attention = observation.getAttention();
    features.push_back(attention);
    
    // Extract novelty if available
    float novelty = observation.getNovelty();
    features.push_back(novelty * 2.0f);  // Scale for feature importance
    
    // Extract prediction error
    float predictionError = observation.getPredictionError();
    features.push_back(predictionError);
    
    return features;
}

float Novelty::computeDifferenceNovelty(const std::vector<float>& current, 
                                       const std::vector<float>& previous,
                                       size_t compareLen) const {
    float totalDiff = 0.0f;
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(current[i] - previous[i]);
        // Use squared difference for more sensitivity to large changes
        totalDiff += diff * diff;
    }
    
    // Normalize and scale to [0, 1]
    float avgDiff = totalDiff / compareLen;
    return std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
}

float Novelty::computeStatisticalNovelty(const std::vector<float>& pattern, 
                                        size_t patternSize) const {
    if (patternSize < 3) return 0.0f;
    
    // Compute basic statistics
    float mean = std::accumulate(pattern.begin(), pattern.begin() + patternSize, 0.0f) / patternSize;
    
    float variance = 0.0f;
    for (size_t i = 0; i < patternSize; ++i) {
        float diff = pattern[i] - mean;
        variance += diff * diff;
    }
    variance /= patternSize;
    
    // Standard deviation
    float stddev = std::sqrt(variance);
    
    // Normalize (higher standard deviation = more novel)
    return std::min(1.0f, stddev * 3.0f);
}

float Novelty::computeContextualNovelty(const std::vector<float>& pattern,
                                        size_t patternSize) const {
    // Compute contextual novelty based on recent history
    if (pImpl->history.size() < 2) return 0.0f;
    
    // Compare with recent history average
    size_t recentHistory = std::min(size_t(10), pImpl->history.size());
    float recentAvg = 0.0f;
    for (size_t i = pImpl->history.size() - recentHistory; i < pImpl->history.size(); ++i) {
        recentAvg += pImpl->history[i];
    }
    recentAvg /= recentHistory;
    
    // Current pattern energy
    float patternEnergy = 0.0f;
    for (size_t i = 0; i < patternSize && i < pattern.size(); ++i) {
        patternEnergy += pattern[i] * pattern[i];
    }
    patternEnergy /= patternSize;
    
    // Contextual novelty is deviation from recent average
    return std::max(0.0f, patternEnergy - recentAvg);
}

} // namespace nlm
