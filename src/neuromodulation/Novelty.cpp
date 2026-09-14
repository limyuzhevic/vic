#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float adaptationThreshold;
    float contextSensitivity;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    
    // Enhanced pattern recognition
    std::vector<std::vector<float>> patternMemory;  // Store experienced patterns
    std::vector<float> patternWeights;              // Confidence in each pattern
    size_t maxMemoryPatterns;
    
    // Multi-scale novelty detection
    float recentChangeRate;
    float longTermChangeRate;
    float biologicalNoise;
    
    // Working memory integration
    std::vector<float> workingMemoryBuffer;
    size_t workingMemoryCapacity;
    
    // Random number generator
    mutable std::mt19937 rng;
    mutable std::uniform_real_distribution<float> uniformDist;
    mutable std::normal_distribution<float> normalDist;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), adaptationThreshold(0.3f),
             contextSensitivity(0.5f), noveltyThreshold(0.3f),
             maxMemoryPatterns(100), recentChangeRate(0.0f), longTermChangeRate(0.0f),
             biologicalNoise(0.05f), workingMemoryCapacity(50),
             rng(std::random_device()()), uniformDist(0.0f, 1.0f),
             normalDist(0.0f, 0.1f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection system initialized with multi-scale pattern matching");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Extract features from observations
    auto currentPattern = extractPatternFromObservation(observation);
    auto previousPattern = extractPatternFromObservation(previousObservation);
    
    detectNovelty(currentPattern, previousPattern);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Multi-scale novelty detection:
    // 1. Immediate change (short-term)
    // 2. Pattern deviation (medium-term)
    // 3. Working memory mismatch (long-term)
    
    // 1. Immediate change rate
    float immediateChange = computePatternDifference(currentPattern, previousPattern);
    pImpl->recentChangeRate = 0.9f * pImpl->recentChangeRate + 
                             0.1f * immediateChange;
    
    // 2. Pattern deviation against memory
    float patternDeviation = computePatternDeviation(currentPattern);
    
    // 3. Working memory context
    float contextMatch = computeWorkingMemoryMatch(currentPattern);
    
    // Combine signals with biological weights
    float immediateNovelty = std::min(1.0f, pImpl->recentChangeRate / pImpl->noveltyThreshold);
    float patternNovelty = patternDeviation * 0.6f;
    float contextNovelty = (1.0f - contextMatch) * 0.4f;
    
    // Weighted combination with temporal smoothing
    pImpl->level = 0.4f * immediateNovelty + 
                   0f2f * patternNovelty + 
                   0f4f * contextNovelty;
    
    // Add biological noise for realistic variability
    float noise = normalDist(pImpl->rng) * pImpl->biologicalNoise;
    pImpl->level = std::max(0.0f, pImpl->level + noise);
    
    // Store in history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Store current pattern
    pImpl->lastPattern = currentPattern;
    updatePatternMemory(currentPattern);
    
    // Log significant novelty
    if (pImpl->level > 0.7f) {
        NLM_LOG_INFO("High novelty detected: " + std::to_string(pImpl->level));
    }
}

void Novelty::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Decay novelty with biological time constants
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * deltaTime);
    
    // Update working memory
    updateWorkingMemory(deltaTime);
    
    // Long-term change rate adaptation
    if (!pImpl->history.empty()) {
        float avgHistory = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f) /
                         pImpl->history.size();
        pImpl->longTermChangeRate = 0.95f * pImpl->longTermChangeRate + 
                                   0f05f * (pImpl->level - avgHistory);
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->patternMemory.clear();
    pImpl->patternWeights.clear();
    pImpl->workingMemoryBuffer.clear();
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::max(0.01f, threshold);
}

void Novelty::setContextSensitivity(float sensitivity) {
    pImpl->contextSensitivity = std::clamp(sensitivity, 0.1f, 1.0f);
}

bool Novelty::isNovel(const std::vector<float>& pattern) const {
    if (pattern.empty()) return false;
    
    float deviation = computePatternDeviation(pattern);
    return deviation > pImpl->noveltyThreshold;
}

std::vector<float> Novelty::extractPatternFromObservation(const Observation& observation) const {
    std::vector<float> pattern;
    
    // Combine different sensory modalities into a unified pattern
    auto vision = observation.getVision();
    auto touch = observation.getTouch();
    auto internal = observation.getInternal();
    auto proprio = observation.getProprioception();
    
    // Vision (highest weight)
    pattern.insert(pattern.end(), vision.begin(), vision.end());
    
    // Touch (medium weight)
    for (size_t i = 0; i < std::min(size_t(8), touch.size()); ++i) {
        pattern.push_back(touch[i] * 0.5f);
    }
    
    // Internal signals (lower weight)
    for (size_t i = 0; i < std::min(size_t(4), internal.size()); ++i) {
        pattern.push_back((internal[i] * 2.0f - 1.0f) * 0.3f);
    }
    
    // Proprioception (lowest weight)
    for (size_t i = 0; i < std::min(size_t(6), proprio.size()); ++i) {
        pattern.push_back((proprio[i] * 2.0f - 1.0f) * 0.2f);
    }
    
    return pattern;
}

float Novelty::computePatternDifference(const std::vector<float>& pattern1,
                                       const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size() || pattern1.empty()) {
        return 0.0f;
    }
    
    float sumDiff = 0.0f;
    for (size_t i = 0; i < pattern1.size(); ++i) {
        float diff = std::abs(pattern1[i] - pattern2[i]);
        sumDiff += diff;
    }
    
    return sumDiff / pattern1.size();
}

float Novelty::computePatternDeviation(const std::vector<float>& pattern) const {
    if (pattern.empty() || pImpl->patternMemory.empty()) {
        return 0.0f;
    }
    
    float totalDeviation = 0.0f;
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < pImpl->patternMemory.size(); ++i) {
        if (pImpl->patternMemory[i].size() == pattern.size()) {
            float dev = computePatternDifference(pattern, pImpl->patternMemory[i]);
            totalDeviation += dev * pImpl->patternWeights[i];
            totalWeight += pImpl->patternWeights[i];
        }
    }
    
    if (totalWeight > 0.0f) {
        return totalDeviation / totalWeight;
    }
    
    // If no patterns in memory, use baseline deviation
    float baselineDev = computePatternDifference(pattern, pImpl->lastPattern);
    return baselineDev * 1.5f;  // Scale up when no memory
}

float Novelty::computeWorkingMemoryMatch(const std::vector<float>& pattern) const {
    if (pImpl->workingMemoryBuffer.empty() || pattern.empty()) {
        return 0.0f;
    }
    
    float similarity = computePatternDifference(pattern, pImpl->workingMemoryBuffer);
    return 1.0f - std::min(1.0f, similarity);
}

void Novelty::updatePatternMemory(const std::vector<float>& pattern) {
    if (pattern.empty()) return;
    
    // Check if this pattern already exists
    for (size_t i = 0; i < pImpl->patternMemory.size(); ++i) {
        if (computePatternDifference(pattern, pImpl->patternMemory[i]) < 0.05f) {
            // Update weight of existing pattern
            pImpl->patternWeights[i] = std::min(1.0f, pImpl->patternWeights[i] * 1.1f);
            return;
        }
    }
    
    // Add new pattern if memory not full
    if (pImpl->patternMemory.size() < pImpl->maxMemoryPatterns) {
        pImpl->patternMemory.push_back(pattern);
        pImpl->patternWeights.push_back(0.5f);
    } else {
        // Replace least confident pattern
        size_t minIndex = 0;
        float minWeight = pImpl->patternWeights[0];
        for (size_t i = 1; i < pImpl->patternWeights.size(); ++i) {
            if (pImpl->patternWeights[i] < minWeight) {
                minIndex = i;
                minWeight = pImpl->patternWeights[i];
            }
        }
        
        pImpl->patternMemory[minIndex] = pattern;
        pImpl->patternWeights[minIndex] = 0.3f;
    }
}

void Novelty::updateWorkingMemory(float dt) {
    // Simple working memory update: store recent patterns
    if (!pImpl->lastPattern.empty()) {
        pImpl->workingMemoryBuffer = pImpl->lastPattern;
        
        // Add some noise for temporal dynamics
        for (auto& val : pImpl->workingMemoryBuffer) {
            val += normalDist(pImpl->rng) * 0.02f;
            val = std::clamp(val, 0.0f, 1.0f);
        }
    }
}

} // namespace nlm
