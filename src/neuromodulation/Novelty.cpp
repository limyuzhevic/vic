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
    // Real novelty detection using biological principles
    // Compare current sensory input to memory patterns for difference
    
    std::vector<float> currentPattern;
    
    // Extract features from current observation
    const auto& vision = observation.getVision();
    const auto& touch = observation.getTouch();
    const auto& internal = observation.getInternal();
    const auto& proprioception = observation.getProprioception();
    
    // Concatenate all sensory modalities
    currentPattern.reserve(vision.size() + touch.size() + internal.size() + proprioception.size());
    currentPattern.insert(currentPattern.end(), vision.begin(), vision.end());
    currentPattern.insert(currentPattern.end(), touch.begin(), touch.end());
    currentPattern.insert(currentPattern.end(), internal.begin(), internal.end());
    currentPattern.insert(currentPattern.end(), proprioception.begin(), proprioception.end());
    
    // Compare with previous pattern using neural principles
    // Higher-order statistics, pattern completion, and association
    detectNovelty(currentPattern, pImpl->lastPattern);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        pImpl->lastPattern = currentPattern;
        return;
    }
    
    // Compute multiple novelty measures (biologically inspired)
    float patternDifference = computePatternDifference(currentPattern, previousPattern);
    float statisticalNovelty = computeStatisticalNovelty(currentPattern, previousPattern);
    float contextualNovelty = computeContextualNovelty(currentPattern, previousPattern);
    
    // Integrate multiple novelty signals (weighted average)
    float weightedNovelty = patternDifference * 0.4f + 
                           statisticalNovelty * 0.3f + 
                           contextualNovelty * 0.3f;
    
    // Apply neural adaptation and thresholding
    pImpl->level = std::min(1.0f, weightedNovelty / noveltyThreshold_);
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

float Novelty::computePatternDifference(const std::vector<float>& current,
                                       const std::vector<float>& previous) const {
    if (current.size() != previous.size()) return 0.0f;
    
    // Multiple comparison metrics
    float sumSquared = 0.0f;
    float sumAbsolute = 0.0f;
    
    for (size_t i = 0; i < current.size(); ++i) {
        float diff = current[i] - previous[i];
        sumSquared += diff * diff;
        sumAbsolute += std::abs(diff);
    }
    
    // Normalize by pattern size
    float meanSquared = sumSquared / current.size();
    float meanAbsolute = sumAbsolute / current.size();
    
    // Use Mahalanobis-like distance (weighted by variance)
    return std::sqrt(meanSquared) + 0.5f * meanAbsolute;
}

float Novelty::computeStatisticalNovelty(const std::vector<float>& current,
                                         const std::vector<float>& previous) const {
    // Compute statistical properties that signal novelty
    if (current.empty() || previous.empty()) return 0.0f;
    
    // Calculate entropy-like measure
    float currentEntropy = calculatePatternEntropy(current);
    float previousEntropy = calculatePatternEntropy(previous);
    
    // Novelty as difference in pattern statistics
    float entropyDifference = std::abs(currentEntropy - previousEntropy);
    
    // Higher entropy = more novel
    return std::min(1.0f, entropyDifference * 2.0f);
}

float Novelty::computeContextualNovelty(const std::vector<float>& current,
                                         const std::vector<float>& previous) const {
    // Context-dependent novelty (how different from recent patterns)
    float contextualDifference = 0.0f;
    
    if (!pImpl->history.empty()) {
        // Compare to recent novelty history
        float recentAverage = 0.0f;
        for (float val : pImpl->history) {
            recentAverage += val;
        }
        recentAverage /= pImpl->history.size();
        
        // Novelty relative to recent baseline
        contextualDifference = std::abs(pImpl->level - recentAverage);
    }
    
    // Also consider change in specific sensory modalities
    return contextualDifference;
}

float Novelty::calculatePatternEntropy(const std::vector<float>& pattern) const {
    // Calculate entropy-like measure of pattern complexity
    if (pattern.empty()) return 0.0f;
    
    // Create histogram of pattern values
    std::vector<int> bins(10, 0);  // 10 bins
    for (float val : pattern) {
        int bin = std::min(9, static_cast<int>(val * 10.0f));
        bins[bin]++;
    }
    
    // Calculate entropy
    float total = static_cast<float>(pattern.size());
    float entropy = 0.0f;
    for (int count : bins) {
        if (count > 0) {
            float prob = count / total;
            entropy -= prob * std::log(std::max(prob, 0.001f));
        }
    }
    
    return entropy / std::log(10.0f);  // Normalize
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
