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
    float sensitivity;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , decayRate(0.1f)
        , noveltyThreshold(0.3f)
        , sensitivity(1.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(class Brain* brain) {
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Novelty::getPlasticityFactor() const {
    // Novelty enhances exploration and learning
    float effectiveLevel = std::min(pImpl->level / pImpl->noveltyThreshold, 1.0f);
    
    // Novelty increases plasticity for novel experiences
    float plasticityFactor = 1.0f + 0.8f * effectiveLevel;
    
    // Higher novelty leads to more exploratory learning
    plasticityFactor = std::min(plasticityFactor, 2.5f);
    
    return plasticityFactor;
}

void Novelty::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay novelty over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * dtFloat);
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Extract features from observations and compute novelty
    // For now, use a simple placeholder implementation
    pImpl->level = 1.0f;  // High novelty by default
    pImpl->history.push_back(pImpl->level);
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
    
    // Normalize by pattern size and sensitivity
    float avgDiff = totalDiff / compareLen;
    pImpl->level = std::min(1.0f, avgDiff * pImpl->sensitivity / pImpl->noveltyThreshold);
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

float Novelty::getDecayRate() const {
    return pImpl->decayRate;
}

float Novelty::getNoveltyThreshold() const {
    return pImpl->noveltyThreshold;
}

void Novelty::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.0f, rate);
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::max(0.1f, threshold);
}

void Novelty::reset() {
    pImpl->level = 0.0f;
    pImpl->history.clear();
}

} // namespace nlm