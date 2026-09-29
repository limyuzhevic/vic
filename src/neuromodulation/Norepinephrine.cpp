#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float vigilanceLevel;
    float locusCoeruleusGain;
    
    // For signal detection and noise reduction
    float signalToNoiseRatio;
    
    Impl() : level(0.0f), arousalLevel(0.0f), vigilanceLevel(0.0f), 
             locusCoeruleusGain(1.0f), signalToNoiseRatio(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}
Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine (NE)";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
    
    // NE affects arousal and vigilance
    pImpl->arousalLevel = std::min(pImpl->level, 1.0f);
    pImpl->vigilanceLevel = std::max(0.0f, pImpl->level - 1.0f);
    
    // Update signal-to-noise ratio
    pImpl->signalToNoiseRatio = 1.0f + pImpl->arousalLevel * 2.0f;
    
    NLM_LOG_INFO(std::string("Norepinephrine: Level = ") + std::to_string(level) + 
                " Arousal = " + std::to_string(pImpl->arousalLevel) + 
                " Vigilance = " + std::to_string(pImpl->vigilanceLevel));
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity with complex effects
    return std::clamp(1.0f + pImpl->arousalLevel * 0.5f - pImpl->vigilanceLevel * 0.2f, 0.5f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics are complex - gradual decay
    pImpl->level *= 0.97f;
    
    // Update derived measures
    pImpl->arousalLevel = std::min(pImpl->level, 1.0f);
    pImpl->vigilanceLevel = std::max(0.0f, pImpl->level - 1.0f);
    pImpl->signalToNoiseRatio = 1.0f + pImpl->arousalLevel * 2.0f;
}

} // namespace nlm