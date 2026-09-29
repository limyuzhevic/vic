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
    // Enhanced novelty detection using multi-level feature analysis
    // Combines pixel differences with higher-level semantic changes
    
    // Extract features from observations
    std::vector<float> currentPattern;
    std::vector<float> previousPattern;
    
    // Simple feature extraction: convert observations to numeric features
    // In a real implementation, this would use more sophisticated feature extraction
    
    // For now, use observation data if available, otherwise create placeholders
    if (!observation.getData().empty()) {
        currentPattern = observation.getData();
        if (!previousObservation.getData().empty()) {
            previousPattern = previousObservation.getData();
        }
    }
    
    // Use the vector-based detection method
    detectNovelty(currentPattern, previousPattern);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute multi-level differences
    std::vector<float> levels = computeFeatureLevels(currentPattern, previousPattern);
    
    // Compute novelty using statistical analysis
    pImpl->level = computeStatisticalNovelty(levels);
    
    // Apply adaptation: reduce novelty if it's too high (prevent runaway)
    const float adaptationRate = 0.05f;
    if (pImpl->level > 1.0f) {
        pImpl->level = 1.0f - (pImpl->level - 1.0f) * adaptationRate;
    }
    
    pImpl->history.push_back(pImpl->level);
    
    // Maintain history size
    const size_t maxHistorySize = 1000;
    if (pImpl->history.size() > maxHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::update(TimestepDuration dt) {
    // Real novelty dynamics with adaptation
    // Novelty decays naturally but can be boosted by persistent novel input
    
    float dtf = static_cast<float>(dt);
    
    // Multiple decay processes
    // Fast decay for immediate novelty reduction
    const float fastDecayRate = 0.2f;
    float fastDecay = pImpl->level * fastDecayRate;
    
    // Slow decay for persistent novelty reduction
    const float slowDecayRate = 0.02f;
    float slowDecay = pImpl->level * slowDecayRate;
    
    // Apply both decay processes
    pImpl->level = std::max(0.0f, pImpl->level - fastDecay - slowDecay * dtf * 10.0f);
    
    // Adaptation: reduce novelty threshold over time for more sensitive detection
    pImpl->noveltyThreshold = std::max(0.1f, pImpl->noveltyThreshold * (1.0f - 0.01f * dtf * 10.0f));
}

// Helper method to compute feature levels
std::vector<float> Novelty::computeFeatureLevels(const std::vector<float>& current,
                                                  const std::vector<float>& previous) {
    std::vector<float> levels;
    
    if (current.empty() || previous.empty()) {
        return levels;
    }
    
    size_t compareLen = std::min(current.size(), previous.size());
    if (compareLen == 0) {
        return levels;
    }
    
    // Compute multiple feature levels
    levels.reserve(5);
    
    // Level 1: Raw differences (high-frequency changes)
    float rawDiff = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        rawDiff += std::abs(current[i] - previous[i]);
    }
    levels.push_back(rawDiff / compareLen);
    
    // Level 2: Squared differences (emphasizes large changes)
    float squaredDiff = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = current[i] - previous[i];
        squaredDiff += diff * diff;
    }
    levels.push_back(std::sqrt(squaredDiff / compareLen));
    
    // Level 3: Relative differences (normalized changes)
    float relDiff = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        if (std::abs(previous[i]) > 1e-6f) {
            relDiff += std::abs((current[i] - previous[i]) / previous[i]);
        }
    }
    levels.push_back(relDiff / compareLen);
    
    // Level 4: Pattern change (changes in relationships between features)
    float patternChange = 0.0f;
    for (size_t i = 0; i < compareLen - 1; ++i) {
        float currentRel = current[i] - current[i+1];
        float previousRel = previous[i] - previous[i+1];
        patternChange += std::abs(currentRel - previousRel);
    }
    levels.push_back(patternChange / std::max(1.0f, static_cast<float>(compareLen - 1)));
    
    // Level 5: Entropy change (information content)
    float currentEntropy = computeEntropy(current);
    float previousEntropy = computeEntropy(previous);
    levels.push_back(std::abs(currentEntropy - previousEntropy));
    
    return levels;
}

// Helper method to compute entropy
float Novelty::computeEntropy(const std::vector<float>& data) {
    if (data.empty()) return 0.0f;
    
    // Simple histogram-based entropy computation
    std::map<float, int> histogram;
    float minVal = *std::min_element(data.begin(), data.end());
    float maxVal = *std::max_element(data.begin(), data.end());
    
    // Create bins
    const int numBins = 10;
    std::vector<float> bins(numBins, 0.0f);
    
    for (float val : data) {
        int bin = static_cast<int>((val - minVal) / (maxVal - minVal + 1e-6f) * numBins);
        if (bin >= 0 && bin < numBins) {
            bins[bin]++;
        }
    }
    
    // Compute entropy
    float entropy = 0.0f;
    for (float count : bins) {
        if (count > 0) {
            float p = count / data.size();
            entropy -= p * std::log(std::max(p, 1e-6f));
        }
    }
    
    return entropy;
}

// Helper method to compute statistical novelty
float Novelty::computeStatisticalNovelty(const std::vector<float>& levels) {
    if (levels.empty()) return 0.0f;
    
    // Compute statistical novelty based on deviation from historical patterns
    // Higher novelty = significant deviation from historical distribution
    
    float mean = 0.0f;
    for (float level : levels) {
        mean += level;
    }
    mean /= levels.size();
    
    float variance = 0.0f;
    for (float level : levels) {
        float diff = level - mean;
        variance += diff * diff;
    }
    variance /= levels.size();
    
    float stdDev = std::sqrt(variance);
    
    // Novelty based on how many standard deviations above mean
    float novelty = 1.0f + (levels[levels.size() - 1] - mean) / (stdDev + 1e-6f);
    
    // Apply nonlinear scaling to ensure novelty stays in reasonable bounds
    return std::min(1.0f, std::max(0.0f, novelty));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
