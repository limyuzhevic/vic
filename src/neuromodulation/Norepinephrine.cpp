#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float alertness;
    float sensoryGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             arousalLevel(0.5f), alertness(1.0f), sensoryGain(1.0f) {}
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
    // NE modulates arousal and attention, affecting plasticity
    // Higher NE increases alertness and can enhance learning for salient stimuli
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate arousal and alertness based on level
    // NE has complex effects on cognitive processing
}

void Norepinephrine::increaseArousal(float level) {
    // Increase overall arousal state
    pImpl->arousalLevel = std::clamp(pImpl->arousalLevel + level, 0.0f, 1.0f);
    
    // This effect would be implemented in Brain integration
    // Increased arousal enhances vigilance and response to salient stimuli
}

void Norepinephrine::modulateAlertness(float level) {
    // Modulate alertness and wakefulness
    pImpl->alertness = std::clamp(pImpl->alertness + level, 0.5f, 2.0f);
    
    // This effect would be implemented in Brain integration
    // Higher alertness improves attentional focus and processing speed
}

void Norepinephrine::enhanceSensoryProcessing(float gain) {
    // Enhance sensory signal gain
    pImpl->sensoryGain = std::clamp(pImpl->sensoryGain + gain, 0.5f, 3.0f);
    
    // This effect would be implemented in Brain integration
    // Enhanced sensory gain makes weak stimuli more detectable
}

void Norepinephrine::enhanceAttentionToNovelty(float strength) {
    // Bias attention toward novel or unexpected stimuli
    // This is a parameter that would be used in Brain
}

void Norepinephrine::modulateStressResponse(float level) {
    // Modulate stress response system
    // NE is key in stress response and can affect learning and memory
    // This effect would be implemented in Brain integration
}

void Norepinephrine::optimizeInformationProcessing(float efficiency) {
    // Enhance information processing efficiency
    // NE modulates prefrontal cortex function for better executive control
    // This effect would be implemented in Brain integration
}

} // namespace nlm