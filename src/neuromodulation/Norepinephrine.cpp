#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalWeight;
    float vigilanceWeight;
    float baselineLevel;
    float decayRate;
    float stressLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousalWeight(0.5f)
        , vigilanceWeight(0.3f)
        , baselineLevel(0.15f)
        , decayRate(0.03f)
        , stressLevel(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE level depends on arousal needs and vigilance requirements
    // In a full implementation, this would integrate with stress and arousal systems
    // For now, use baseline level with decay
    pImpl->level = pImpl->baselineLevel;
    
    // Modify based on stress level
    pImpl->level *= (1.0f + pImpl->stressLevel * 0.5f);
    
    // Decay toward baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Norepinephrine::getArousalFactor() const {
    // Increased arousal when NE is high
    return std::pow(1.0f + pImpl->level * pImpl->arousalWeight, 1.8f);
}

float Norepinephrine::getVigilanceFactor() const {
    // Enhanced vigilance when NE is high (attention sharpening)
    return std::pow(1.0f + pImpl->level * pImpl->vigilanceWeight, 2.0f);
}

float Norepinephrine::getStressAdaptationFactor() const {
    // Stress adaptation factor
    return std::pow(1.0f + pImpl->stressLevel, 1.5f);
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->stressLevel = stressLevel;
}

void Norepinephrine::setArousalWeight(float weight) {
    pImpl->arousalWeight = weight;
}

void Norepinephrine::setVigilanceWeight(float weight) {
    pImpl->vigilanceWeight = weight;
}

void Norepinephrine::setBaselineLevel(float level) {
    pImpl->baselineLevel = level;
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
    pImpl->stressLevel = 0.0f;
}

} // namespace nlm