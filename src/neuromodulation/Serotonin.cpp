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
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(1.0f) {}
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
    // Serotonin modulates mood and impulsivity
    // Higher 5-HT reduces plasticity for impulsive actions
    return 0.8f - 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

void Serotonin::signalMood(float mood) {
    // Boost mood
    setLevel(std::min(pImpl->peak, getLevel() + mood * pImpl->releaseRate));
}

void Serotonin::reduceImpulsivity() {
    // Serotonin reduces impulsive behavior
    if (pImpl->brain) {
        // Reduce high-frequency bursting
        auto* spikeSys = pImpl->brain->getSpikeSystem();
        if (spikeSys) {
            spikeSys->reduceBursting(0.1f * getLevel());
        }
    }
}

void Serotonin::enhanceSocialBehavior() {
    // Serotonin enhances social tendencies and cooperation
    if (pImpl->brain) {
        auto* workingMem = pImpl->brain->getWorkingMemory();
        if (workingMem) {
            workingMem->enhanceSocialCohesion();
        }
    }
}

} // namespace nlm
