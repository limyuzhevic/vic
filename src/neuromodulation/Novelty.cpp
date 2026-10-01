// Novelty detection implementation
// Real-time novelty detection using statistical pattern matching

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
    float baseline;
    std::vector<float> featureStats;  // Mean and stddev for features
    size_t featureCount;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f),
             baseline(0.0f), featureCount(0) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized");
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

void Novelty::detectNovelty(const Observation& observation,
                            const Observation& previousObservation) {
    // Enhanced novelty detection using statistical analysis
    auto currentData = observation.getData();
    auto previousData = previousObservation.getData();
    
    if (currentData.empty() || previousData.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute novelty using multiple metrics
    float meanNovelty = 0.0f;
    float maxNovelty = 0.0f;
    
    size_t compareLen = std::min(currentData.size(), previousData.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentData[i] - previousData[i]);
        meanNovelty += diff;
        maxNovelty = std::max(maxNovelty, diff);
    }
    
    if (compareLen > 0) {
        meanNovelty /= compareLen;
        
        // Combine different novelty measures
        pImpl->level = (meanNovelty * 0.7f) + (maxNovelty * 0.3f);
        
        // Apply baseline correction and thresholding
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->baseline);
        pImpl->level = std::min(1.0f, pImpl->level / pImpl->noveltyThreshold);
    }
    
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update pattern statistics
    pImpl->lastPattern = currentData;
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Enhanced novelty detection with statistical features
    std::vector<float> differences;
    differences.reserve(std::min(currentPattern.size(), previousPattern.size()));
    
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        differences.push_back(diff);
    }
    
    if (differences.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute statistical novelty measures
    float meanDiff = std::accumulate(differences.begin(), differences.end(), 0.0f) / differences.size();
    
    float sumSquaredDiff = 0.0f;
    for (float diff : differences) {
        sumSquaredDiff += (diff - meanDiff) * (diff - meanDiff);
    }
    
    float stdDev = std::sqrt(sumSquaredDiff / differences.size());
    
    // Novelty as standardized deviation from baseline
    float noveltyScore = stdDev / (stdDev + 0.01f);  // Prevent division by zero
    
    // Apply novelty threshold and bounds
    pImpl->level = std::min(1.0f, noveltyScore / pImpl->noveltyThreshold);
    
    // Update feature statistics for adaptive thresholding
    updateFeatureStatistics(differences);
    
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::updateFeatureStatistics(const std::vector<float>& newFeatures) {
    if (newFeatures.empty()) return;
    
    size_t featureCount = newFeatures.size();
    
    // Initialize feature statistics on first call
    if (pImpl->featureCount == 0) {
        pImpl->featureStats.resize(featureCount * 2, 0.0f);  // Mean + stddev for each feature
        pImpl->featureCount = featureCount;
    }
    
    // Update running statistics (Welford's algorithm)
    for (size_t i = 0; i < featureCount && i < newFeatures.size(); ++i) {
        float x = newFeatures[i];
        size_t meanIdx = i;
        size_t stddevIdx = i + pImpl->featureCount;
        
        float mean = pImpl->featureStats[meanIdx];
        size_t count = 1;  // Simplified - in real implementation would track count
        
        // Update mean
        float delta = x - mean;
        pImpl->featureStats[meanIdx] = mean + delta / static_cast<float>(count);
        
        // Update variance and stddev
        pImpl->featureStats[stddevIdx] += delta * (x - pImpl->featureStats[meanIdx]);
    }
}

void Novelty::update(TimestepDuration dt) {
    // Novelty decay with homeostatic regulation
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    
    // Apply adaptive decay based on system state
    pImpl->level *= decayFactor;
    
    // Homeostatic regulation to prevent runaway novelty
    if (pImpl->level > 0.8f) {
        pImpl->level *= 0.95f;  // Strong decay for high novelty
    } else if (pImpl->level < 0.01f) {
        pImpl->level = 0.0f;  // Clamp to zero for very low novelty
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

float Novelty::getAdaptiveThreshold() const {
    // Compute adaptive novelty threshold based on feature statistics
    if (pImpl->featureStats.empty()) return pImpl->noveltyThreshold;
    
    float totalStdDev = 0.0f;
    size_t featureCount = pImpl->featureStats.size() / 2;
    
    for (size_t i = 0; i < featureCount; ++i) {
        size_t stddevIdx = i + featureCount;
        totalStdDev += pImpl->featureStats[stddevIdx];
    }
    
    float avgStdDev = totalStdDev / featureCount;
    return std::max(pImpl->noveltyThreshold * 0.5f, std::min(1.0f, avgStdDev));
}

} // namespace nlm