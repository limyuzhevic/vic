#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodWeight;
    float socialModulation;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.08f)
        , moodWeight(1.2f)
        , socialModulation(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized - mood, impulsivity, and social behavior");
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects mood and impulse control
    // Moderate levels optimize behavior
    return 1.0f + 0.2f * pImpl->level - 0.1f * std::pow(pImpl->level, 2);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply mood effects on behavior
    if (pImpl->brain && pImpl->level > 0.5f) {
        // Enhanced mood can affect action selection
        auto* dopamine = pImpl->brain->getDopamine();
        if (dopamine) {
            // Serotonin can modulate dopamine activity
            float currentDA = dopamine->getLevel();
            // Apply mood-dependent modulation
            dopamine->setLevel(std::min(1.0f, currentDA + 0.1f * pImpl->level));
        }
    }
}

void Serotonin::signalMood(float moodStrength) {
    // Bottom-up mood signal
    setLevel(std::min(pImpl->peak, pImpl->level + moodStrength * 0.3f));
}

void Serotonin::enhanceSocialBehavior(float socialLevel) {
    // Top-down social modulation
    setLevel(std::min(pImpl->peak, pImpl->level + socialLevel * 0.2f));
}

float Serotonin::getMoodLevel() const {
    return pImpl->level;
}

float Serotonin::getSocialModulation() const {
    return pImpl->socialModulation;
}

void Serotonin::setMoodWeight(float weight) {
    pImpl->moodWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Serotonin::setSocialModulation(float modulation) {
    pImpl->socialModulation = std::clamp(modulation, 0.0f, 1.0f);
}

} // namespace nlm
