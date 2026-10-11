#include "BasePimpl.hpp"
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
    pImpl->get()->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized");
}

float Novelty::getLevel() const {
    return pImpl->get()->level;
}

void Novelty::setLevel(float level) {
    pImpl->get()->level = level;
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Extract features from observations and compare
    // Simple implementation: just set to a placeholder
    pImpl->get()->level = 1.0f;
    pImpl->get()->history.push_back(pImpl->get()->level);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->get()->level = 0.0f;
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
    pImpl->get()->level = std::min(1.0f, avgDiff / pImpl->get()->noveltyThreshold);
    pImpl->get()->history.push_back(pImpl->get()->level);
    
    // Store current pattern for next comparison
    pImpl->get()->lastPattern = currentPattern;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty
    pImpl->get()->level = std::max(0.0f, pImpl->get()->level - pImpl->get()->decayRate * static_cast<float>(dt));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->get()->history;
}

void Novelty::clearHistory() {
    pImpl->get()->history.clear();
}

} // namespace nlm
