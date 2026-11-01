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
    // Phase 6: Implement real novelty detection
    // Extract and compare features from observations
    // Connect to working memory and episodic memory for context
    
    // Extract features from current observation
    std::vector<float> currentPattern;
    extractFeatures(observation, currentPattern);
    
    // Compare with previous observation
    detectNovelty(currentPattern, pImpl->lastPattern);
    
    // In Phase 6, novelty should also consider:
    // - Brain state changes (working memory updates)
    // - Previous experiences (episodic memory retrieval)
    // - Developmental stage effects on novelty detection
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute multiple novelty measures:
    // 1. Simple difference measure
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDiff += diff;
    }
    
    float avgDiff = totalDiff / compareLen;
    
    // 2. Statistical surprise (how unlikely the current pattern is)
    float statisticalNovelty = computeStatisticalNovelty(currentPattern);
    
    // 3. Pattern change complexity
    float complexity = computePatternComplexity(currentPattern);
    
    // Combine multiple novelty measures
    pImpl->level = std::min(1.0f, 
                         0.4f * (avgDiff / pImpl->noveltyThreshold) +  // Pattern difference
                         0.3f * statisticalNovelty +                   // Statistical surprise
                         0.3f * complexity);                         // Pattern complexity
    
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
    
    // In Phase 6, novelty would also:
    // - Integrate with working memory to detect unexpected patterns
    // - Connect to episodic memory for contextual novelty
    // - Consider developmental stage effects on novelty detection
}

void Novelty::update(TimestepDuration dt) {
    // Phase 6: Implement sophisticated novelty dynamics
    // Multi-timescale decay and integration with other systems
    
    // Fast decay for immediate novelty
    float fastDecay = 1.0f - (pImpl->decayRate * 3.0f * static_cast<float>(dt));
    pImpl->level = std::max(0.0f, pImpl->level * fastDecay);
    
    // Background decay for sustained novelty levels
    float backgroundDecay = 1.0f - (pImpl->decayRate * 0.1f * static_cast<float>(dt));
    pImpl->level = std::max(0.0f, pImpl->level * backgroundDecay);
    
    // In Phase 6, integrate with:
    // - Dopamine system for reward-based novelty modulation
    // - Working memory for pattern-based novelty
    // - Episodic memory for contextual novelty
}

void Novelty::extractFeatures(const Observation& observation, 
                             std::vector<float>& features) {
    // Extract meaningful features from observation
    // Connect to sensory processing and working memory
    
    // For Phase 6, this should:
    // 1. Extract relevant features from sensory input
    // 2. Compare with stored patterns in working memory
    // 3. Consider developmental stage effects on feature extraction
    // 4. Incorporate prediction errors for adaptive feature detection
    
    // For now, create a simple placeholder feature vector
    features = observation.getData();
}

float Novelty::computeStatisticalNovelty(const std::vector<float>& pattern) const {
    // Compute how statistically novel a pattern is
    // Phase 6: Use history and probability distributions
    
    if (pImpl->history.empty()) {
        return 0.5f;  // Neutral novelty
    }
    
    // Calculate mean and standard deviation from history
    float sum = 0.0f;
    for (float val : pImpl->history) {
        sum += val;
    }
    float mean = sum / pImpl->history.size();
    
    float sqSum = 0.0f;
    for (float val : pImpl->history) {
        float diff = val - mean;
        sqSum += diff * diff;
    }
    float stddev = std::sqrt(sqSum / pImpl->history.size());
    
    // Normalize the current pattern value
    float patternVal = 0.0f;
    if (!pattern.empty()) {
        patternVal = pattern[0];  // Use first feature as placeholder
    }
    
    // Calculate z-score (how many standard deviations away from mean)
    float zScore = 0.0f;
    if (stddev > 0.0f) {
        zScore = std::abs(patternVal - mean) / stddev;
    }
    
    return std::min(1.0f, zScore / 2.0f);
}

float Novelty::computePatternComplexity(const std::vector<float>& pattern) const {
    // Compute pattern complexity (how diverse/unpredictable the pattern is)
    if (pattern.empty()) return 0.0f;
    
    // Calculate variance of pattern features
    float sum = 0.0f;
    for (float val : pattern) {
        sum += val;
    }
    float mean = sum / pattern.size();
    
    float variance = 0.0f;
    for (float val : pattern) {
        float diff = val - mean;
        variance += diff * diff;
    }
    variance /= pattern.size();
    
    return std::sqrt(variance) / 10.0f;  // Normalize to [0,1]
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
