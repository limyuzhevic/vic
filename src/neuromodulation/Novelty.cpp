#include "Novelty.hpp"
#include "../world/Observation.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <numeric>

namespace nlm {

struct Novelty::Impl {
    // Brain reference for neural integration
    class Brain* brain;
    
    // Novelty state
    float level;                    // Current novelty level
    float threshold;                // Novelty detection threshold
    float patternThreshold;         // Pattern matching threshold
    float adaptationRate;           // Rate of adaptation to new patterns
    
    // Pattern recognition
    std::vector<float> lastPattern; // Previous sensory pattern
    std::vector<float> patternBuffer; // Pattern history buffer
    std::vector<float> noveltyBuffer; // Novelty history buffer
    
    // Statistical components
    float noveltyMean;              // Mean novelty level
    float noveltyVariance;          // Variance of novelty levels
    float lastNoveltyChange;        // Last change in novelty
    
    // Neural dynamics
    float decayRate;                // Natural decay rate
    float attentionModulation;      // Attention modulation of novelty
    float memoryIntegration;        // Memory integration for novelty
    
    // Processing components
    float patternSimilarity;        // Similarity to previous patterns
    float featureSalience;          // Salience of features
    float contextSensitivity;       // Sensitivity to context changes
    
    // Statistics
    std::vector<float> history;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    // State flags
    bool hasPreviousObservation;    // Whether we have a previous observation
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , threshold(0.3f)
        , patternThreshold(0.5f)
        , adaptationRate(0.2f)
        , noveltyMean(0.0f)
        , noveltyVariance(0.0f)
        , lastNoveltyChange(0.0f)
        , decayRate(0.1f)
        , attentionModulation(1.0f)
        , memoryIntegration(0.7f)
        , patternSimilarity(0.0f)
        , featureSalience(0.0f)
        , contextSensitivity(0.5f)
        , hasPreviousObservation(false) {
        
        // Seed RNG with chrono for biological variability
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
        dist = std::uniform_real_distribution<float>(0.9f, 1.1f);
    }
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized with neural dynamics");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::detectNovelty(const Observation& observation,
                           const Observation& previousObservation) {
    // Detect novelty from observation objects
    const std::vector<float>& currentPattern = observation.getData();
    const std::vector<float>& previousPattern = previousObservation.getData();
    
    detectNovelty(currentPattern, previousPattern);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    // Real novelty detection: biologically-inspired neural implementation
    
    if (currentPattern.empty()) {
        NLM_LOG_WARNING("Empty current pattern in novelty detection");
        pImpl->level = 0.0f;
        pImpl->hasPreviousObservation = false;
        return;
    }
    
    // Store previous pattern if needed
    if (!pImpl->lastPattern.empty()) {
        pImpl->lastPattern = previousPattern;
    }
    
    // Compute pattern similarity
    pImpl->patternSimilarity = computePatternSimilarity(currentPattern, previousPattern);
    
    // Compute feature salience (variance and contrast)
    pImpl->featureSalience = computeFeatureSalience(currentPattern);
    
    // Context sensitivity based on pattern change
    float patternChange = computePatternChange(currentPattern, previousPattern);
    pImpl->contextSensitivity = std::min(1.0f, patternChange * 2.0f);
    
    // Novelty computation
    float noveltyFromSimilarity = 1.0f - pImpl->patternSimilarity;
    float noveltyFromSalience = pImpl->featureSalience * pImpl->contextSensitivity;
    
    // Weighted combination
    float targetNovelty = noveltyFromSimilarity * 0.6f + noveltyFromSalience * 0.4f;
    
    // Temporal integration with neural dynamics
    float learningRate = pImpl->adaptationRate * pImpl->attentionModulation * pImpl->memoryIntegration;
    float noveltyUpdate = (targetNovelty - pImpl->level) * learningRate;
    
    // Add biological noise and constraints
    float biologicalNoise = (dist(pImpl->rng) - 1.0f) * 0.02f;
    pImpl->level += noveltyUpdate + biologicalNoise;
    pImpl->level = std::max(0.0f, std::min(1.0f, pImpl->level));
    
    // Update statistics
    pImpl->lastNoveltyChange = std::abs(targetNovelty - pImpl->level);
    
    // Update history
    pImpl->history.push_back(pImpl->level);
    pImpl->patternBuffer.push_back(currentPattern.size() > 0 ? currentPattern.back() : 0.0f);
    
    // Trim history
    const size_t maxHistory = 1000;
    if (pImpl->history.size() > maxHistory) {
        pImpl->history.erase(pImpl->history.begin());
        pImpl->patternBuffer.erase(pImpl->patternBuffer.begin());
    }
    
    pImpl->hasPreviousObservation = true;
    pImpl->lastPattern = currentPattern;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(0.0f, pImpl->level - decay);
    
    // Update statistics
    updateStatistics();
    
    // Store history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->patternBuffer.clear();
    pImpl->noveltyBuffer.clear();
    pImpl->noveltyMean = 0.0f;
    pImpl->noveltyVariance = 0.0f;
    pImpl->lastNoveltyChange = 0.0f;
}

float Novelty::getNoveltyMean() const {
    return pImpl->noveltyMean;
}

float Novelty::getNoveltyVariance() const {
    return pImpl->noveltyVariance;
}

float Novelty::getLastNoveltyChange() const {
    return pImpl->lastNoveltyChange;
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->threshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Novelty::setPatternMatchingThreshold(float threshold) {
    pImpl->patternThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Novelty::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.0f, 1.0f);
}

// Helper functions

float Novelty::computePatternSimilarity(const std::vector<float>& pattern1,
                                       const std::vector<float>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) {
        return 0.0f;
    }
    
    // Compute normalized cross-correlation as similarity measure
    float similarity = 0.0f;
    size_t minSize = std::min(pattern1.size(), pattern2.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        similarity += pattern1[i] * pattern2[i];
    }
    
    float norm1 = computeVectorNorm(pattern1);
    float norm2 = computeVectorNorm(pattern2);
    
    if (norm1 > 0.0f && norm2 > 0.0f) {
        similarity /= (norm1 * norm2);
    }
    
    return std::max(0.0f, std::min(1.0f, similarity));
}

float Novelty::computeVectorNorm(const std::vector<float>& vec) const {
    float sum = 0.0f;
    for (float val : vec) {
        sum += val * val;
    }
    return std::sqrt(sum);
}

float Novelty::computeFeatureSalience(const std::vector<float>& pattern) const {
    if (pattern.empty()) {
        return 0.0f;
    }
    
    // Compute variance as a measure of feature salience
    float mean = computeMean(pattern);
    float variance = 0.0f;
    
    for (float val : pattern) {
        float diff = val - mean;
        variance += diff * diff;
    }
    
    variance /= static_cast<float>(pattern.size());
    
    // Normalize and scale
    return std::min(1.0f, variance * 2.0f);
}

float Novelty::computeMean(const std::vector<float>& vec) const {
    if (vec.empty()) {
        return 0.0f;
    }
    
    float sum = std::accumulate(vec.begin(), vec.end(), 0.0f);
    return sum / static_cast<float>(vec.size());
}

float Novelty::computePatternChange(const std::vector<float>& pattern1,
                                   const std::vector<float>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) {
        return 1.0f; // Maximum change if no pattern available
    }
    
    float change = 0.0f;
    size_t minSize = std::min(pattern1.size(), pattern2.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        change += std::abs(pattern1[i] - pattern2[i]);
    }
    
    return change / static_cast<float>(minSize);
}

void Novelty::updateStatistics() {
    if (pImpl->history.empty()) {
        return;
    }
    
    // Compute mean
    float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
    pImpl->noveltyMean = sum / static_cast<float>(pImpl->history.size());
    
    // Compute variance
    float varianceSum = 0.0f;
    for (float val : pImpl->history) {
        float diff = val - pImpl->noveltyMean;
        varianceSum += diff * diff;
    }
    pImpl->noveltyVariance = varianceSum / static_cast<float>(pImpl->history.size());
}

} // namespace nlm
