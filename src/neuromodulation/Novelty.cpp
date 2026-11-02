#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float baseline;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    float maxLevel;
    float minLevel;
    float averageLevel;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), 
             baseline(0.0f), noveltyThreshold(0.3f),
             maxLevel(0.0f), minLevel(0.0f), averageLevel(0.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized");
}

const char* Novelty::getName() const {
    return "Novelty";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
    updateLevelStats(level);
}

float Novelty::getPlasticityFactor() const {
    // Novelty increases plasticity for new learning
    return 0.5f + 0.5f * pImpl->level;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty towards baseline
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * 
                   std::exp(-pImpl->decayRate * static_cast<float>(dt));
    updateLevelStats(pImpl->level);
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    if (!observation.isValid() || !previousObservation.isValid()) {
        setLevel(0.0f);
        return;
    }
    
    // Extract features from observations
    std::vector<float> currentPattern = extractFeatures(observation);
    std::vector<float> previousPattern = previousObservation.isValid() ? 
                                         extractFeatures(previousObservation) : 
                                         std::vector<float>();
    
    detectNovelty(currentPattern, previousPattern);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty()) {
        setLevel(0.0f);
        return;
    }
    
    if (previousPattern.empty()) {
        // First observation - moderate novelty
        setLevel(0.5f);
        pImpl->lastPattern = currentPattern;
        updateLevelStats(pImpl->level);
        return;
    }
    
    // Compute difference between patterns using multiple metrics
    float totalDifference = computePatternDifference(currentPattern, previousPattern);
    float normalizedDifference = totalDifference / static_cast<float>(currentPattern.size());
    
    // Update novelty level based on difference
    float novelLevel = std::min(1.0f, normalizedDifference / pImpl->noveltyThreshold);
    setLevel(novelLevel);
    
    // Track history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::max(0.0f, std::min(1.0f, level));
    updateLevelStats(pImpl->level);
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::max(0.01f, threshold);
}

float Novelty::getNoveltyLevel() const {
    return pImpl->level;
}

float Novelty::getNoveltyThreshold() const {
    return pImpl->noveltyThreshold;
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->maxLevel = pImpl->baseline;
    pImpl->minLevel = pImpl->baseline;
    pImpl->averageLevel = pImpl->baseline;
}

void Novelty::setBaseline(float baseline) {
    pImpl->baseline = baseline;
    if (pImpl->level < baseline) {
        setLevel(baseline);
    }
    updateLevelStats(pImpl->level);
}

void Novelty::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Novelty::setReleaseRate(float rate) {
    // Novelty release rate affects how quickly novelty decays
    // Higher rate means faster decay
    pImpl->decayRate = rate;
}

float Novelty::getMaxLevel() const {
    return pImpl->maxLevel;
}

float Novelty::getMinLevel() const {
    return pImpl->minLevel;
}

float Novelty::getAverageLevel() const {
    return pImpl->averageLevel;
}

void Novelty::updateLevelStats(float level) {
    pImpl->maxLevel = std::max(pImpl->maxLevel, level);
    pImpl->minLevel = std::min(pImpl->minLevel, level);
    
    if (!pImpl->history.empty()) {
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        pImpl->averageLevel = sum / static_cast<float>(pImpl->history.size());
    } else {
        pImpl->averageLevel = level;
    }
}

// Pattern extraction and difference computation

std::vector<float> Novelty::extractFeatures(const Observation& observation) const {
    if (!observation.isValid()) {
        return std::vector<float>();
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return std::vector<float>();
    }
    
    return extractFeatures(*sensoryInput);
}

std::vector<float> Novelty::extractFeatures(const SensoryInput& sensoryInput) const {
    const std::vector<float>& data = sensoryInput.getData();
    
    if (data.empty()) {
        return std::vector<float>();
    }
    
    // Extract multiple features based on input type
    std::vector<float> features;
    
    if (strcmp(sensoryInput.getType(), "Vision") == 0) {
        features = extractVisualFeatures(data);
    } else if (strcmp(sensoryInput.getType(), "Audio") == 0) {
        features = extractAudioFeatures(data);
    } else if (strcmp(sensoryInput.getType(), "InternalSignals") == 0) {
        features = extractInternalFeatures(data);
    } else {
        // Generic feature extraction
        features = extractGenericFeatures(data);
    }
    
    return features;
}

std::vector<float> Novelty::extractVisualFeatures(const std::vector<float>& data) const {
    // Extract visual features: statistics, gradients, textures
    std::vector<float> features;
    
    if (data.size() < 3) {
        features.push_back(0.0f);
        return features;
    }
    
    // Compute statistics
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float mean = sum / data.size();
    
    float variance = 0.0f;
    for (float val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    
    float stddev = std::sqrt(variance);
    
    features.push_back(mean);
    features.push_back(stddev);
    
    // Compute gradients (edges)
    float totalGradient = 0.0f;
    for (size_t i = 1; i < data.size(); ++i) {
        totalGradient += std::abs(data[i] - data[i-1]);
    }
    features.push_back(totalGradient / (data.size() - 1));
    
    // Compute texture complexity (gradient variance)
    float gradMean = totalGradient / (data.size() - 1);
    float gradVariance = 0.0f;
    for (size_t i = 1; i < data.size(); ++i) {
        float grad = std::abs(data[i] - data[i-1]);
        gradVariance += (grad - gradMean) * (grad - gradMean);
    }
    gradVariance /= (data.size() - 1);
    features.push_back(std::sqrt(gradVariance));
    
    return features;
}

std::vector<float> Novelty::extractAudioFeatures(const std::vector<float>& data) const {
    // Extract audio features: spectral, rhythmic, temporal
    std::vector<float> features;
    
    if (data.size() < 3) {
        features.push_back(0.0f);
        return features;
    }
    
    // Spectral characteristics
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float mean = sum / data.size();
    
    float variance = 0.0f;
    for (float val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    
    float spectralFlatness = std::sqrt(variance) / (std::abs(mean) + 0.01f);
    features.push_back(spectralFlatness);
    
    // Rhythm characteristics
    float zeroCrossings = 0.0f;
    for (size_t i = 1; i < data.size(); ++i) {
        if ((data[i] >= 0.0f && data[i-1] < 0.0f) || (data[i] < 0.0f && data[i-1] >= 0.0f)) {
            zeroCrossings++;
        }
    }
    float rhythmStrength = zeroCrossings / (data.size() - 1);
    features.push_back(rhythmStrength);
    
    // Temporal dynamics
    float autocorrelation = computeAutocorrelation(data);
    features.push_back(autocorrelation);
    
    return features;
}

std::vector<float> Novelty::extractInternalFeatures(const std::vector<float>& data) const {
    // Extract internal neural state features
    std::vector<float> features;
    
    if (data.empty()) {
        features.push_back(0.0f);
        return features;
    }
    
    // Statistics
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float mean = sum / data.size();
    
    float variance = 0.0f;
    for (float val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    
    float coherence = std::sqrt(variance) / (std::abs(mean) + 0.01f);
    features.push_back(coherence);
    
    // Activation patterns
    float activationLevel = mean;
    features.push_back(activationLevel);
    
    // Pattern regularity
    float regularity = computePatternRegularity(data);
    features.push_back(regularity);
    
    return features;
}

std::vector<float> Novelty::extractGenericFeatures(const std::vector<float>& data) const {
    // Generic feature extraction for unknown input types
    std::vector<float> features;
    
    if (data.empty()) {
        features.push_back(0.0f);
        return features;
    }
    
    // Basic statistics
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float mean = sum / data.size();
    
    float variance = 0.0f;
    for (float val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    
    float stddev = std::sqrt(variance);
    
    features.push_back(mean);
    features.push_back(stddev);
    
    return features;
}

float Novelty::computePatternDifference(const std::vector<float>& pattern1,
                                        const std::vector<float>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) {
        return 0.0f;
    }
    
    size_t compareLen = std::min(pattern1.size(), pattern2.size());
    float totalDifference = 0.0f;
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(pattern1[i] - pattern2[i]);
        totalDifference += diff;
    }
    
    return totalDifference;
}

float Novelty::computeAutocorrelation(const std::vector<float>& data) const {
    if (data.size() < 3) return 0.0f;
    
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float mean = sum / data.size();
    
    float autocorrelation = 0.0f;
    size_t lag = std::min(static_cast<size_t>(data.size() / 2), static_cast<size_t>(10));
    
    for (size_t i = 0; i < lag; ++i) {
        if (i + 1 < data.size()) {
            autocorrelation += (data[i] - mean) * (data[i + 1] - mean);
        }
    }
    
    // Normalize by variance
    float variance = 0.0f;
    for (float val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    
    if (variance > 0.0f) {
        autocorrelation /= variance;
    }
    
    return autocorrelation;
}

float Novelty::computePatternRegularity(const std::vector<float>& data) const {
    if (data.size() < 3) return 0.0f;
    
    // Compute regularity based on local gradients
    float totalVariation = 0.0f;
    for (size_t i = 1; i < data.size(); ++i) {
        totalVariation += std::abs(data[i] - data[i-1]);
    }
    
    float meanVariation = totalVariation / (data.size() - 1);
    
    // Regularity is inverse of variation (lower variation = higher regularity)
    float regularity = 1.0f / (meanVariation + 0.01f);
    return std::min(1.0f, regularity);
}

} // namespace nlm
