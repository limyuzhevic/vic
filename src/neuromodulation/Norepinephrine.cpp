#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousalGain;
    float stressResponse;
    float recoveryRate;
    float vigilance;
    
    Impl() : level(0.1f), baseline(0.05f), arousalGain(3.0f), 
             stressResponse(1.5f), recoveryRate(0.2f), vigilance(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl), stressEnabled_(true), recoveryRate_(0.3f) {}

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
    // NE modulates plasticity for arousal and attention
    float arousalMod = pImpl->arousalGain * pImpl->level;
    return 1.0f + arousalMod * 0.2f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay and recovery
    float decay = pImpl->recoveryRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update vigilance
    pImpl->vigilance = std::min(1.0f, pImpl->level * 2.0f);
}

float Norepinephrine::getArousalLevel() const {
    // NE increases overall neural excitability
    return pImpl->level * 0.8f + 0.1f;  // Base arousal
}

float Norepinephrine::getVigilanceFactor() const {
    // NE improves vigilance and attention to salient stimuli
    return 0.3f + pImpl->vigilance * 0.7f;
}

void Norepinephrine::signalSurprise(float predictionError) {
    // Strong NE response to prediction errors (surprise)
    float surge = std::min(0.8f, predictionError * 2.0f);
    if (surge > 0.0f) {
        pImpl->level = std::min(1.0f, pImpl->level + surge);
        if (stressEnabled_) {
            NLM_LOG_INFO("Norepinephrine stress response: level=" + std::to_string(pImpl->level) + ", error=" + std::to_string(predictionError));
        }
    }
}

} // namespace nlm
