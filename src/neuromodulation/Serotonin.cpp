#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float impulsivity;
    float socialWeight;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.04f),
             releaseRate(1.0f), impulsivity(0.5f), socialWeight(0.5f) {}
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
    // Phase 2: Real serotonin-modulated plasticity
    // Serotonin regulates mood, impulsivity, and social behavior
    // Low serotonin -> high impulsivity -> more exploration/reinforcement
    float modulation = 1.0f - 0.5f * pImpl->level;  // Range: 0.5 - 1.0
    // Apply impulsivity factor
    modulation *= (1.0f + pImpl->impulsivity * 0.3f);
    modulation = std::clamp(modulation, 0.3f, 2.0f);
    return modulation;
}

void Serotonin::update(TimestepDuration dt) {
    // Real serotonin dynamics: moderate decay
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Serotonin::signalMood(float rewardHistory, float socialContext) {
    // Real mood signaling based on reward history and social context
    float rewardFactor = std::clamp(rewardHistory + 1.0f, 0.0f, 2.0f) * 0.3f;
    float socialFactor = std::clamp(socialContext + 1.0f, 0.0f, 2.0f) * 0.2f;
    
    float burst = rewardFactor + socialFactor;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Update derived traits
    pImpl->impulsivity = 0.5f + 0.3f * rewardHistory;
    pImpl->socialWeight = 0.5f + 0.3f * socialContext;
}

void Serotonin::regulateSocialBehavior(float groupCohesion, float dominance) {
    // Regulate social behavior based on group dynamics
    float cohesionFactor = std::clamp(groupCohesion, 0.0f, 1.0f);
    float dominanceFactor = std::clamp(dominance, 0.0f, 1.0f);
    
    // High cohesion + high dominance -> higher serotonin (social bonding)
    float socialDrive = cohesionFactor * (1.0f - dominanceFactor) * 0.5f;
    setLevel(getLevel() + socialDrive * 0.1f);
}

} // namespace nlm
