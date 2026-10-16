// Implementation of Serotonin neuromodulator
#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

Serotonin::Serotonin() : pImpl(new Impl) {
    // 5-HT effects on mood, impulsivity, and social behavior
    pImpl->moodMod = 1.1f;          // Improves mood
    pImpl->impulsivityMod = 0.8f;   // Reduces impulsivity
    pImpl->socialMod = 1.0f;       // Neutral social effect
    pImpl->plasticityMod = 1.2f;   // Enhances plasticity
}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin neuromodulator initialized");
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    NLM_LOG_DEBUG(std::string("5-HT level set to: ") + std::to_string(pImpl->level));
}

float Serotonin::getMoodEffect() const {
    return pImpl->moodMod;
}

float Serotonin::getImpulsivityEffect() const {
    return pImpl->impulsivityMod;
}

float Serotonin::getSocialEffect() const {
    return pImpl->socialMod;
}

float Serotonin::getPlasticityEffect() const {
    return pImpl->plasticityMod;
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics: increases with social feedback, decays naturally
    if (pImpl->brain) {
        // Modulate based on social feedback
        float socialFeedback = pImpl->brain->getSocialFeedback();
        pImpl->level += socialFeedback * 0.05f - pImpl->decayRate * static_cast<float>(dt);
    }
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    if (pImpl->level > 0.5f) {
        NLM_LOG_DEBUG(std::string("5-HT high: ") + std::to_string(pImpl->level));
    }
}

} // namespace nlm
