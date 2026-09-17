#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

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
    // Compare current and previous observations
    float totalDiff = 0.0f;
    
    // Vision novelty
    size_t minVision = std::min(observation.vision.size(), previousObservation.vision.size());
    for (size_t i = 0; i < minVision; ++i) {
        totalDiff += std::abs(observation.vision[i] - previousObservation.vision[i]);
    }
    
    // Touch novelty
    minVision = std::min(observation.touch.size(), previousObservation.touch.size());
    for (size_t i = 0; i < minVision; ++i) {
        totalDiff += std::abs(observation.touch[i] - previousObservation.touch[i]);
    }
    
    // Internal novelty
    minVision = std::min(observation.internal.size(), previousObservation.internal.size());
    for (size_t i = 0; i < minVision; ++i) {
        totalDiff += std::abs(observation.internal[i] - previousObservation.internal[i]);
    }
    
    // Proprioception novelty
    minVision = std::min(observation.proprioception.size(), previousObservation.proprioception.size());
    for (size_t i = 0; i < minVision; ++i) {
        totalDiff += std::abs(observation.proprioception[i] - previousObservation.proprioception[i]);
    }
    
    // Normalize by total features
    size_t totalFeatures = observation.getTotalFeatureCount();
    pImpl->level = (totalFeatures > 0) ? std::min(1.0f, totalDiff / static_cast<float>(totalFeatures) / pImpl->noveltyThreshold) : 0.0f;
    
    pImpl->history.push_back(pImpl->level);
    pImpl->lastPattern = observation.vision; // Use vision as main pattern
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute difference between patterns
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDiff += diff;
    }
    
    // Normalize by pattern size
    float avgDiff = totalDiff / compareLen;
    
    // Update novelty level based on difference
    pImpl->level = std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
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
