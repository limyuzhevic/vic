#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalLevel;
    float vigilanceLevel;
    float stressLevel;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousalLevel(1.0f)
        , vigilanceLevel(1.0f)
        , stressLevel(0.0f)
        , decayRate(0.05f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = level;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE increases plasticity during arousal and stress
    return std::min(2.0f, 1.0f + pImpl->level * (pImpl->stressLevel > 0.5f ? 0.5f : 0.3f));
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay norepinephrine level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseVigilance() {
    // Increase vigilance and arousal
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + 0.1f);
    pImpl->level = std::min(1.0f, pImpl->level + 0.1f);
}

void Norepinephrine::stressResponse(float stressorLevel) {
    // Response to stress/stimulus
    pImpl->stressLevel = std::min(1.0f, stressorLevel);
    pImpl->level = std::min(1.0f, pImpl->level + pImpl->stressLevel * 0.5f);
}

} // namespace nlm