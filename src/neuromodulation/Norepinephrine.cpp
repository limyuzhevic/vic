#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

Norepinephrine::Norepinephrine()
    : level(0.0f)
    , baseline(0.0f)
    , peak(1.0f)
    , decayRate(0.1f)
    , releaseRate(1.0f)
    , arousalLevel(0.0f)
    , stressLevel(0.0f)
{
}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return level;
}

void Norepinephrine::setLevel(float lvl) {
    level = std::clamp(lvl, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Implement real norepinephrine effects
    // NE enhances arousal, attention, and working memory plasticity
    float arousalBoost = std::min(1.0f, arousalLevel * 1.5f);
    float stressBoost = std::min(1.0f, stressLevel * 1.2f);
    
    // Combined effects on learning
    float learningEnhancement = arousalBoost * 0.7f + stressBoost * 0.3f;
    
    return 0.5f + learningEnhancement * 0.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    level = std::max(baseline, level - decayRate * static_cast<float>(dt));
    arousalLevel = std::max(0.0f, arousalLevel - decayRate * static_cast<float>(dt));
    stressLevel = std::max(0.0f, stressLevel - decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalAlertness() {
    // Burst of norepinephrine on alertness/arousal
    arousalLevel = std::min(1.0f, arousalLevel + 0.5f);
    level = std::min(peak, level + 0.4f);
}

void Norepinephrine::signalStress() {
    // Stress response
    stressLevel = std::min(1.0f, stressLevel + 0.3f);
    level = std::min(peak, level + 0.2f);
}

float Norepinephrine::getArousalLevel() const {
    return arousalLevel;
}

float Norepinephrine::getLearningEnhancement() const {
    // Return learning enhancement factor for neuromodulation
    return 1.0f + arousalLevel * 0.8f + stressLevel * 0.3f;
}

} // namespace nlm