#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    float vigilance;
    float stress;
    float sensoryGain;
    float wmCapacity;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), arousal(0.5f), 
             vigilance(0.5f), stress(0.0f), sensoryGain(1.0f), 
             wmCapacity(1.0f) {}
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
    // Norepinephrine enhances plasticity - moderate levels improve learning
    return 0.6f + 0.4f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Stress recovery over time
    if (pImpl->stress > 0.0f) {
        pImpl->stress = std::max(0.0f, pImpl->stress - pImpl->decayRate * 0.5f * static_cast<float>(dt));
    }
    
    // Arousal and vigilance decay
    pImpl->arousal = std::max(0.1f, pImpl->arousal - pImpl->decayRate * static_cast<float>(dt));
    pImpl->vigilance = std::max(0.1f, pImpl->vigilance - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // Norepinephrine directly controls arousal and alertness
    pImpl->arousal = std::min(1.0f, pImpl->arousal + arousalLevel);
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * pImpl->releaseRate);
}

void Norepinephrine::enhanceVigilance(float vigilanceStrength) {
    // Norepinephrine increases vigilance and attention to salient stimuli
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + vigilanceStrength * 0.8f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceStrength * pImpl->releaseRate * 0.7f);
}

void Norepinephrine::signalStress(float stressLevel, float recoveryRate) {
    // Norepinephrine responds to stress and affects adaptive behavior
    pImpl->stress = std::min(1.0f, pImpl->stress + stressLevel);
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->releaseRate);
}

void Norepinephrine::enhanceSensoryProcessing(float gainFactor) {
    // Norepinephrine enhances sensory signal gain
    pImpl->sensoryGain = std::min(2.0f, pImpl->sensoryGain + gainFactor * 0.5f);
    if (gainFactor > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + gainFactor * 0.2f);
    }
}

void Norepinephrine::modulateWorkingMemoryCapacity(float capacityModulator) {
    // Norepinephrine affects working memory capacity and retention
    pImpl->wmCapacity = std::min(1.5f, pImpl->wmCapacity + capacityModulator * 0.5f);
    if (capacityModulator > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + capacityModulator * 0.3f);
    }
}

} // namespace nlm