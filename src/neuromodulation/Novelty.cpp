#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float noveltyThreshold;
    float decayRate;
    float accumulationRate;
    float adaptationThreshold;
    float maxLevel;
    std::vector<float> patternHistory;
    std::vector<float> sensoryHistory;
    std::vector<float> predictionErrors;
    size_t historySize;
    
    Impl() : brain(nullptr), level(0.0f), noveltyThreshold(0.15f), decayRate(0.05f),
             accumulationRate(0.2f), adaptationThreshold(0.3f), maxLevel(1.0f),
             historySize(100) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized with " + std::to_string(pImpl->historySize) + 
                 std::string(" history slots"));
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->maxLevel);
}

void Novelty::detectNovelty(const Observation& observation,
                           const Observation& previousObservation) {
    if (!pImpl->brain) {
        NLM_LOG_WARNING("Novelty detection called before initialization");
        return;
    }
    
    // Extract features from observations
    std::vector<float> currentPattern = extractFeatures(observation);
    std::vector<float> previousPattern = extractFeatures(previousObservation);
    
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute novelty as difference from expected patterns
    float novelty = computeNovelty(currentPattern, previousPattern);
    
    // Update novelty level with adaptive response
    if (novelty > pImpl->adaptationThreshold) {
        // Strong novelty trigger - sudden increase
        pImpl->level = std::min(pImpl->maxLevel, pImpl->level + novelty * 2.0f * pImpl->accumulationRate);
        
        // Trigger curiosity-based exploration
        if (pImpl->brain) {
            pImpl->brain->triggerExploration(novelty);
        }
        
        NLM_LOG_DEBUG(std::string("High novelty detected: ") + std::to_string(novelty));
    } else {
        // Gradual novelty adaptation
        pImpl->level += (novelty - pImpl->level) * pImpl->accumulationRate;
    }
    
    // Store pattern for future comparison
    if (pImpl->patternHistory.size() >= pImpl->historySize) {
        pImpl->patternHistory.erase(pImpl->patternHistory.begin());
    }
    pImpl->patternHistory.push_back(novelty);
    
    // Store sensory observation
    std::vector<float> sensoryPattern = observation.getSensoryData();
    if (sensoryPattern.size() > 0) {
        if (pImpl->sensoryHistory.size() >= pImpl->historySize) {
            pImpl->sensoryHistory.erase(pImpl->sensoryHistory.begin());
        }
        pImpl->sensoryHistory.push_back(sensoryPattern[0]);  // Use first value as example
    }
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute difference between patterns
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDiff += diff;
    }
    
    // Normalize by pattern size
    float avgDiff = totalDiff / compareLen;
    
    // Apply adaptive threshold for novelty detection
    float novelty = std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
    
    // Update novelty level with history integration
    float historicalInfluence = 0.0f;
    if (pImpl->patternHistory.size() > 5) {
        // Average recent novelty history
        float recentSum = 0.0f;
        size_t count = std::min<size_t>(pImpl->patternHistory.size(), 10);
        for (size_t i = pImpl->patternHistory.size() - count; i < pImpl->patternHistory.size(); ++i) {
            recentSum += pImpl->patternHistory[i];
        }
        historicalInfluence = recentSum / count;
    }
    
    // Combine current and historical novelty
    float combinedNovelty = 0.7f * novelty + 0.3f * historicalInfluence;
    
    // Update novelty level with adaptive response
    if (combinedNovelty > pImpl->adaptationThreshold) {
        // Strong novelty trigger
        pImpl->level = std::min(pImpl->maxLevel, pImpl->level + combinedNovelty * 1.5f);
        
        // Trigger curiosity
        if (pImpl->brain) {
            pImpl->brain->triggerCuriosity(combinedNovelty);
        }
    } else {
        // Gradual adaptation
        pImpl->level += (combinedNovelty - pImpl->level) * pImpl->accumulationRate;
    }
    
    // Store in history
    if (pImpl->patternHistory.size() >= pImpl->historySize) {
        pImpl->patternHistory.erase(pImpl->patternHistory.begin());
    }
    pImpl->patternHistory.push_back(combinedNovelty);
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time with exponential decay
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level *= decayFactor;
    
    // Apply homeostatic regulation to prevent runaway novelty
    if (pImpl->level < 0.01f && pImpl->patternHistory.size() > 10) {
        // Low novelty: check if we've adapted enough
        float recentNovelty = 0.0f;
        size_t recentCount = std::min<size_t>(pImpl->patternHistory.size(), 10);
        for (size_t i = 0; i < recentCount; ++i) {
            recentNovelty += pImpl->patternHistory[pImpl->patternHistory.size() - 1 - i];
        }
        recentNovelty /= recentCount;
        
        if (recentNovelty < pImpl->noveltyThreshold * 0.5f) {
            // Reset to baseline to allow fresh detection
            pImpl->level = 0.0f;
        }
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->patternHistory;
}

void Novelty::clearHistory() {
    pImpl->patternHistory.clear();
    pImpl->sensoryHistory.clear();
    pImpl->predictionErrors.clear();
}

std::vector<float> Novelty::extractFeatures(const Observation& observation) const {
    std::vector<float> features;
    
    // Extract various features from observation
    // In a real implementation, this would extract meaningful features
    // For now, just return sensory data as features
    auto sensory = observation.getSensoryData();
    
    // Simple feature extraction: downsample and compute statistics
    if (sensory.size() > 10) {
        // Downsample to 10 features
        for (size_t i = 0; i < 10 && i < sensory.size(); ++i) {
            size_t idx = i * sensory.size() / 10;
            features.push_back(sensory[idx]);
        }
    } else {
        features = sensory;
    }
    
    return features;
}

float Novelty::computeNovelty(const std::vector<float>& current,
                             const std::vector<float>& previous) const {
    if (current.size() != previous.size()) {
        // Handle different sizes by comparing overlapping portion
        size_t minSize = std::min(current.size(), previous.size());
        return computeNovelty(
            std::vector<float>(current.begin(), current.begin() + minSize),
            std::vector<float>(previous.begin(), previous.begin() + minSize)
        );
    }
    
    if (current.empty()) {
        return 0.0f;
    }
    
    // Compute average absolute difference
    float totalDiff = 0.0f;
    for (size_t i = 0; i < current.size(); ++i) {
        totalDiff += std::abs(current[i] - previous[i]);
    }
    
    return totalDiff / current.size();
}

} // namespace nlm