#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousal;
    float vigilance;
    float decayRate;
    std::vector<float> history;
    
    Impl() : brain(nullptr), level(0.0f), arousal(0.5f), vigilance(0.5f), decayRate(0.1f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances arousal and vigilance, increasing plasticity for novel stimuli
    float factor = 1.0f + pImpl->arousal * pImpl->level * 0.3f;
    factor += pImpl->vigilance * pImpl->level * 0.2f;
    return factor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay norepinephrine
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->history.push_back(pImpl->level);
}

void Norepinephrine::signalArousal(float arousalStrength) {
    // Increase arousal for alert behavior
    pImpl->level = std::min(1.0f, pImpl->level + arousalStrength * 0.3f);
}

void Norepinephrine::setVigilance(float vigilanceLevel) {
    pImpl->vigilance = vigilanceLevel;
}

void Norepinephrine::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
