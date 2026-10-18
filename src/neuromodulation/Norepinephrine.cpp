#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float maxLevel;
    float decayRate;
    float recoveryRate;
    bool isSleep;
    float arousalLevel;
    std::vector<float> alertnessHistory;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , maxLevel(1.0f)
        , decayRate(0.2f)
        , recoveryRate(0.15f)
        , isSleep(false)
        , arousalLevel(0.5f)
        , alertnessHistory() {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine (arousal) system initialized");
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    if (pImpl->isSleep) {
        pImpl->level = pImpl->baseline;
        return;
    }
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->maxLevel);
    pImpl->arousalLevel = pImpl->level;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity during high arousal (learning) states
    if (pImpl->isSleep) return 0.1f;
    return 1.0f + pImpl->level * 0.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    if (pImpl->isSleep) {
        // Recover during sleep
        pImpl->level = std::min(pImpl->maxLevel, pImpl->level + pImpl->recoveryRate * static_cast<float>(dt));
        pImpl->isSleep = false;
        return;
    }
    
    // Natural decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousalLevel = pImpl->level;
    pImpl->alertnessHistory.push_back(pImpl->arousalLevel);
}

void Norepinephrine::increaseArousal(float amount) {
    if (!pImpl->isSleep) {
        setLevel(getLevel() + amount);
    }
}

void Norepinephrine::decreaseArousal(float amount) {
    setLevel(getLevel() - amount);
}

void Norepinephrine::setSleepState(bool sleeping) {
    if (pImpl->isSleep != sleeping) {
        pImpl->isSleep = sleeping;
        if (sleeping) {
            NLM_LOG_INFO("Norepinephrine system entering sleep state");
        }
    }
}

bool Norepinephrine::isAsleep() const {
    return pImpl->isSleep;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->isSleep = false;
    pImpl->arousalLevel = pImpl->baseline;
    pImpl->alertnessHistory.clear();
}

void Norepinephrine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Norepinephrine::getBaseline() const {
    return pImpl->baseline;
}

void Norepinephrine::setReactivity(float reactivity) {
    pImpl->decayRate = reactivity;
}

const std::vector<float>& Norepinephrine::getAlertnessHistory() const {
    return pImpl->alertnessHistory;
}

} // namespace nlm