#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float impulsivity;
    float socialBehavior;
    float baseline;
    float peak;
    float anxietyLevel;
    float socialCohesion;
    
    Impl() : brain(nullptr), level(0.0f), mood(0.0f), impulsivity(0.0f), socialBehavior(0.0f),
             baseline(0.0f), peak(2.0f), anxietyLevel(0.0f), socialCohesion(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

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
    // Serotonin modulates mood and social behavior
    // Complex effects: optimal levels improve learning, too much causes anxiety
    // Tonic serotonin enhances signal-to-noise ratio in cortical circuits
    float moodMod = 1.0f - std::abs(pImpl->mood - 0.5f) * 2.0f;  // Inverted U-curve
    return 0.6f + pImpl->level * moodMod;  // 0.6 to 2.0x modulation
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin decays with time constant; reset during stress
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.03f * static_cast<float>(dt));
    
    // Update derived mood and social states
    pImpl->mood = pImpl->level * 0.8f;
    pImpl->impulsivity = 1.0f - pImpl->mood * 0.5f;  // Low mood = higher impulsivity
    pImpl->socialCohesion = pImpl->mood * 0.9f;      // High mood = better social behavior
}

void Serotonin::regulateMood(float anxiety, float socialContext) {
    // Hippocampal serotonin regulates anxiety and social behavior
    // Rats with depleted serotonin become socially isolated and anxious
    float moodSignal = anxiety * 0.7f + socialContext * 0.3f;
    
    // Update level based on mood signal
    if (moodSignal > pImpl->level) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + moodSignal * 0.4f);
    }
    pImpl->anxietyLevel = anxiety;
}

void Serotonin::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized - mood and social behavior modulation ready");
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialCohesion;
}

} // namespace nlm