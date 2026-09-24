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

Novelty::Novelty() : pImpl(new Impl) {
    NLM_LOG_INFO("Novelty: constructor initialized");
}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    if (level < 0.0f) {
        NLM_LOG_WARNING("Novelty::setLevel: invalid level {:.6f} (must be >= 0.0), clamping to 0.0", level);
        pImpl->level = 0.0f;
    } else if (level > 1.0f) {
        NLM_LOG_WARNING("Novelty::setLevel: invalid level {:.6f} (must be <= 1.0), clamping to 1.0", level);
        pImpl->level = 1.0f;
    } else {
        pImpl->level = level;
    }
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Extract feature vectors from observations
    const std::vector<float>& currentData = observation.getData();
    const std::vector<float>& previousData = previousObservation.getData();
    
    NLM_LOG_INFO("Novelty::detectNovelty: comparing observations with {} vs {} dimensions",
                 currentData.size(), previousData.size());
    
    // Use the vector-based implementation for novelty detection
    detectNovelty(currentData, previousData);
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
    if (dt <= 0) {
        NLM_LOG_WARNING("Novelty::update: invalid timestep duration {:.6f} (must be > 0)", dt);
        return;
    }
    
    // Decay novelty
    float oldLevel = pImpl->level;
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    if (std::abs(pImpl->level - oldLevel) > 1e-6f) {
        NLM_LOG_INFO("Novelty::update: novelty decayed from {:.6f} to {:.6f}", oldLevel, pImpl->level);
    }
    
    // Limit history size to prevent unbounded memory growth
    const size_t MAX_HISTORY_SIZE = 1000;
    if (pImpl->history.size() >= MAX_HISTORY_SIZE) {
        NLM_LOG_DEBUG("Novelty::update: history size limit reached ({}), clearing oldest entries", MAX_HISTORY_SIZE);
        pImpl->history.erase(pImpl->history.begin());
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    size_t oldSize = pImpl->history.size();
    pImpl->history.clear();
    
    if (oldSize > 0) {
        NLM_LOG_INFO("Novelty::clearHistory: cleared {} history entries", oldSize);
    }
}

} // namespace nlm
