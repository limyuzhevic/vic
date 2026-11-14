#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal and vigilance state
    float arousalLevel;
    float vigilance;
    
    // Motor activation modulation
    float motorGain;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , arousalLevel(0.5f)
        , vigilance(0.5f)
        , motorGain(0.5f) {}
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
    // NE enhances plasticity under certain conditions
    // Better arousal can improve learning efficiency
    float plasticity = 1.0f;
    
    if (pImpl->level > 0.3f) {
        // Moderate NE enhances plasticity (optimal arousal)
        plasticity = 1.5f;
    } else if (pImpl->level > 0.6f) {
        // High NE can impair plasticity (too much arousal)
        plasticity = 1.2f;
    }
    
    return plasticity;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay arousal and vigilance
    pImpl->arousalLevel = std::max(0.1f, pImpl->arousalLevel - 0.01f * static_cast<float>(dt));
    pImpl->vigilance = std::max(0.1f, pImpl->vigilance - 0.01f * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalSignal) {
    // Increase NE level based on arousal/stress
    // Stress response increases NE release
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalSignal * pImpl->releaseRate * 2.0f);
    
    // Update arousal state
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + arousalSignal * 0.1f);
}

void Norepinephrine::signalVigilance(float vigilanceSignal) {
    // Increase vigilance/alertness
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + vigilanceSignal * 0.2f);
    
    // Modulate NE release based on vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceSignal * pImpl->releaseRate * 0.5f);
}

void Norepinephrine::setMotorGain(float gain) {
    pImpl->motorGain = std::clamp(gain, 0.1f, 2.0f);
}

float Norepinephrine::getMotorGain() const {
    return pImpl->motorGain;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

} // namespace nlm