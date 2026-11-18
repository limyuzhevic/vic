#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    float releaseRate;
    float impactOnMood;
    float impactOnImpulsivity;
    float impactOnMemory;
    float impactOnSocialBehavior;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , impactOnMood(0.5f)
        , impactOnImpulsivity(0.3f)
        , impactOnMemory(0.4f)
        , impactOnSocialBehavior(0.6f) {}
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
    // Serotonin modulates mood and impulsivity effects on plasticity
    return 0.4f + 0.6f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMoodChange(float strength) {
    // 5-HT signals mood changes and affects impulsivity
    pImpl->level = std::min(1.0f, pImpl->level + strength * pImpl->releaseRate);
}

void Serotonin::signalImpulsivityChange(float strength) {
    // 5-HT reduces impulsivity and promotes patience
    pImpl->level = std::min(1.0f, pImpl->level + strength * pImpl->releaseRate * 0.8f);
}

void Serotonin::signalMemoryModulation(float strength) {
    // 5-HT modulates memory consolidation and retrieval
    pImpl->level = std::min(1.0f, pImpl->level + strength * pImpl->releaseRate * 0.5f);
}

void Serotonin::setMoodImpact(float impact) {
    pImpl->impactOnMood = impact;
}

void Serotonin::setImpulsivityImpact(float impact) {
    pImpl->impactOnImpulsivity = impact;
}

void Serotonin::setMemoryImpact(float impact) {
    pImpl->impactOnMemory = impact;
}

void Serotonin::setSocialBehaviorImpact(float impact) {
    pImpl->impactOnSocialBehavior = impact;
}

float Serotonin::getMoodImpact() const {
    return pImpl->impactOnMood;
}

float Serotonin::getImpulsivityImpact() const {
    return pImpl->impactOnImpulsivity;
}

float Serotonin::getMemoryImpact() const {
    return pImpl->impactOnMemory;
}

float Serotonin::getSocialBehaviorImpact() const {
    return pImpl->impactOnSocialBehavior;
}

} // namespace nlm
