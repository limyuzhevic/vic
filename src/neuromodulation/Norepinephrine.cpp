#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousal;
    float vigilance;
    float focus;
    float decayRate;
    float recoveryRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousal(0.5f)
        , vigilance(0.3f)
        , focus(0.4f)
        , decayRate(0.08f)
        , recoveryRate(0.05f) {}
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
    // Norepinephrine affects arousal and vigilance
    // Higher NE increases alertness and focus
    float arousalMod = 1.0f + pImpl->arousal * pImpl->level;
    float vigilanceMod = 1.0f + pImpl->vigilance * pImpl->level;
    float focusMod = 1.0f + pImpl->focus * pImpl->level;
    return arousalMod * vigilanceMod * focusMod;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay with slow recovery
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->level += pImpl->recoveryRate * static_cast<float>(dt);
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

void Norepinephrine::setArousal(float arousal) {
    pImpl->arousal = arousal;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

void Norepinephrine::setVigilance(float vigilance) {
    pImpl->vigilance = vigilance;
}

float Norepinephrine::getFocus() const {
    return pImpl->focus;
}

void Norepinephrine::setFocus(float focus) {
    pImpl->focus = focus;
}

} // namespace nlm
