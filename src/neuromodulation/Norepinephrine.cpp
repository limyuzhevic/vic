#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalWeight;
    float vigilanceWeight;
    float stressWeight;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousalWeight(0.4f)
        , vigilanceWeight(0.4f)
        , stressWeight(0.2f)
        , decayRate(0.04f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine (NE) system initialized");
}

const char* Norepinephrine::getName() const {
    return "Norepinephrine";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = level;
}

void Norepinephrine::update(float arousal, float vigilance, float stress, TimestepDuration dt) {
    // NE increases with arousal, vigilance, and stress
    float target = pImpl->arousalWeight * arousal + 
                   pImpl->vigilanceWeight * vigilance +
                   pImpl->stressWeight * stress;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal and stress
    float modulation = 1.0f;
    if (pImpl->level > 0.5f) {
        modulation += pImpl->level * 0.3f;  // High arousal enhances plasticity
    }
    if (pImpl->level > 0.8f) {
        modulation += pImpl->level * 0.2f;  // Stress impairs plasticity
    }
    return modulation;
}

void Norepinephrine::setArousalWeight(float weight) {
    pImpl->arousalWeight = weight;
}

void Norepinephrine::setVigilanceWeight(float weight) {
    pImpl->vigilanceWeight = weight;
}

void Norepinephrine::setStressWeight(float weight) {
    pImpl->stressWeight = weight;
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
