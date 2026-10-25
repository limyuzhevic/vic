#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalLevel;
    float decayRate;
    float enhancementRate;
    float baselineArousal;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousalLevel(0.5f)
        , decayRate(0.03f)
        , enhancementRate(1.5f)
        , baselineArousal(0.5f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 3.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine enhances focus and memory encoding
    // Higher NE increases signal-to-noise ratio
    return 0.7f + 0.6f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal level based on NE concentration
    pImpl->arousalLevel = pImpl->baselineArousal + pImpl->level * 0.33f;
}

void Norepinephrine::increaseArousal(float arousalBoost) {
    pImpl->arousalLevel += arousalBoost * pImpl->enhancementRate;
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel);
    pImpl->level = std::min(3.0f, pImpl->level + arousalBoost);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

} // namespace nlm
