#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), arousalLevel(1.0f), vigilanceGain(1.2f) {}
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
    // NE modulates arousal and memory consolidation
    // Higher NE increases arousal and stabilizes memory traces
    float consolidationFactor = 1.0f + pImpl->level * 0.5f;
    return std::min(1.8f, consolidationFactor);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE decays towards baseline, increases with stress/arousal
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalStress(float stressLevel) {
    // NE increases with stress/arousal
    if (stressLevel > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (stressLevel - 0.5f) * pImpl->releaseRate * 1.5f);
    }
}

void Norepinephrine::enhanceMemoryConsolidation(const std::vector<float>& trace) {
    // NE enhances memory consolidation during high arousal
    if (!trace.empty()) {
        float traceStrength = 0.0f;
        for (float val : trace) {
            traceStrength += std::abs(val);
        }
        
        float consolidationBoost = traceStrength * 0.15f * pImpl->level;
        // This would be applied to memory consolidation system
    }
}

void Norepinephrine::increaseVigilance(float baselineInhibition) {
    // NE increases vigilance and reduces behavioral inhibition
    float vigilanceMod = 1.0f + pImpl->level * pImpl->vigilanceGain;
    // This would be applied to action selection system
}

} // namespace nlm