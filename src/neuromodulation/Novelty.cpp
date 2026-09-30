#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float adaptationRate;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    float patternMemory[20];  // Store recent patterns for comparison
    int patternIndex;
    float maxNovelty;
    float learningRate;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), adaptationRate(0.05f),
             noveltyThreshold(0.3f), patternIndex(0), maxNovelty(0.0f),
             learningRate(0.1f) {
        for (int i = 0; i < 20; ++i) {
            patternMemory[i] = 0.0f;
        }
    }
};

Novelty::Novelty() : pImpl(new Impl) {
    NLM_LOG_INFO("Novelty detection enhanced initialized");
}

Novelty::~Novelty() {
    NLM_LOG_INFO("Novelty detection enhanced destroyed");
}

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection enhanced initialized with %d pattern memory slots", 20);
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Enhanced novelty detection using sensory features
    // Placeholder for observation-based novelty detection
    pImpl->level = 1.0f;  // Maximum novelty for placeholder
    pImpl->history.push_back(pImpl->level);
    
    // Store pattern in memory
    pImpl->patternMemory[pImpl->patternIndex] = pImpl->level;
    pImpl->patternIndex = (pImpl->patternIndex + 1) % 20;
    
    // Update max novelty
    pImpl->maxNovelty = std::max(pImpl->maxNovelty, pImpl->level);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Enhanced novelty detection using multiple similarity measures
    std::vector<float> similarities;
    
    // 1. Euclidean distance
    float euclideanDist = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = currentPattern[i] - previousPattern[i];
        euclideanDist += diff * diff;
    }
    euclideanDist = std::sqrt(euclideanDist);
    similarities.push_back(1.0f - std::min(1.0f, euclideanDist / 2.0f));  // Normalize
    
    // 2. Cosine similarity
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        dotProduct += currentPattern[i] * previousPattern[i];
        normA += currentPattern[i] * currentPattern[i];
        normB += previousPattern[i] * previousPattern[i];
    }
    normA = std::sqrt(normA);
    normB = std::sqrt(normB);
    if (normA > 0.0f && normB > 0.0f) {
        float cosineSim = dotProduct / (normA * normB);
        similarities.push_back(cosineSim);
    } else {
        similarities.push_back(0.0f);
    }
    
    // 3. Statistical deviation (mean and std dev)
    float meanCurrent = 0.0f;
    float meanPrevious = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        meanCurrent += currentPattern[i];
        meanPrevious += previousPattern[i];
    }
    meanCurrent /= compareLen;
    meanPrevious /= compareLen;
    
    float stdDevCurrent = 0.0f;
    float stdDevPrevious = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        stdDevCurrent += (currentPattern[i] - meanCurrent) * (currentPattern[i] - meanCurrent);
        stdDevPrevious += (previousPattern[i] - meanPrevious) * (previousPattern[i] - meanPrevious);
    }
    stdDevCurrent = std::sqrt(stdDevCurrent / compareLen);
    stdDevPrevious = std::sqrt(stdDevPrevious / compareLen);
    
    float statsSimilarity = 0.0f;
    if (stdDevCurrent > 0.0f && stdDevPrevious > 0.0f) {
        // Correlation of deviations
        float covariance = 0.0f;
        for (size_t i = 0; i < compareLen; ++i) {
            covariance += (currentPattern[i] - meanCurrent) * (previousPattern[i] - meanPrevious);
        }
        statsSimilarity = covariance / (stdDevCurrent * stdDevPrevious);
    }
    similarities.push_back(statsSimilarity);
    
    // Compute final novelty as 1 - average similarity
    float avgSimilarity = 0.0f;
    for (float sim : similarities) {
        avgSimilarity += sim;
    }
    avgSimilarity /= similarities.size();
    
    pImpl->level = 1.0f - avgSimilarity;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Store in history
    pImpl->history.push_back(pImpl->level);
    
    // Store pattern in memory
    pImpl->patternMemory[pImpl->patternIndex] = pImpl->level;
    pImpl->patternIndex = (pImpl->patternIndex + 1) % 20;
    
    // Update max novelty
    pImpl->maxNovelty = std::max(pImpl->maxNovelty, pImpl->level);
    
    // Adapt to new patterns over time
    pImpl->adaptationRate = 0.05f + pImpl->level * 0.05f;  // More adaptation for novel patterns
}

void Novelty::update(TimestepDuration dt) {
    // Enhanced novelty dynamics with adaptation
    float timeFactor = static_cast<float>(dt) / 100.0f;
    
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * timeFactor);
    
    // Adapt to environment based on max novelty seen
    pImpl->level += (pImpl->maxNovelty - pImpl->level) * pImpl->adaptationRate * timeFactor;
    
    // Enhanced decay for high novelty (fading effect)
    if (pImpl->level > 0.7f) {
        pImpl->level *= (1.0f - pImpl->decayRate * 2.0f * timeFactor);
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->maxNovelty = 0.0f;
}

float Novelty::getMaxNovelty() const {
    return pImpl->maxNovelty;
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = threshold;
}

bool Novelty::isNovel(const std::vector<float>& pattern, float customThreshold) const {
    float threshold = customThreshold > 0.0f ? customThreshold : pImpl->noveltyThreshold;
    return pImpl->level > threshold;
}

float Novelty::getPatternSimilarity(const std::vector<float>& pattern1,
                                   const std::vector<float>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    size_t compareLen = std::min(pattern1.size(), pattern2.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        normA += pattern1[i] * pattern1[i];
        normB += pattern2[i] * pattern2[i];
    }
    
    normA = std::sqrt(normA);
    normB = std::sqrt(normB);
    
    if (normA > 0.0f && normB > 0.0f) {
        return dotProduct / (normA * normB);
    }
    return 0.0f;
}

} // namespace nlm