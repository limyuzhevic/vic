#include "Novelty.hpp"

namespace nlm {

struct Novelty::Impl {
    float level;
    float decayRate;
    std::vector<float> history;
    
    Impl() : level(0.0f), decayRate(0.1f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // TODO PHASE 2: Implement real novelty detection
    // PLACEHOLDER: Simple difference detection
    pImpl->level = 1.0f;  // Placeholder
    pImpl->history.push_back(pImpl->level);
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
