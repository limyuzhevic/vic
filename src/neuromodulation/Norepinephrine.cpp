#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal and vigilance state
    float arousalLevel;
    float stressLevel;
    float vigilanceLevel;
    std::vector<float> neuralExcitability;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , arousalLevel(0.0f), stressLevel(0.0f), vigilanceLevel(0.0f) {}
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
    // NE modulates plasticity - enhances LTP during high arousal
    // Higher NE increases learning during stress/novelty
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update internal state
    updateInternalState();
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // Increase arousal - increases alertness and attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * pImpl->releaseRate * 0.8f);
    pImpl->arousalLevel = arousalLevel;
    
    // Update neural excitability
    updateNeuralExcitability(arousalLevel);
}

void Norepinephrine::signalStress(float stressLevel) {
    // Stress response - increases NE levels and vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->releaseRate * 1.5f);
    pImpl->stressLevel = stressLevel;
    
    // Stress enhances memory encoding
    updateMemoryEncoding(stressLevel);
}

void Norepinephrine::updateInternalState() {
    // Compute vigilance based on level
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->level * 1.5f);
    
    // Decay internal state
    pImpl->arousalLevel *= 0.95f;
    pImpl->stressLevel *= 0.9f;
    pImpl->vigilanceLevel *= 0.9f;
}

void Norepinephrine::updateNeuralExcitability(float arousalLevel) {
    // Increase neural excitability in key brain regions
    for (auto& excitability : pImpl->neuralExcitability) {
        excitability = std::min(1.0f, excitability + arousalLevel * 0.2f);
    }
}

void Norepinephrine::updateMemoryEncoding(float stressLevel) {
    // Stress enhances memory encoding through amygdala-hippocampal interactions
    // This is handled at a higher level, but we store the stress level
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

} // namespace nlm