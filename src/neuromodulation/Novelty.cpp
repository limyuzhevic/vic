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
    
    // Compute difference between patterns using multiple metrics
    float totalDifference = 0.0f;
    float magnitudeDifference = 0.0f;
    float patternDifference = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDifference += diff;
        
        // Magnitude difference (energy difference)
        magnitudeDifference += std::abs(currentPattern[i]) - std::abs(previousPattern[i]);
        
        // Pattern difference (change in correlations)
        if (i > 0) {
            patternDifference += std::abs(currentPattern[i] - currentPattern[i-1]) - 
                               std::abs(previousPattern[i] - previousPattern[i-1]);
        }
    }
    
    // Normalize differences
    float avgDifference = totalDifference / compareLen;
    float avgMagnitudeDiff = magnitudeDifference / compareLen;
    float avgPatternDiff = patternDifference / compareLen;
    
    // Combine metrics with different weights
    float noveltyScore = avgDifference * 0.5f + 
                        std::abs(avgMagnitudeDiff) * 0.3f + 
                        std::abs(avgPatternDiff) * 0.2f;
    
    // Apply nonlinear scaling for sensitivity
    noveltyScore = 1.0f - std::exp(-noveltyScore * 5.0f); // Compresses to [0, 1)
    
    // Update novelty level with adaptive response
    float learningRate = 0.2f; // Faster adaptation for novelty
    pImpl->level += (noveltyScore - pImpl->level) * learningRate;
    
    // Novelty detection influences neural gain and exploration
    if (pImpl->brain && pImpl->level > noveltyScore * 0.5f) {
        // Strong novelty drives increased exploration and learning
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, pImpl->level * 3.0f);
    }
    
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
