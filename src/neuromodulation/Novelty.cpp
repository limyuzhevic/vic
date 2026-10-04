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
    float noveltyThreshold;
    float maxNovelty;
    float memoryDecay;
    std::vector<float> history;
    std::vector<float> lastPattern;
    std::vector<float> featureBuffer;
    size_t bufferSize;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f), 
             maxNovelty(1.0f), memoryDecay(0.95f), bufferSize(10) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->featureBuffer.resize(pImpl->bufferSize, 0.0f);
    NLM_LOG_INFO("Novelty detection system initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute comprehensive novelty from multiple perspectives
    
    // 1. Direct pattern difference (simplified Euclidean distance)
    float directDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        directDiff += diff * diff;
    }
    
    directDiff = std::sqrt(directDiff / static_cast<float>(compareLen));
    
    // 2. Statistical novelty (change in variance and mean)
    float currentMean = std::accumulate(currentPattern.begin(), currentPattern.end(), 0.0f) / currentPattern.size();
    float previousMean = std::accumulate(previousPattern.begin(), previousPattern.end(), 0.0f) / previousPattern.size();
    
    float currentVar = 0.0f, previousVar = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        currentVar += std::pow(currentPattern[i] - currentMean, 2);
        previousVar += std::pow(previousPattern[i] - previousMean, 2);
    }
    currentVar /= compareLen;
    previousVar /= compareLen;
    
    float varianceChange = std::abs(currentVar - previousVar) / std::max(0.01f, currentVar + previousVar);
    
    // 3. Pattern complexity (novelty in sequential patterns)
    float patternNovelty = 0.0f;
    if (currentPattern.size() >= 2) {
        for (size_t i = 1; i < compareLen; ++i) {
            float change = std::abs(currentPattern[i] - currentPattern[i-1]) - 
                          std::abs(previousPattern[i] - previousPattern[i-1]);
            patternNovelty += std::abs(change);
        }
        patternNovelty /= (compareLen - 1);
    }
    
    // Combine novelty measures with adaptive weights
    float novelty = pImpl->noveltyThreshold * directDiff +
                  0.3f * varianceChange +
                  0.5f * patternNovelty;
    
    // Apply novelty saturation and smoothing
    pImpl->level = std::min(1.0f, novelty);
    
    // Smooth with previous novelty level for stability
    pImpl->level = 0.7f * pImpl->level + 0.3f * pImpl->level;
    
    // Store in history for temporal dynamics
    pImpl->history.push_back(pImpl->level);
    
    // Keep history bounded
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update last pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::detectNovelty(const Observation& observation,
                           const Observation& previousObservation) {
    // Convert observations to feature vectors for comparison
    std::vector<float> currentFeatures;
    std::vector<float> previousFeatures;
    
    // Extract features from observations
    if (!observation.empty()) {
        currentFeatures.reserve(observation.size());
        for (const auto& value : observation) {
            currentFeatures.push_back(static_cast<float>(value));
        }
    }
    
    if (!previousObservation.empty()) {
        previousFeatures.reserve(previousObservation.size());
        for (const auto& value : previousObservation) {
            previousFeatures.push_back(static_cast<float>(value));
        }
    }
    
    detectNovelty(currentFeatures, previousFeatures);
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time (forgetting principle)
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply memory decay to history
    for (auto& value : pImpl->history) {
        value *= pImpl->memoryDecay;
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->lastPattern.clear();
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Novelty::setDecayRate(float decayRate) {
    pImpl->decayRate = std::max(0.0f, decayRate);
}

float Novelty::getRecentNovelty(size_t numSamples) const {
    if (pImpl->history.empty()) return 0.0f;
    
    size_t samples = std::min(numSamples, pImpl->history.size());
    if (samples == 0) return 0.0f;
    
    float sum = std::accumulate(pImpl->history.end() - samples, pImpl->history.end(), 0.0f);
    return sum / samples;
}

} // namespace nlm
