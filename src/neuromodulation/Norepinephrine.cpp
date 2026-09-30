#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float stressResponse;
    float lcFireRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.15f), arousalLevel(0.5f), stressResponse(0.0f), lcFireRate(0.1f) {}
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
    // NE modulates plasticity based on arousal - enhances encoding of salient events
    float arousalFactor = std::min(1.0f, pImpl->arousalLevel * 2.0f);
    return 0.3f + 0.7f * arousalFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: arousal and stress responses decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Arousal modulates level
    if (pImpl->arousalLevel > 0.5f) {
        float arousalBoost = (pImpl->arousalLevel - 0.5f) * 0.3f;
        pImpl->level = std::min(1.0f, pImpl->level + arousalBoost);
    }
}

void Norepinephrine::setArousal(float arousalLevel) {
    pImpl->arousalLevel = std::clamp(arousalLevel, 0.0f, 1.0f);
}

float Norepinephrine::getArousal() const {
    return pImpl->arousalLevel;
}

void Norepinephrine::triggerStressResponse(float stressLevel) {
    // Stress response increases NE level
    pImpl->stressResponse = std::clamp(stressLevel, 0.0f, 1.0f);
    pImpl->level = std::min(1.0f, pImpl->level + pImpl->stressResponse * 0.5f);
}

void Norepinephrine::setLCFireRate(float rate) {
    pImpl->lcFireRate = std::clamp(rate, 0.0f, 1.0f);
    // Update baseline based on LC firing rate
    pImpl->baseline = pImpl->lcFireRate * 0.1f;
}

float Norepinephrine::getLCFireRate() const {
    return pImpl->lcFireRate;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->stressResponse = 0.0f;
}

} // namespace nlm