#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Novelty::Impl {
    float level;
    float decayRate;
    float baseline;
    float explorationModulator;
    
    Impl() : level(0.0f), decayRate(0.05f), baseline(0.0f), explorationModulator(1.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

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
    // Novelty enhances plasticity for new learning
    return 1.0f + pImpl->level * 0.5f;
}

void Novelty::update(TimestepDuration dt) {
    // Decay with small baseline level
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Novelty::computeNovelty(const std::vector<float>& currentVision, 
                           const std::vector<float>& previousVision) {
    // Compute novelty as average difference from previous experience
    if (currentVision.empty() || previousVision.empty() || 
        currentVision.size() != previousVision.size()) {
        return;
    }
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < currentVision.size(); ++i) {
        totalDiff += std::abs(currentVision[i] - previousVision[i]);
    }
    
    pImpl->level = totalDiff / currentVision.size();
    
    // Enhance exploration drive
    pImpl->explorationModulator = 1.0f + pImpl->level * 0.8f;
}

float Novelty::getExplorationModulator() const {
    return pImpl->explorationModulator;
}

} // namespace nlm