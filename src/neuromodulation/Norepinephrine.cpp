#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalWeight;
    float vigilanceWeight;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousalWeight(0.6f)
        , vigilanceWeight(0.4f)
        , decayRate(0.03f) {}
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
    // NE modulates plasticity for arousal-related processes
    return 0.7f + 0.3f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE affects arousal and vigilance
    // In a real implementation, this would affect general neural excitability
    // and signal detection sensitivity
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float intensity) {
    // NE responds to novel or salient stimuli
    pImpl->level = std::min(1.0f, pImpl->level + intensity * 0.4f);
}

void Norepinephrine::enhanceVigilance(float vigilance) {
    // NE supports sustained attention and alertness
    pImpl->level = std::min(1.0f, pImpl->level + vigilance * 0.3f);
}

} // namespace nlm