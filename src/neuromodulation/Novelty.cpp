#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    
    // Neural substrate integration
    float patternVariance;
    float adaptationRate;
    float socialLearningWeight;
    
    // History-based novelty detection
    std::vector<std::vector<float>> experiencePatterns;
    std::vector<float> patternRecency;
    float memoryDecay;
    
    // Real-time neural activity statistics
    float meanActivity;
    float activityVariance;
    float recentChangeRate;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f)
        , patternVariance(0.5f), adaptationRate(0.05f), socialLearningWeight(0.2f)
        , memoryDecay(0.95f), meanActivity(0.0f), activityVariance(0.0f), recentChangeRate(0.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection system initialized with neural substrate");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Extract features from neural activity patterns
    std::vector<float> currentPattern;
    std::vector<float> previousPattern;
    
    // In real implementation, extract actual neural activity features
    // For now, compute basic feature differences
    float patternDiff = 0.0f;
    
    // Simple feature extraction from observation (placeholder)
    if (!observation.features.empty()) {
        currentPattern = observation.features;
    } else {
        // Generate synthetic features from observation data
        currentPattern = {observation.temperature, observation.humidity, observation.pressure,
                         observation.soundLevel, observation.lightLevel, observation.movement};
    }
    
    if (!previousObservation.features.empty()) {
        previousPattern = previousObservation.features;
    } else if (!pImpl->lastPattern.empty()) {
        previousPattern = pImpl->lastPattern;
    } else {
        // Generate synthetic previous pattern
        previousPattern = {0.5f, 0.5f, 1.0f, 0.3f, 0.7f, 0.2f};
    }
    
    // Compute pattern difference using Euclidean distance
    if (currentPattern.size() >= 2 && previousPattern.size() >= 2) {
        size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
        float sumSquaredDiff = 0.0f;
        
        for (size_t i = 0; i < compareLen; ++i) {
            float diff = currentPattern[i] - previousPattern[i];
            sumSquaredDiff += diff * diff;
        }
        
        float euclideanDistance = std::sqrt(sumSquaredDiff);
        
        // Normalize and compute novelty
        float maxExpectedDiff = 2.0f; // Maximum expected difference between similar patterns
        float normalizedDiff = std::min(1.0f, euclideanDistance / maxExpectedDiff);
        
        // Apply adaptation to recurring patterns
        patternDiff = normalizedDiff * (1.0f - pImpl->adaptationRate * getPatternRecency(currentPattern));
        
        // Update experience patterns with social learning component
        if (pImpl->brain) {
            // Social learning from other agents' experiences
            patternDiff = patternDiff * (1.0f - pImpl->socialLearningWeight) + 
                         getSocialPatternSimilarity(currentPattern) * pImpl->socialLearningWeight;
        }
    }
    
    // Update novelty level with temporal dynamics
    pImpl->level = 0.7f * pImpl->level + 0.3f * patternDiff;
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
    
    // Update neural activity statistics
    pImpl->meanActivity = 0.9f * pImpl->meanActivity + 0.1f * patternDiff;
    pImpl->activityVariance = 0.8f * pImpl->activityVariance + 0.2f * (patternDiff - pImpl->meanActivity) * (patternDiff - pImpl->meanActivity);
    pImpl->recentChangeRate = std::abs(patternDiff - (pImpl->history.size() >= 2 ? pImpl->history[pImpl->history.size() - 2] : 0.0f));
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute difference between patterns with neural-inspired metrics
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    // Weighted sum of differences (more weight to recent features)
    for (size_t i = 0; i < compareLen; ++i) {
        float weight = 1.0f + static_cast<float>(i) / static_cast<float>(compareLen);
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDiff += weight * diff;
    }
    
    // Normalize by pattern size and apply nonlinear transformation
    float avgDiff = totalDiff / compareLen;
    float noveltyScore = std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
    
    // Apply adaptation to recurring patterns using memory decay
    float patternId = computePatternHash(currentPattern);
    float patternRecency = getPatternRecency(patternId);
    float adaptationFactor = std::pow(1.0f - pImpl->adaptationRate, patternRecency);
    
    // Update novelty level with biological plausibility
    pImpl->level = 0.6f * pImpl->level + 0.4f * noveltyScore * adaptationFactor;
    pImpl->history.push_back(pImpl->level);
    
    // Update neural activity statistics
    updateNeuralStatistics(currentPattern);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty with time and neural adaptation
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level *= decayFactor;
    
    // Neural adaptation reduces novelty response over time
    pImpl->adaptationRate = std::min(0.1f, pImpl->adaptationRate * (1.0f + pImpl->recentChangeRate * 0.5f));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

// Helper functions for neural novelty detection
float Novelty::computePatternHash(const std::vector<float>& pattern) {
    // Simple hash function for pattern identification
    float hash = 0.0f;
    for (size_t i = 0; i < pattern.size(); ++i) {
        hash += pattern[i] * (i + 1) * 0.1f;
    }
    return hash;
}

void Novelty::updatePatternMemory(const std::vector<float>& pattern) {
    float patternId = computePatternHash(pattern);
    
    // Add or update pattern in memory
    auto it = std::find(pImpl->experiencePatterns.begin(), 
                       pImpl->experiencePatterns.end(), pattern);
    if (it != pImpl->experiencePatterns.end()) {
        // Update existing pattern
        size_t index = std::distance(pImpl->experiencePatterns.begin(), it);
        pImpl->patternRecency[index] = std::min(10.0f, pImpl->patternRecency[index] + 1.0f);
    } else {
        // Add new pattern
        pImpl->experiencePatterns.push_back(pattern);
        pImpl->patternRecency.push_back(1.0f);
    }
    
    // Decay old patterns
    for (float& recency : pImpl->patternRecency) {
        recency *= pImpl->memoryDecay;
    }
}

float Novelty::getPatternRecency(const std::vector<float>& pattern) {
    float patternId = computePatternHash(pattern);
    return getPatternRecency(patternId);
}

float Novelty::getPatternRecency(float patternId) {
    // Find pattern recency (simplified implementation)
    // In real implementation, use proper lookup table
    return 1.0f; // Default low recency
}

float Novelty::getSocialPatternSimilarity(const std::vector<float>& pattern) {
    // Estimate similarity to other agents' experiences
    // In real implementation, would access shared memory or social learning system
    return 0.5f; // Default medium similarity
}

void Novelty::updateNeuralStatistics(const std::vector<float>& pattern) {
    // Update running statistics for neural activity
    if (pattern.empty()) return;
    
    // Update mean
    float oldMean = pImpl->meanActivity;
    pImpl->meanActivity = 0.9f * pImpl->meanActivity + 0.1f * pattern[0];
    
    // Update variance
    pImpl->activityVariance = 0.9f * pImpl->activityVariance + 
                              0.1f * (pattern[0] - pImpl->meanActivity) * (pattern[0] - pImpl->meanActivity);
    
    // Update recent change rate
    pImpl->recentChangeRate = std::abs(pattern[0] - oldMean);
}

} // namespace nlm
