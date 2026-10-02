#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Novelty::Impl {
    // Brain reference for context
    class Brain* brain;
    
    // Current novelty level
    float level;
    
    // Previous observation pattern for comparison
    std::vector<float> previousPattern;
    
    // Parameters for novelty detection
    float noveltyThreshold;       // Threshold for considering change as novel
    float adaptationRate;          // Rate at which novelty detector adapts to statistics
    float decayRate;               // Natural decay of novelty over time
    float memoryTraceDecay;        // Decay rate for memory traces
    
    // Statistics and history
    float minNovelty;
    float maxNovelty;
    float sumNovelty;
    size_t countNovelty;
    std::vector<float> history;
    
    // Internal state for learning
    float recentNovelty;
    size_t consecutiveNovelties;   // Number of consecutive novel experiences
    
    // Neural adaptation mechanisms
    float predictionErrorHistory;  // Running average of prediction errors
    float predictionErrorVariance; // Variance of prediction errors
    float surpriseSignal;          // Combined surprise from prediction and sensory mismatch
    
    // Memory and association
    std::vector<std::pair<std::vector<float>, float>> memoryTraces;  // Pattern -> strength
    
    Impl() : brain(nullptr), level(0.0f),
             noveltyThreshold(0.3f), adaptationRate(0.05f), decayRate(0.1f),
             memoryTraceDecay(0.98f), recentNovelty(0.0f),
             consecutiveNovelties(0), predictionErrorHistory(0.0f),
             predictionErrorVariance(0.0f), surpriseSignal(0.0f),
             minNovelty(std::numeric_limits<float>::max()),
             maxNovelty(std::numeric_limits<float>::lowest()),
             sumNovelty(0.0f), countNovelty(0) {}
};

Novelty::Novelty() : pImpl(new Impl), recentNovelty_(0.0f) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection system initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

void Novelty::detectObservationNovelty(const Observation& observation,
                                       const Observation& previousObservation) {
    // Compute novelty based on observation differences
    const auto& currentData = observation.getData();
    const auto& prevData = previousObservation.getData();
    
    if (currentData.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Update previous pattern
    pImpl->previousPattern = currentData;
    
    // Compute various novelty metrics
    float mse = 0.0f;  // Mean squared error
    float mae = 0.0f;  // Mean absolute error
    float energyChange = 0.0f;  // Change in signal energy
    
    size_t compareLen = std::min(currentData.size(), prevData.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = currentData[i] - prevData[i];
        mse += diff * diff;
        mae += std::abs(diff);
        energyChange += std::abs(currentData[i] - prevData[i]);
    }
    
    // Normalize by pattern size
    if (compareLen > 0) {
        mse /= compareLen;
        mae /= compareLen;
        energyChange /= compareLen;
    }
    
    // Novelty is based on unexpected changes
    // More complex: surprise = -log(p(x)) where p(x) is probability of observation
    float surprise = std::exp(-mse / 0.1f);  // Based on MSE (lower MSE = higher probability)
    
    // Blend different novelty measures
    pImpl->level = 0.6f * surprise + 0.2f * (1.0f - mae) + 0.2f * energyChange;
    
    // Normalize to [0, 1] range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    pImpl->recentNovelty = pImpl->level;
    
    // Update statistics
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    if (pImpl->countNovelty > 0) {
        pImpl->minNovelty = std::min(pImpl->minNovelty, pImpl->level);
        pImpl->maxNovelty = std::max(pImpl->maxNovelty, pImpl->level);
        pImpl->sumNovelty += pImpl->level;
    } else {
        pImpl->minNovelty = pImpl->level;
        pImpl->maxNovelty = pImpl->level;
    }
    pImpl->countNovelty++;
}

void Novelty::detectPatternNovelty(const std::vector<float>& currentPattern,
                                    const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Store previous pattern
    pImpl->previousPattern = currentPattern;
    
    // Compute pattern similarity and novelty
    float similarity = computePatternSimilarity(currentPattern, previousPattern);
    float patternNovelty = 1.0f - similarity;  // Novelty is inverse of similarity
    
    // Apply statistical novelty detection
    float statisticalNovelty = computeStatisticalNovelty(currentPattern);
    
    // Combine similarity and statistical novelty
    pImpl->level = 0.7f * patternNovelty + 0.3f * statisticalNovelty;
    
    pImpl->recentNovelty = pImpl->level;
    
    // Update statistics
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time (familiarity reduces novelty)
    float timeStep = static_cast<float>(dt);
    
    // Natural decay
    pImpl->level *= (1.0f - pImpl->decayRate * timeStep);
    
    // Update prediction error statistics
    pImpl->predictionErrorHistory *= 0.99f;  // Exponential decay
    pImpl->predictionErrorVariance *= 0.99f;
    
    // Update memory traces
    updateMemoryTraces(timeStep);
    
    // Check for consecutive novel experiences
    if (pImpl->level > pImpl->noveltyThreshold) {
        pImpl->consecutiveNovelties++;
    } else {
        pImpl->consecutiveNovelties = 0;
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->memoryTraces.clear();
}

float Novelty::getMeanNovelty() const {
    if (pImpl->countNovelty == 0) return 0.0f;
    return pImpl->sumNovelty / pImpl->countNovelty;
}

float Novelty::getMaxNovelty() const {
    return pImpl->maxNovelty;
}

float Novelty::getMinNovelty() const {
    return pImpl->minNovelty;
}

bool Novelty::isSignificant() const {
    return pImpl->level > pImpl->noveltyThreshold;
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Novelty::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.01f, 0.5f);
}

void Novelty::setDecaysRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.0f, 0.5f);
}

void Novelty::setMemoryTraceDecay(float decay) {
    pImpl->memoryTraceDecay = std::clamp(decay, 0.5f, 0.999f);
}

// Helper functions for novelty detection
float Novelty::computePatternSimilarity(const std::vector<float>& pattern1,
                                       const std::vector<float>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    size_t len = std::min(pattern1.size(), pattern2.size());
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < len; ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(norm1 * norm2);
}

float Novelty::computeStatisticalNovelty(const std::vector<float>& pattern) const {
    // Compute statistical novelty based on pattern properties
    if (pattern.empty()) return 0.0f;
    
    // Compute pattern statistics
    float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    float variance = 0.0f;
    
    for (float val : pattern) {
        float diff = val - mean;
        variance += diff * diff;
    }
    variance /= pattern.size();
    
    // Novelty is based on variance (higher variance = more novel)
    float novelty = variance / 10.0f;  // Normalize
    
    return std::clamp(novelty, 0.0f, 1.0f);
}

void Novelty::updateMemoryTraces(float dt) {
    // Update memory traces with decay
    for (auto& trace : pImpl->memoryTraces) {
        trace.second *= pImpl->memoryTraceDecay;
    }
    
    // Remove very weak traces
    pImpl->memoryTraces.erase(
        std::remove_if(pImpl->memoryTraces.begin(), pImpl->memoryTraces.end(),
                      [](const std::pair<std::vector<float>, float>& trace) {
                          return trace.second < 0.01f;
                      }),
        pImpl->memoryTraces.end()
    );
}

} // namespace nlm
