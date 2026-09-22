// include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float vigilanceFactor;
    float explorationDrive;
    float decayRate;
    
    Impl() : level(0.0f), arousalLevel(0.0f), vigilanceFactor(1.0f), explorationDrive(0.0f), decayRate(0.1f) {}
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
    // NE directly modulates arousal and vigilance
    pImpl->arousalLevel = 0.5f + 0.5f * pImpl->level;
    pImpl->vigilanceFactor = 0.3f + 0.7f * pImpl->level;
    pImpl->explorationDrive = pImpl->level * 0.4f;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for learning under attention
    // Optimal arousal enhances learning, very high levels cause stress impairment
    float arousal = pImpl->level;
    if (arousal < 0.3f) {
        return 0.8f;  // Low arousal = reduced plasticity
    } else if (arousal < 0.7f) {
        return 1.0f + 0.3f * (arousal - 0.3f) / 0.4f;  // Optimal range
    } else {
        return 1.3f;  // High arousal = enhanced but regulated plasticity
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update vigilance based on recent stimulation
    pImpl->vigilanceFactor = std::clamp(pImpl->vigilanceFactor * 0.95f + 0.05f, 0.1f, 1.0f);
}

void Norepinephrine::signalStimulation(float sensoryInput, float stressLevel) {
    // NE responds to sensory stimulation and stress
    float stimulation = std::min(1.0f, sensoryInput * 0.8f);
    float netInput = stimulation - stressLevel * 0.5f;
    if (netInput > 0) {
        pImpl->level = std::min(1.0f, pImpl->level + netInput * 0.3f);
    }
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceFactor() const {
    return pImpl->vigilanceFactor;
}

float Norepinephrine::getExplorationDrive() const {
    return pImpl->explorationDrive;
}

void Norepinephrine::increaseVigilance(float increment) {
    pImpl->vigilanceFactor = std::min(1.0f, pImpl->vigilanceFactor + increment);
}

} // namespace nlm