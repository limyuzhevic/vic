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
    float releaseRate;
    float learningRateModulation;
    float impulsivityFactor;
    float moodState;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(0.3f),
             learningRateModulation(1.0f), impulsivityFactor(1.0f), moodState(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
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
    // 5-HT modulates learning rate and behavioral flexibility
    return 0.3f + 0.7f * pImpl->learningRateModulation;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodStrength) {
    // Mood signals boost 5-HT level
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodStrength * pImpl->releaseRate);
}

void Serotonin::signalSocialBehavior(float socialStrength) {
    // Social behavior enhances 5-HT level
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialStrength * pImpl->releaseRate * 0.6f);
    pImpl->impulsivityFactor = 0.8f;  // Reduce impulsivity
}

float Serotonin::getLearningRateModulation() const {
    return pImpl->learningRateModulation;
}

float Serotonin::getImpulsivityFactor() const {
    return pImpl->impulsivityFactor;
}

} // namespace nlm