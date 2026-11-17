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

// Enhanced novelty detection with pattern analysis
void Novelty::detectNovelty(const Observation& observation,
                           const Observation& previousObservation) {
    if (!observation.isValid() || !previousObservation.isValid()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Extract patterns from observations
    const SensoryInput* currentInput = observation.getSensoryInput();
    const SensoryInput* previousInput = previousObservation.getSensoryInput();
    
    if (!currentInput || !previousInput) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Get sensory patterns
    const std::vector<float>& currentPattern = currentInput->getData();
    const std::vector<float>& previousPattern = previousInput->getData();
    
    detectNovelty(currentPattern, previousPattern);
}

// Enhanced novelty detection with feature-based analysis
void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Feature-based novelty detection
    // Compute multiple features for robust novelty detection
    
    // 1. Global intensity change
    float currentMean = 0.0f, previousMean = 0.0f;
    for (float value : currentPattern) currentMean += value;
    for (float value : previousPattern) previousMean += value;
    currentMean /= currentPattern.size();
    previousMean /= previousPattern.size();
    
    float intensityNovelty = std::abs(currentMean - previousMean);
    
    // 2. Statistical variance change
    float currentVariance = 0.0f, previousVariance = 0.0f;
    for (float value : currentPattern) currentVariance += (value - currentMean) * (value - currentMean);
    for (float value : previousPattern) previousVariance += (value - previousMean) * (value - previousMean);
    currentVariance /= currentPattern.size();
    previousVariance /= previousPattern.size();
    
    float varianceNovelty = std::abs(currentVariance - previousVariance);
    
    // 3. Pattern structure change (spectral analysis)
    float patternNovelty = 0.0f;
    if (currentPattern.size() == previousPattern.size()) {
        for (size_t i = 0; i < currentPattern.size(); ++i) {
            float diff = std::abs(currentPattern[i] - previousPattern[i]);
            patternNovelty += diff;
        }
        patternNovelty /= currentPattern.size();
    }
    
    // Weighted combination of novelty features
    pImpl->level = std::min(1.0f, 
                         0.4f * intensityNovelty + 
                         0.4f * varianceNovelty + 
                         0.2f * patternNovelty);
    
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}


void Novelty::update(TimestepDuration dt) {
    // Decay novelty
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
