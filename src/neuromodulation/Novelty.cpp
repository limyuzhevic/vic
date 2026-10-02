#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Novelty::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float learningRate;
    float detectionThreshold;
    float memoryActivation;
    TimestepDuration lastUpdateTime;
    float noveltySensitivity;
    float patternChangeRate;
    
    // Historical pattern tracking
    std::vector<float> patternHistory;
    std::deque<float> noveltyHistory;
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.05f), 
             learningRate(0.1f), detectionThreshold(0.3f), memoryActivation(0.0f),
             lastUpdateTime(0), noveltySensitivity(1.0f), patternChangeRate(0.1f) {}
};

Novelty::Novelty() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Novelty detection system initialized");
}

Novelty::~Novelty() = default;

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) return;
    
    float novelty = computeNovelty(currentPattern, previousPattern);
    
    // Update level with novelty input
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->learningRate * 2.0f);
    
    // Store pattern for future comparison
    pImpl->patternHistory = currentPattern;
    pImpl->noveltyHistory.push_back(novelty);
    if (pImpl->noveltyHistory.size() > 1000) {
        pImpl->noveltyHistory.pop_front();
    }
    
    NLM_LOG_TRACE("Novelty detected: level={}, intensity={}", pImpl->level, novelty);
}

void Novelty::detectNovelty(const class Observation& observation,
                           const class Observation& previousObservation) {
    // Convert observations to patterns for analysis
    std::vector<float> currentPattern = extractPatternFromObservation(observation);
    std::vector<float> previousPattern = extractPatternFromObservation(previousObservation);
    
    detectNovelty(currentPattern, previousPattern);
}

void Novelty::update(TimestepDuration dt) {
    if (dt <= 0) return;
    
    TimestepDuration timeSinceUpdate = dt - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = dt;
    
    // Natural decay of novelty signal
    float decayFactor = 1.0f - pImpl->decayRate * static_cast<float>(timeSinceUpdate);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Reset memory activation over time
    pImpl->memoryActivation = std::max(0.0f, pImpl->memoryActivation - 0.01f * static_cast<float>(dt));
}

float Novelty::computeNovelty(const std::vector<float>& current,
                             const std::vector<float>& previous) const {
    if (current.size() != previous.size() || current.empty()) return 0.0f;
    
    // Compute pattern change magnitude
    float totalChange = 0.0f;
    float maxChange = 0.0f;
    
    for (size_t i = 0; i < current.size(); ++i) {
        float change = std::abs(current[i] - previous[i]);
        totalChange += change;
        maxChange = std::max(maxChange, change);
    }
    
    float avgChange = totalChange / current.size();
    
    // Combine average and maximum changes
    float novelty = avgChange * 0.7f + maxChange * 0.3f;
    
    // Apply sensitivity factor
    novelty = std::min(1.0f, novelty * pImpl->noveltySensitivity);
    
    return novelty;
}

float Novelty::getNoveltyScore(const std::vector<float>& pattern) const {
    // Compute novelty relative to stored history
    if (pImpl->patternHistory.empty()) return 0.0f;
    
    float similarity = computeSimilarity(pattern, pImpl->patternHistory);
    float novelty = 1.0f - similarity;
    
    return std::clamp(novelty, 0.0f, 1.0f);
}

const std::vector<float>& Novelty::getPatternHistory() const {
    return pImpl->patternHistory;
}

const std::vector<float>& Novelty::getNoveltyHistory() const {
    return pImpl->noveltyHistory;
}

void Novelty::setDetectionThreshold(float threshold) {
    pImpl->detectionThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Novelty::setNoveltySensitivity(float sensitivity) {
    pImpl->noveltySensitivity = std::clamp(sensitivity, 0.1f, 3.0f);
}

void Novelty::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->memoryActivation = 0.0f;
    pImpl->patternHistory.clear();
    pImpl->noveltyHistory.clear();
}

void Novelty::setMemoryActivation(float activation) {
    pImpl->memoryActivation = std::clamp(activation, 0.0f, 1.0f);
    
    // Memory activation can enhance novelty detection
    if (activation > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + activation * 0.3f);
    }
}

float Novelty::computeSimilarity(const std::vector<float>& a,
                               const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

std::vector<float> Novelty::extractPatternFromObservation(const class Observation& observation) const {
    std::vector<float> pattern;
    
    // Convert observation to feature vector
    // In a real implementation, this would extract meaningful features
    // For now, use a simple approach
    pattern.reserve(20);
    
    // Use observation type and basic properties
    float obsType = static_cast<float>(observation.getType());
    pattern.push_back(obsType);
    
    // Add any available numeric features
    // observation.getReward() would be called here in real implementation
    pattern.push_back(0.5f); // Placeholder
    
    // Add location or position information if available
    pattern.push_back(0.0f); // Placeholder for x coordinate
    pattern.push_back(0.0f); // Placeholder for y coordinate
    
    return pattern;
}

} // namespace nlm