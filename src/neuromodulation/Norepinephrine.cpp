#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baselineLevel;
    float arousalLevel;
    float vigilanceLevel;
    float locusCoeruleusActivity;
    float stressResponse;
};

Norepinephrine::Norepinephrine() : pImpl(new Impl()) {
    pImpl->level = 0.0f;
    pImpl->baselineLevel = 0.02f;
    pImpl->arousalLevel = 0.5f;
    pImpl->vigilanceLevel = 0.5f;
    pImpl->locusCoeruleusActivity = 0.0f;
    pImpl->stressResponse = 0.0f;
}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine (NE)";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // NE affects arousal and alertness
    pImpl->arousalLevel = 0.3f + pImpl->level * 2.7f;
    
    // NE affects vigilance and focus
    pImpl->vigilanceLevel = pImpl->level * 1.5f;
    if (pImpl->vigilanceLevel > 1.0f) pImpl->vigilanceLevel = 1.0f;
    
    // High NE triggers locus coeruleus burst mode
    pImpl->locusCoeruleusActivity = pImpl->level > 0.5f ? 1.0f : 0.0f;
    
    // NE can trigger stress response
    pImpl->stressResponse = pImpl->level > 0.7f ? (pImpl->level - 0.7f) * 3.33f : 0.0f;
    
    NLM_LOG_DEBUG("Norepinephrine level set to " + std::to_string(level) + 
                  ", arousal: " + std::to_string(pImpl->arousalLevel) +
                  ", vigilance: " + std::to_string(pImpl->vigilanceLevel));
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal state
    // Optimal arousal (moderate NE) enhances learning
    if (pImpl->arousalLevel > 0.5f && pImpl->arousalLevel < 1.5f) {
        return 1.5f;  // Enhanced plasticity
    } else if (pImpl->arousalLevel > 2.0f) {
        return 0.5f;  // High stress impairs plasticity
    } else {
        return 1.0f;  // Baseline
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Gradual decay
    pImpl->level *= std::pow(0.95f, dt);
    
    // Reset stress response gradually
    pImpl->stressResponse *= std::pow(0.8f, dt);
}

void Norepinephrine::signalArousalIncrease(float intensity) {
    setLevel(std::min(pImpl->level + intensity, 1.0f));
}

void Norepinephrine::signalStress(float stressLevel) {
    if (stressLevel > pImpl->stressResponse) {
        pImpl->stressResponse = stressLevel;
        setLevel(std::min(pImpl->level + stressLevel * 0.3f, 1.0f));
    }
}

void Norepinephrine::triggerBurstMode() {
    // Locus coeruleus burst firing enhances signal detection
    pImpl->locusCoeruleusActivity = 1.0f;
    setLevel(std::min(pImpl->level + 0.3f, 1.0f));
    NLM_LOG_INFO("NE burst mode triggered - enhanced signal detection");
}

} // namespace nlm