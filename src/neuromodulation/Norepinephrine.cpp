#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float arousalLevel;
    float vigilanceBoost;
    float stressResponse;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , arousalLevel(0.5f)
        , vigilanceBoost(1.2f)
        , stressResponse(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized (arousal and vigilance)");
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    float decayRate = 0.03f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal(float strength) {
    // Implementation would increase neural arousal
    // For now, just log the effect
    NLM_LOG_INFO("Norepinephrine increasing arousal by " + std::to_string(strength));
}

void Norepinephrine::enhanceVigilance(float duration) {
    // Implementation would boost vigilance and alertness
    // For now, just log the effect
    NLM_LOG_INFO("Norepinephrine enhancing vigilance for " + std::to_string(duration) + "s");
}

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
    // Norepinephrine modulates plasticity based on arousal
    return 1.0f + pImpl->level * 0.3f;
}

} // namespace nlm
