#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float vigilance;
    float locusCoeruleusActivity;
    
    Impl() : level(0.0f), arousalLevel(0.0f), vigilance(0.0f), locusCoeruleusActivity(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal
    // High arousal (NE) enhances plasticity for salient events
    return 1.0f + pImpl->level * 0.3f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Update NE based on environmental challenges and uncertainty
    // Simplified: level increases with time and reset with success
    pImpl->level = 0.1f;  // Baseline level
    
    // NE affects alertness and response preparation
    pImpl->arousalLevel = pImpl->level * 0.7f;
    pImpl->vigilance = pImpl->level * 0.8f;
    pImpl->locusCoeruleusActivity = pImpl->level * 0.9f;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

void Norepinephrine::signalChallenge(float magnitude) {
    // NE increases with environmental challenges and novelty
    setLevel(getLevel() + magnitude * 0.2f);
}

void Norepinephrine::signalReward(float reward) {
    // NE decreases after reward (reset of arousal system)
    setLevel(getLevel() * 0.9f);
}

} // namespace nlm