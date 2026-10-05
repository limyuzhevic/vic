#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float arousalRate;
    float vigilanceThreshold;
    float stressResponseRate;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.08f), 
             arousalRate(0.7f), vigilanceThreshold(0.5f), stressResponseRate(0.6f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);  // Higher range for arousal
}

float Norepinephrine::getPlasticityFactor() const {
    // NE increases plasticity for learning and memory during arousal
    return 0.3f + 0.7f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // Signal increased arousal/stress
    pImpl->level = std::min(2.0f, pImpl->level + arousal * pImpl->arousalRate);
}

void Norepinephrine::signalVigilance(float vigilance) {
    // Signal increased vigilance
    if (vigilance > pImpl->vigilanceThreshold) {
        pImpl->level = std::min(2.0f, pImpl->level + (vigilance - pImpl->vigilanceThreshold) * 0.3f);
    }
}

void Norepinephrine::signalStress(float stress) {
    // Stress response - increases NE level
    pImpl->level = std::min(2.0f, pImpl->level + stress * pImpl->stressResponseRate);
}

} // namespace nlm
