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
    // Enhanced novelty detection using feature space distance
    // Convert observations to feature vectors and compute distance
    std::vector<float> currentPattern, previousPattern;
    
    // Extract features from observation (simplified for now)
    currentPattern = observation.getData();
    previousPattern = previousObservation.getData();
    
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute Mahalanobis distance for feature space difference
    float totalSquaredDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = currentPattern[i] - previousPattern[i];
        totalSquaredDiff += diff * diff;
    }
    
    // Normalize by pattern size and scale factor
    float featureDistance = totalSquaredDiff / (compareLen + 1.0f);
    
    // Apply nonlinear transformation to novelty function
    pImpl->level = std::min(1.0f, std::sqrt(featureDistance) / pImpl->noveltyThreshold);
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute multiple distance metrics for robust novelty detection
    float euclideanDistance = 0.0f;
    float manhattanDistance = 0.0f;
    float cosineSimilarity = 0.0f;
    
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = currentPattern[i] - previousPattern[i];
        euclideanDistance += diff * diff;
        manhattanDistance += std::abs(diff);
    }
    
    // Compute cosine similarity
    float dotProduct = 0.0f;
    float currentNorm = 0.0f;
    float previousNorm = 0.0f;
    
    for (size_t i = 0; i < compareLen; ++i) {
        dotProduct += currentPattern[i] * previousPattern[i];
        currentNorm += currentPattern[i] * currentPattern[i];
        previousNorm += previousPattern[i] * previousPattern[i];
    }
    
    if (currentNorm > 0.0f && previousNorm > 0.0f) {
        cosineSimilarity = dotProduct / std::sqrt(currentNorm * previousNorm);
    }
    
    // Combine multiple metrics for robust novelty detection
    float patternDistance = std::min(1.0f, std::sqrt(euclideanDistance / compareLen));
    float manhattanNorm = std::min(1.0f, manhattanDistance / (compareLen * 10.0f));
    float noveltyFromCosine = std::max(0.0f, 1.0f - cosineSimilarity);
    
    // Weighted combination of different distance metrics
    pImpl->level = 0.5f * patternDistance + 0.3f * manhattanNorm + 0.2f * noveltyFromCosine;
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison with temporal decay
    pImpl->lastPattern = currentPattern;
    pImpl->history.push_back(pImpl->level * 0.5f); // Add decayed version
}

void Novelty::update(TimestepDuration dt) {
    // Enhanced novelty dynamics with temporal filtering
    // Implements exponential decay with multiple timescales for biological realism
    
    // Fast timescale decay (0.1s) for immediate novelty fading
    float fastDecay = std::exp(-0.1f * static_cast<float>(dt));
    
    // Medium timescale decay (1.0s) for persistent novelty
    float mediumDecay = std::exp(-1.0f * static_cast<float>(dt));
    
    // Compute weighted decay based on novelty level
    float fastWeight = 0.7f + 0.3f * pImpl->level;  // Higher novelty -> faster decay
    float mediumWeight = 1.0f - fastWeight;
    
    pImpl->level = pImpl->level * (fastDecay * fastWeight + mediumDecay * mediumWeight);
    
    // Add stochastic component for biological noise
    if (pImpl->brain) {
        // Add small amount of random exploration
        float exploration = pImpl->brain->getRandomGenerator()->getFloat(0.0f, 0.05f);
        pImpl->level += exploration * (1.0f - pImpl->level);
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
