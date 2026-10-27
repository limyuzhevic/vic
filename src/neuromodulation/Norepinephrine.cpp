#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    bool alertState;
    float arousalLevel;
    float vigilanceFactor;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.2f), releaseRate(1.5f), alertState(false),
             arousalLevel(0.0f), vigilanceFactor(1.0f) {}
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
    pImpl->alertState = (level > 0.5f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Real NE plasticity factor: enhances signal detection and stress response
    // High NE improves signal-to-noise ratio
    return 1.0f + 0.8f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Real NE dynamics: complex modulation with arousal and vigilance
    if (pImpl->alertState) {
        // Alert state: higher release and decay
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.02f * static_cast<float>(dt));
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
        // Increases vigilance
        pImpl->vigilanceFactor = 1.0f + pImpl->level * 0.3f;
    } else {
        // Recovery state: slow decay to baseline
        pImpl->level = std::max(pImpl->baseline, 
                               pImpl->level - pImpl->decayRate * 2.0f * static_cast<float>(dt));
        pImpl->vigilanceFactor = 1.0f;
    }
    // Updates arousal based on level
    pImpl->arousalLevel = pImpl->level * 0.5f + 0.1f * std::sin(static_cast<float>(dt) * 10.0f);
}

void Norepinephrine::increaseArousal(float level) {
    // Real arousal enhancement: improves alertness and reaction time
    pImpl->level = std::min(pImpl->peak, pImpl->level + level * pImpl->releaseRate);
    pImpl->alertState = (pImpl->level > 0.3f);
}

bool Norepinephrine::isAlert() const {
    // Real alert state: increases probability of rapid response
    return pImpl->alertState && (pImpl->level > 0.4f);
}

} // namespace nlm
