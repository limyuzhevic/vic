#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.1f), releaseRate(1.0f) {}
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
    // Serotonin modulates mood, impulsivity, and social behavior
    // Higher serotonin decreases impulsivity and aggression
    return 0.5f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::reduceImpulsivity(float strength) {
    // Decrease impulsive actions and improve self-control
    if (brain_ && strength > 0.0f) {
        // In a full implementation, this would increase inhibitory control
        // and decrease reward-seeking behavior
    }
}

void Serotonin::promoteSocialBehavior(float strength) {
    // Enhance social behavior and reduce aggression
    if (brain_ && strength > 0.0f) {
        // In a full implementation, this would increase social approach behavior
        // and decrease avoidance responses
    }
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

} // namespace nlm