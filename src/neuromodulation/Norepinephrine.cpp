#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalGain;
    float vigilanceBoost;
    float stressResponse;
    float resetThreshold;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.08f), arousalGain(0.3f), 
             vigilanceBoost(0.7f), stressResponse(0.5f), resetThreshold(0.1f) {}
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
    // NE modulates plasticity: moderate levels enhance learning, high levels can impair
    if (pImpl->level < 0.3f) {
        return 0.3f + 0.7f * (pImpl->level / 0.3f);  // Low NE: gradual increase in plasticity
    } else {
        return 1.0f - 0.5f * ((pImpl->level - 0.3f) / 0.7f);  // High NE: reduces plasticity
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: decays slowly, can burst during stress/arousal
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float stress, float novelty, float threat) {
    // NE bursts during arousal, stress, or threat detection
    float arousal = std::clamp((stress * 0.5f + novelty * 0.3f + threat * 0.8f), 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal);
}

void Norepinephrine::resetLevel() {
    // Reset to baseline when safety is restored
    if (pImpl->level < pImpl->resetThreshold) {
        pImpl->level = pImpl->baseline;
    }
}

} // namespace nlm