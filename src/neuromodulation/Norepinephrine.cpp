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
    float locusCoeruleusFiring;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousal(1.0f)
        , vigilance(1.0f)
        , stressResponse(0.0f)
        , locusCoeruleusFiring(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.02f) {}
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
    // NE affects arousal, vigilance, and stress response
    float vigilanceEffect = 1.0f + (pImpl->level * 0.5f * pImpl->vigilance);
    return vigilanceEffect;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: phasic bursts in response to salient events, tonic baseline
    float tonicLevel = pImpl->baseline * (1.0f + pImpl->stressResponse * 0.2f);
    
    // Smooth update towards target
    float target = tonicLevel + pImpl->level * 0.1f;
    pImpl->level += (target - pImpl->level) * 0.08f;
    
    // Decay and natural reset
    pImpl->level = std::max(tonicLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal and vigilance based on level
    pImpl->arousal = 1.0f + getLevel() * 0.8f;
    pImpl->vigilance = 1.0f + getLevel() * 0.6f;
}

void Norepinephrine::signalStress(float stressLevel) {
    // NE responds to stress and threat detection
    float stressBursts = stressLevel * 0.6f;
    setLevel(std::min(pImpl->peak, getLevel() + stressBursts * 2.0f));
    
    // Increase stress response
    pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + stressLevel * 0.3f);
}

void Norepinephrine::enhanceVigilance() {
    // NE increases vigilance and alertness
    pImpl->vigilance = std::min(2.0f, pImpl->vigilance + 0.5f);
}

void Norepinephrine::updateArousal(float delta) {
    // Changes in arousal level
    pImpl->arousal = std::clamp(pImpl->arousal + delta, 0.5f, 2.0f);
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->stressResponse = 0.0f;
    pImpl->arousal = 1.0f;
    pImpl->vigilance = 1.0f;
}

} // namespace nlm
