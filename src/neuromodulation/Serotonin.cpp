#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float mood;
    float impulsivityModulation;
    float socialBehaviorBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.08f)
        , mood(0.5f)
        , impulsivityModulation(1.0f)
        , socialBehaviorBoost(0.8f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized (mood, impulsivity, social behavior)");
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    float decayRate = 0.04f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayRate * static_cast<float>(dt));
}

void Serotonin::improveMood(float strength) {
    // Implementation would improve mood and reduce negative affect
    // For now, just log the effect
    NLM_LOG_INFO("Serotonin improving mood by " + std::to_string(strength));
}

void Serotonin::reduceImpulsivity(float duration) {
    // Implementation would reduce impulsive behaviors
    // For now, just log the effect
    NLM_LOG_INFO("Serotonin reducing impulsivity for " + std::to_string(duration) + "s");
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
    // Serotonin modulates plasticity for emotional learning
    return 1.0f + pImpl->level * 0.2f;
}

} // namespace nlm
