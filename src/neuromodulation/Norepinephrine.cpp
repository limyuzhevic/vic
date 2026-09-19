#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalSensitivity;
    float stressSensitivity;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousalSensitivity(1.0f)
        , stressSensitivity(0.5f)
        , decayRate(0.05f) {}
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
    // NE modulates plasticity based on arousal
    return 1.0f + 0.3f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Norepinephrine increases with arousal and stress
    float arousal = 0.0f;  // TODO: Get actual arousal signal
    float stress = 0.0f;   // TODO: Get actual stress signal
    
    // Norepinephrine increases with arousal and stress
    float target = pImpl->arousalSensitivity * arousal + 
                   pImpl->stressSensitivity * stress;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

float Norepinephrine::getArousalModulation() const {
    return pImpl->level * pImpl->arousalSensitivity;
}

float Norepinephrine::getSignalDetectionModulation() const {
    // NE improves signal-to-noise ratio
    return 1.0f + 0.8f * pImpl->level;
}

float Norepinephrine::getMemoryEncodingModulation() const {
    // NE enhances memory encoding
    return 1.0f + 0.6f * pImpl->level;
}

void Norepinephrine::setArousalSensitivity(float sensitivity) {
    pImpl->arousalSensitivity = sensitivity;
}

void Norepinephrine::setStressSensitivity(float sensitivity) {
    pImpl->stressSensitivity = sensitivity;
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm