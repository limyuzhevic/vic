#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousal;
    float locusCoeruleusGain;
    float attentionToFocusGain;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.5f)
        , decayRate(0.05f)
        , arousal(1.0f)
        , locusCoeruleusGain(1.2f)
        , attentionToFocusGain(0.8f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine (NE) system initialized - arousal and vigilance modulator");
}

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
    // NE modulates arousal and affects signal-to-noise ratio
    // Moderate NE increases plasticity, very high or low NE decreases it
    float arousalFactor = pImpl->level / pImpl->baseline;
    if (arousalFactor < 0.5f) {
        return 0.6f + arousalFactor * 0.8f;  // Low NE: low plasticity
    } else if (arousalFactor > 1.5f) {
        return 1.2f - (arousalFactor - 1.5f) * 0.4f;  // High NE: reduced plasticity (hyperarousal)
    } else {
        return 1.0f;  // Optimal arousal: balanced plasticity
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalStrength, float attention) {
    // NE release from locus coeruleus in response to salient stimuli
    float burst = arousalStrength * pImpl->locusCoeruleusGain * (1.0f + attention * pImpl->attentionToFocusGain);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    NLM_LOG_INFO("Norepinephrine signal: arousal = " + std::to_string(arousalStrength) + 
                 ", attention = " + std::to_string(attention) + 
                 ", NE level = " + std::to_string(pImpl->level));
}

void Norepinephrine::enhanceAlertness(float alertnessSignal) {
    // NE enhances vigilance and alertness
    float boost = alertnessSignal * pImpl->arousal;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
    
    NLM_LOG_INFO("Norepinephrine enhancing alertness: " + std::to_string(alertnessSignal) + 
                 ", NE level = " + std::to_string(pImpl->level));
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal * (1.0f + pImpl->level);
}

float Norepinephrine::getAttentionToFocusGain() const {
    return pImpl->attentionToFocusGain;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm