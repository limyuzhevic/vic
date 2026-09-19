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
    static constexpr size_t MAX_HISTORY_SIZE = 1000;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Invalid brain pointer for Novelty initialization");
        return;
    }
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
    if (!pImpl->brain) {
        NLM_LOG_ERROR("Brain not initialized for Novelty detection");
        return;
    }
    
    // Extract features from observations using brain's feature extraction
    // This would be implemented with the actual brain functionality
    // For now, use a basic placeholder but with proper validation
    pImpl->level = 0.0f;
    
    // Store observation pattern for future comparison
    // This is a simplified version - in production would extract proper features
    
    // Add level to history with capacity check
    if (pImpl->history.size() < pImpl->MAX_HISTORY_SIZE) {
        pImpl->history.push_back(pImpl->level);
    }
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (!pImpl->brain) {
        NLM_LOG_ERROR("Brain not initialized for Novelty detection");
        return;
    }
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
    
    // Add level to history with capacity check
    if (pImpl->history.size() < pImpl->MAX_HISTORY_SIZE) {
        pImpl->history.push_back(pImpl->level);
    }
    
    // Store current pattern using copy, not reference
    pImpl->lastPattern.assign(currentPattern.begin(), currentPattern.end());
    if (pImpl->lastPattern.size() > 1000) {
        pImpl->lastPattern.resize(1000);
    }
}

void Novelty::update(TimestepDuration dt) {
    if (!pImpl->brain) {
        return;
    }
    
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
