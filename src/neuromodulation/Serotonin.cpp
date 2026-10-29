#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodBoost;
    float satietyFactor;
    float socialBehaviorMod;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.01f)
        , peak(1.0f)
        , decayRate(0.2f)
        , moodBoost(1.3f)
        , satietyFactor(0.8f)
        , socialBehaviorMod(1.2f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getMoodFactor() const {
    return 1.0f + pImpl->level * (pImpl->moodBoost - 1.0f);
}

float Serotonin::getSatietyFactor() const {
    return pImpl->level * pImpl->satietyFactor;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity for stable memory formation
    return 1.0f - 0.1f * pImpl->level;  // High serotonin reduces plasticity for stability
}

void Serotonin::signalReward(float reward) {
    // TODO PHASE 2: Implement real serotonin signaling
    // PLACEHOLDER: Serotonin responds to reward prediction errors
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * 0.1f);
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Slow serotonin dynamics
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::improveMood() {
    if (pImpl->brain) {
        // Mild mood elevation affects behavioral output
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f);
    }
}

void Serotonin::reduceImpulsivity() {
    if (pImpl->brain) {
        // Higher serotonin reduces impulsive actions
        if (pImpl->brain->getAttention()) {
            auto* attention = pImpl->brain->getAttention();
            attention->reduceImpulsivity(0.2f);
        }
    }
}

} // namespace nlm