#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalLevel;
    float vigilanceWeight;
    float alertnessWeight;
    float locusCoeruleusGain;
    float tonicRelease;
    float phasicTrigger;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousalLevel(0.0f)
        , vigilanceWeight(0.6f)
        , alertnessWeight(0.4f)
        , locusCoeruleusGain(1.0f)
        , tonicRelease(0.05f)
        , phasicTrigger(0.1f) {}
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

void Norepinephrine::setLevel(float level) {
    // NE has both tonic and phasic components
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Update arousal based on NE level
    pImpl->arousalLevel = pImpl->level * 0.8f + pImpl->arousalLevel * 0.2f;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity in a complex way:
    // - Low NE: increases exploration plasticity
    // - Medium NE: optimizes learning rate
    // - High NE: may cause overgeneralization
    
    if (pImpl->level < 0.3f) {
        // Low arousal promotes exploration
        return 1.2f + pImpl->level * 0.5f;
    } else if (pImpl->level < 0.7f) {
        // Optimal arousal for learning
        return 1.5f - (pImpl->level - 0.3f) * 0.5f;
    } else {
        // High arousal may cause overgeneralization
        return std::max(0.8f, 1.2f - (pImpl->level - 0.7f) * 1.0f);
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay of phasic responses
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->tonicRelease * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float intensity, float duration) {
    // Phasic bursts of NE in response to salient events
    // This simulates locus coeruleus firing patterns
    
    float burstAmount = intensity * pImpl->phasicTrigger;
    // Duration matters: longer events cause sustained NE release
    float sustainedAmount = burstAmount * std::min(1.0f, duration * 0.5f);
    
    setLevel(pImpl->level + burstAmount + sustainedAmount);
}

float Norepinephrine::getVigilance() const {
    // Vigilance increases with NE level
    if (pImpl->level < 0.2f) return 0.0f;
    return std::min(1.0f, (pImpl->level - 0.2f) * 2.0f);
}

float Norepinephrine::getAlertness() const {
    // Alertness is the response to unexpected or surprising events
    // In a full implementation, this would integrate prediction error
    return pImpl->level * pImpl->alertnessWeight;
}

} // namespace nlm