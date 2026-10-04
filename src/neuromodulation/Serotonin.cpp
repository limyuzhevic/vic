#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float moodModifier;
    float socialValue;
    float resetRate;
    float impulseControl;
    
    Impl() : level(0.5f), baseline(0.3f), moodModifier(1.0f), 
             socialValue(0.0f), resetRate(0.1f), impulseControl(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl), socialLearningEnabled_(true), resetRate_(0.2f) {}

Serotonin::~Serotonin() = default;

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
    // Serotonin modulates plasticity for mood and social behavior
    float moodMod = pImpl->moodModifier * pImpl->level;
    return 1.0f + moodMod * 0.1f;
}

void Serotonin::update(TimestepDuration dt) {
    // Gradual return to baseline
    float reset = pImpl->resetRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - reset);
}

float Serotonin::getMoodModifier() const {
    // Serotonin improves mood and reward sensitivity
    return 0.7f + pImpl->level * 0.6f;
}

float Serotonin::getSocialFactor() const {
    // Serotonin enhances social bonding and cooperation
    return 0.4f + pImpl->socialValue * 0.8f;
}

float Serotonin::getImpulseControl() const {
    // Higher serotonin improves impulse control
    return 0.3f + pImpl->level * 0.8f;
}

void Serotonin::signalSocialInteraction(float socialValue) {
    // Strong serotonin response to social interaction
    pImpl->socialValue = socialValue;
    pImpl->level = std::min(1.0f, pImpl->level + 0.2f * socialValue);
    if (socialLearningEnabled_) {
        NLM_LOG_INFO("Serotonin social learning: social_value=" + std::to_string(socialValue) + ", level=" + std::to_string(pImpl->level));
    }
}

} // namespace nlm
