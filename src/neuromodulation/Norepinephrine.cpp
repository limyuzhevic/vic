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
    float arousalFactor;
    float vigilanceFactor;
    float reactivityFactor;
    float stressLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.05f), arousalFactor(0.4f), vigilanceFactor(0.3f),
             reactivityFactor(0.2f), stressLevel(0.0f) {}
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
    // NE enhances arousal and vigilance, improving signal-to-noise ratio
    float arousalEffect = std::min(1.0f, pImpl->level * pImpl->arousalFactor);
    float vigilanceEffect = std::min(1.0f, pImpl->level * pImpl->vigilanceFactor);
    float reactivityEffect = std::min(1.0f, pImpl->level * pImpl->reactivityFactor);
    
    // NE has complex effects: moderate levels help learning, high levels can impair
    float optimalEffect = std::exp(-(pImpl->level - 0.5f) * (pImpl->level - 0.5f) * 4.0f);
    
    return 1.0f + optimalEffect * (arousalEffect + vigilanceEffect) - reactivityEffect * 0.2f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics include baseline maintenance and response to stimuli
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Stress adaptation - chronic stress changes NE dynamics
    if (pImpl->stressLevel > 0.0f) {
        pImpl->decayRate = 0.03f;  // Slower decay under stress
        pImpl->baseline += pImpl->stressLevel * 0.01f;
    }
}

void Norepinephrine::signalArousal(float arousal) {
    // NE responds to novel or significant stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->arousalFactor);
    
    // Arousal also affects vigilance
    if (arousal > 0.7f) {
        pImpl->vigilanceFactor = std::min(1.0f, pImpl->vigilanceFactor + 0.1f);
    }
}

void Norepinephrine::increaseVigilance(float vigilance) {
    // Directly increase vigilance factor
    pImpl->vigilanceFactor = std::min(1.0f, pImpl->vigilanceFactor + vigilance * 0.1f);
}

void Norepinephrine::setStressLevel(float stress) {
    pImpl->stressLevel = std::clamp(stress, 0.0f, 1.0f);
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getVigilanceFactor() const {
    return pImpl->vigilanceFactor;
}

void Norepinephrine::setVigilanceFactor(float factor) {
    pImpl->vigilanceFactor = factor;
}

} // namespace nlm