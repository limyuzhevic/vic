#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousal;
    float vigilance;
    float locusCoeruleusGain;
    float baseline;
    float peak;
    float stressLevel;
    float rewardSensitivity;
    
    Impl() : brain(nullptr), level(0.0f), arousal(0.0f), vigilance(0.0f), locusCoeruleusGain(1.0f),
             baseline(0.0f), peak(2.0f), stressLevel(0.0f), rewardSensitivity(1.0f) {}
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
    // NE modulates arousal and stress response
    // High NE increases alertness but can impair fine motor coordination
    // Effects depend on context and baseline stress level
    float stressMod = 1.0f + pImpl->stressLevel * 0.5f;
    return 0.8f + pImpl->level * stressMod;  // 0.8 to 2.4x modulation
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: increases with stress/reward, decays slowly
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.02f * static_cast<float>(dt));
    
    // Update vigilance based on current level
    // High NE = high vigilance but increased anxiety
    pImpl->vigilance = pImpl->level * 0.8f;
}

void Norepinephrine::enhanceAlertness(float stress, float rewardPrediction) {
    // LC-NE system responds to stressors and unexpected rewards
    // This is the "fight or flight" signal
    float alertnessSignal = stress * 0.7f + rewardPrediction * 0.3f;
    
    // Update level based on alerting signal
    if (alertnessSignal > pImpl->level) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + alertnessSignal * 0.3f);
    }
    pImpl->stressLevel = stress;
    pImpl->rewardSensitivity = rewardPrediction;
}

void Norepinephrine::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized - arousal and vigilance modulation ready");
}

} // namespace nlm