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
    float adaptationRate;
    float maxLevel;
    
    // Pattern analysis
    size_t patternSize;
    float patternSimilarityThreshold;
    
    Impl() : 
        brain(nullptr), 
        level(0.0f), 
        decayRate(0.1f), 
        noveltyThreshold(0.3f),
        adaptationRate(0.2f),
        maxLevel(1.0f),
        patternSize(10),
        patternSimilarityThreshold(0.8f) {
        history.reserve(100);
        lastPattern.reserve(patternSize);
    }
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize with empty pattern
    pImpl->lastPattern.clear();
    
    NLM_LOG_INFO("Novelty detection initialized with brain reference");
    NLM_LOG_DEBUG(std::string("Novelty threshold: ") + std::to_string(pImpl->noveltyThreshold));
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->maxLevel);
    pImpl->history.push_back(pImpl->level);
    
    if (pImpl->level > 0.7f) {
        NLM_LOG_INFO(std::string("High novelty detected: ") + std::to_string(pImpl->level));
    }
}

void Novelty::detectNovelty(const Observation& observation,
                            const Observation& previousObservation) {
    // Extract features from observations
    std::vector<float> currentPattern;
    std::vector<float> previousPattern;
    
    // Convert observations to numerical patterns for comparison
    // This is a simplified implementation - real systems would have
    // proper feature extraction based on sensory modalities
    
    // For now, use the observation data directly if available
    if (!observation.empty()) {
        currentPattern.assign(observation.begin(), observation.end());
    }
    
    if (!previousObservation.empty()) {
        previousPattern.assign(previousObservation.begin(), previousObservation.end());
    }
    
    detectNovelty(currentPattern, previousPattern);
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
    
    // Use subset if patterns are longer than expected size
    if (compareLen > pImpl->patternSize) {
        std::vector<float> currentSubset(currentPattern.begin(), 
                                        currentPattern.begin() + pImpl->patternSize);
        std::vector<float> previousSubset(previousPattern.begin(), 
                                         previousPattern.begin() + pImpl->patternSize);
        currentPattern.swap(currentSubset);
        previousPattern.swap(previousSubset);
        compareLen = pImpl->patternSize;
    }
    
    // Calculate Euclidean distance between patterns
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = currentPattern[i] - previousPattern[i];
        totalDiff += diff * diff;
    }
    
    totalDiff = std::sqrt(totalDiff);
    
    // Normalize by pattern size for meaningful comparison
    float avgDiff = totalDiff / compareLen;
    
    // Update novelty level based on difference with adaptive thresholding
    // Use exponential function to make small differences less significant
    pImpl->level = std::min(pImpl->maxLevel, avgDiff / pImpl->noveltyThreshold);
    
    // Apply adaptation: gradually decrease novelty detection sensitivity
    if (pImpl->level > 0.5f) {
        // High novelty: store in history
        pImpl->history.push_back(pImpl->level);
    }
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
    
    // Log novelty detection
    if (pImpl->level > 0.1f) {
        NLM_LOG_DEBUG(std::string("Novelty detected: diff=") + std::to_string(avgDiff) +
                     ", level=" + std::to_string(pImpl->level));
    }
}

void Novelty::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Decay novelty over time (forgetfulness)
    float decay = pImpl->decayRate * deltaTime;
    pImpl->level = std::max(0.0f, pImpl->level - decay);
    
    // Adaptive decay based on recent history
    if (!pImpl->history.empty()) {
        // Compute moving average of recent novelty levels
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        float avgLevel = sum / pImpl->history.size();
        
        // Adjust decay rate based on average novelty
        pImpl->decayRate = 0.05f + 0.1f * avgLevel;  // More decay when novelty is high
    }
    
    // Clean old history (keep only recent)
    if (pImpl->history.size() > 50) {
        pImpl->history.erase(pImpl->history.begin(), 
                           pImpl->history.begin() + 10);
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    NLM_LOG_INFO("Novelty history cleared");
}

} // namespace nlm
