// Implementation of Norepinephrine neuromodulator
#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    // NE effects on arousal and vigilance
    pImpl->arousalMod = 1.3f;      // Increases arousal
    pImpl->vigilanceMod = 1.2f;    // Increases vigilance
    pImpl->plasticityMod = 1.1f;  // Modulates plasticity
    pImpl->attentionMod = 0.8f;   // Slightly decreases focused attention
}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine neuromodulator initialized");
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    NLM_LOG_DEBUG(std::string("NE level set to: ") + std::to_string(pImpl->level));
}

float Norepinephrine::getArousalEffect() const {
    return pImpl->arousalMod;
}

float Norepinephrine::getVigilanceEffect() const {
    return pImpl->vigilanceMod;
}

float Norepinephrine::getPlasticityEffect() const {
    return pImpl->plasticityMod;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: increases with stress/arousal demands, decays naturally
    if (pImpl->brain) {
        // Modulate based on arousal demands
        float arousalDemand = pImpl->brain->getArousalDemand();
        pImpl->level += arousalDemand * 0.08f - pImpl->decayRate * static_cast<float>(dt);
    }
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    if (pImpl->level > 0.5f) {
        NLM_LOG_DEBUG(std::string("NE high: ") + std::to_string(pImpl->level));
    }
}

} // namespace nlm
