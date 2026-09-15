#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include "../world/Observation.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

// Novelty implementation
struct Novelty::Impl {
    float level;
    float decayRate;
    float noveltyThreshold;
    float statisticalThreshold;
    
    std::vector<float> history;
    std::vector<float> lastPattern;
    std::vector<float> patternHistory;
    
    std::unique_ptr<RandomGenerator> rng;
    
    Impl()
        : level(0.0f)
        , decayRate(NoveltyConstants::PATTERN_FORGET_RATE)
        , noveltyThreshold(NoveltyConstants::FEATURE_DISTANCE_THRESHOLD)
        , statisticalThreshold(NoveltyConstants::STATISTICAL_THRESHOLD)
        , rng(nullptr) {
        rng = std::make_unique<RandomGenerator>(42);
        patternHistory.reserve(NoveltyConstants::PATTERN_HISTORY_SIZE);
    }
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "Novelty";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Novelty::getPlasticityFactor() const {
    return pImpl->level;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Novelty::reset() {
    pImpl->level = 0.0f;
    pImpl->lastPattern.clear();
    pImpl->history.clear();
}

bool Novelty::isActive() const {
    return pImpl->level > 0.1f;
}

void Novelty::initialize(Brain* brain) {
    // Store brain reference if needed for novelty integration
}

void Novelty::detectNovelty(const Observation& observation) {
    detectNovelty(observation.getData());
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern) {
    if (currentPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute novelty based on difference from previous pattern
    if (!pImpl->lastPattern.empty()) {
        pImpl->level = computeFeatureNovelty(currentPattern, pImpl->lastPattern);
    } else {
        // First observation - store it but no novelty
        pImpl->level = 0.0f;
    }
    
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
    
    // Add to pattern history for statistical analysis
    patternHistory.push_back(pImpl->level);
    if (patternHistory.size() > NoveltyConstants::PATTERN_HISTORY_SIZE) {
        patternHistory.erase(patternHistory.begin());
    }
}

float Novelty::computeFeatureNovelty(const std::vector<float>& current, 
                                     const std::vector<float>& previous) const {
    if (current.empty() || previous.empty()) return 0.0f;
    
    size_t compareLen = std::min(current.size(), previous.size());
    
    // Compute normalized difference
    float totalDiff = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(current[i] - previous[i]);
        totalDiff += diff;
    }
    
    float avgDiff = totalDiff / static_cast<float>(compareLen);
    
    // Normalize by typical range (0-1)
    float novelty = avgDiff / pImpl->noveltyThreshold;
    
    // Apply statistical novelty component
    float statisticalNovelty = computeStatisticalNovelty(current);
    
    // Combine both measures
    float combinedNovelty = (novelty * 0.7f) + (statisticalNovelty * 0.3f);
    
    return std::min(1.0f, combinedNovelty);
}

float Novelty::computeStatisticalNovelty(const std::vector<float>& pattern) const {
    if (patternHistory.size() < NoveltyConstants::STATISTICAL_SAMPLE_SIZE) {
        return 0.0f;
    }
    
    // Simple statistical test: check if current pattern deviates significantly
    // from historical mean
    float patternMean = 0.0f;
    for (float value : patternHistory) {
        patternMean += value;
    }
    patternMean /= static_cast<float>(patternHistory.size());
    
    float patternStd = 0.0f;
    for (float value : patternHistory) {
        float diff = value - patternMean;
        patternStd += diff * diff;
    }
    patternStd = std::sqrt(patternStd / static_cast<float>(patternHistory.size()));
    
    if (patternStd < 0.001f) {
        return 0.0f;
    }
    
    // Compute current pattern mean
    float currentMean = 0.0f;
    for (float value : pattern) {
        currentMean += value;
    }
    currentMean /= static_cast<float>(pattern.size());
    
    // Compute standardized difference
    float zScore = std::abs(currentMean - patternMean) / patternStd;
    
    // Convert to novelty score
    float novelty = std::min(1.0f, zScore / pImpl->statisticalThreshold);
    
    return novelty;
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = threshold;
}

void Novelty::setStatisticalThreshold(float threshold) {
    pImpl->statisticalThreshold = threshold;
}

void Novelty::setPatternMemorySize(size_t size) {
    patternHistory.reserve(size);
}

void Novelty::forgetOldPatterns(float rate) {
    pImpl->decayRate = rate;
}

const std::vector<float>& Novelty::getCurrentPattern() const {
    return pImpl->lastPattern;
}

const std::vector<float>& Novelty::getPreviousPattern() const {
    return patternHistory; // This is a simplification - would need to track properly
}

} // namespace nlm
