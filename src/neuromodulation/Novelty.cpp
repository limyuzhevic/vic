#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    std::vector<float> history;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , decayRate(0.1f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection system initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::detectNovelty(const Observation& observation,
                           const Observation& previousObservation) {
    // TODO PHASE 2: Implement real novelty detection using observation data
    // PLACEHOLDER: Simple threshold-based novelty
    float diff = 0.0f; // Placeholder calculation
    setLevel(std::min(1.0f, pImpl->level + diff * 0.1f));
    pImpl->history.push_back(pImpl->level);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    // TODO PHASE 2: Implement real novelty detection from patterns
    // PLACEHOLDER: Calculate mean squared difference
    float sumDiff = 0.0f;
    size_t count = std::min(currentPattern.size(), previousPattern.size());
    for (size_t i = 0; i < count; ++i) {
        float diff = currentPattern[i] - previousPattern[i];
        sumDiff += diff * diff;
    }
    
    float mse = count > 0 ? sumDiff / count : 0.0f;
    float novelty = std::sqrt(mse);
    
    setLevel(std::min(1.0f, pImpl->level + novelty * 0.1f));
    pImpl->history.push_back(pImpl->level);
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
