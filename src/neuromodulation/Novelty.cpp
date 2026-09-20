// Novelty implementation
#include "Novelty.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    float level;
    float baseline;
    std::vector<float> history;
    
    Impl() : level(0.0f), baseline(0.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(class Brain* brain) {
    // Store brain reference if needed
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::detectNovelty(const Observation& observation, 
                           const Observation& previousObservation) {
    // Compute novelty as difference from previous observation
    float diff = std::abs(observation.rewards[0] - previousObservation.rewards[0]);
    setLevel(diff);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    // Compute pattern difference
    if (currentPattern.size() != previousPattern.size()) return;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < currentPattern.size(); ++i) {
        totalDiff += std::abs(currentPattern[i] - previousPattern[i]);
    }
    
    float avgDiff = totalDiff / currentPattern.size();
    setLevel(avgDiff);
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time
    pImpl->level = std::max(pImpl->baseline, pImpl->level * 0.95f);
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
