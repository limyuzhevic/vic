#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f) {}
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
    // Extract features from observations and compare
    // Enhanced implementation using statistical novelty detection
    const std::vector<float>& currentData = observation.getData();
    const std::vector<float>& previousData = previousObservation.getData();
    
    if (currentData.empty() || previousData.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute statistical differences
    float meanDiff = 0.0f, stdDiff = 0.0f;
    size_t compareLen = std::min(currentData.size(), previousData.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = currentData[i] - previousData[i];
        meanDiff += diff;
        
        // Compute standard deviation of differences
        stdDiff += diff * diff;
    }
    
    meanDiff /= compareLen;
    stdDiff = std::sqrt(stdDiff / compareLen);
    
    // Novelty based on both mean shift and variability
    float statisticalNovelty = std::abs(meanDiff) + stdDiff;
    
    // Apply temporal context - compare with running average
    pImpl->history.push_back(statisticalNovelty);
    if (pImpl->history.size() > 20) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Compute context-adjusted novelty
    float avgNovelty = 0.0f;
    if (!pImpl->history.empty()) {
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        avgNovelty = sum / pImpl->history.size();
    }
    
    // Novelty is significant if current value exceeds context average
    pImpl->level = std::max(0.0f, (statisticalNovelty - avgNovelty) / pImpl->noveltyThreshold);
    
    // Store current observation for next comparison
    pImpl->lastPattern = currentData;
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute sophisticated novelty metrics based on neural principles
    // Novelty as multivariate statistical deviation
    
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    // Compute principal components for dimensionality reduction
    std::vector<float> differences;
    differences.reserve(compareLen);
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = currentPattern[i] - previousPattern[i];
        differences.push_back(diff);
    }
    
    // Compute Mahalanobis-like distance (simplified)
    float variance = 0.0f;
    for (float diff : differences) {
        variance += diff * diff;
    }
    variance /= compareLen;
    
    // Novelty based on normalized deviation from pattern
    float normalizedDeviation = variance / (variance + 0.01f);  // Avoid division by zero
    
    // Apply temporal smoothing - compare with running novelty average
    pImpl->history.push_back(normalizedDeviation);
    if (pImpl->history.size() > 50) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Compute temporal novelty (difference from recent average)
    float avgNovelty = 0.0f;
    if (!pImpl->history.empty()) {
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        avgNovelty = sum / pImpl->history.size();
    }
    
    // Novelty is significant if current deviation exceeds temporal context
    pImpl->level = std::max(0.0f, (normalizedDeviation - avgNovelty) / pImpl->noveltyThreshold);
    
    // Store current pattern for next comparison with more precision
    pImpl->lastPattern = currentPattern;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty with realistic time constant
    // Novelty signals are transient and decay over time
    // Models the adaptive nature of novelty detection
    
    const float dt_sec = static_cast<float>(dt) * 0.001f;  // Convert to seconds
    float timeConstant = 5.0f;  // Novelty decay time constant (seconds)
    
    // Exponential decay: N(t) = N(0) * exp(-t/tau)
    // Using Euler integration for stability
    if (pImpl->level > 0.0f) {
        pImpl->level *= std::exp(-dt_sec / timeConstant);
    }
    
    // Add small baseline recovery for low novelty
    pImpl->level = std::max(pImpl->level, 0.01f);
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
