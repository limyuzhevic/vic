#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousal;
    float vigilance;
    float stressResponse;
    float decayRate;
    float releaseRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousal(1.0f)
        , vigilance(1.0f)
        , stressResponse(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates arousal and attention
    // Higher NE increases alertness and signal detection
    float arousalFactor = 1.0f + pImpl->level * pImpl->arousal;
    float vigilanceFactor = 1.0f + pImpl->level * pImpl->vigilance;
    return (arousalFactor * vigilanceFactor) / 2.0f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float intensity) {
    // Signal arousal event (stress, excitement, novelty)
    pImpl->level = std::min(1.0f, pImpl->level + intensity * pImpl->releaseRate);
    NLM_LOG_INFO("Norepinephrine arousal: " + std::to_string(intensity));
}

void Norepinephrine::signalStressResponse(float magnitude) {
    // Signal stress response (threat, danger)
    pImpl->level = std::min(1.0f, pImpl->level + magnitude * 0.8f * pImpl->releaseRate);
    NLM_LOG_INFO("Norepinephrine stress response: " + std::to_string(magnitude));
}

void Norepinephrine::signalVigilance(float focus) {
    // Signal increased vigilance (need for focused attention)
    pImpl->level = std::min(1.0f, pImpl->level + focus * 0.6f * pImpl->releaseRate);
    NLM_LOG_INFO("Norepinephrine vigilance: " + std::to_string(focus));
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm