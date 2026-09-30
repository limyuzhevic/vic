#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

Serotonin::Serotonin()
    : level(0.0f)
    , baseline(0.0f)
    , peak(1.0f)
    , decayRate(0.1f)
    , releaseRate(1.0f)
    , moodLevel(0.0f)
    , socialApproach(0.0f)
    , impulseControl(1.0f)
{
}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return level;
}

void Serotonin::setLevel(float lvl) {
    level = std::clamp(lvl, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Implement real serotonin effects
    // Serotonin modulates mood, social behavior, and impulse control
    float moodBoost = std::min(1.0f, moodLevel * 1.5f);
    float socialBoost = std::min(1.0f, socialApproach * 1.2f);
    float impulseBoost = std::min(1.0f, (1.0f - impulseControl) * 0.8f);
    
    // Combined effects on learning and memory
    float stabilityFactor = moodBoost * 0.4f + socialBoost * 0.3f + impulseBoost * 0.3f;
    
    return 0.5f + stabilityFactor * 0.5f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    level = std::max(baseline, level - decayRate * static_cast<float>(dt));
    moodLevel = std::max(0.0f, moodLevel - decayRate * static_cast<float>(dt));
    socialApproach = std::max(0.0f, socialApproach - decayRate * static_cast<float>(dt));
    // impulseControl tends to recover towards 1.0
    impulseControl = std::min(1.0f, impulseControl + decayRate * static_cast<float>(dt) * 0.5f);
}

void Serotonin::signalPleasure() {
    // Burst of serotonin on pleasure/mood
    moodLevel = std::min(1.0f, moodLevel + 0.5f);
    level = std::min(peak, level + 0.4f);
}

void Serotonin::signalAnhedonia() {
    // Reduced mood/anhdeponia
    moodLevel = std::max(0.0f, moodLevel - 0.3f);
    level = std::max(0.0f, level - 0.2f);
}

float Serotonin::getMoodLevel() const {
    return moodLevel;
}

void Serotonin::signalSocialReward() {
    // Social reward signals
    socialApproach = std::min(1.0f, socialApproach + 0.4f);
    level = std::min(peak, level + 0.3f);
}

float Serotonin::getSocialApproach() const {
    return socialApproach;
}

void Serotonin::signalImpulse() {
    // Impulsive behavior reduces impulse control
    impulseControl = std::max(0.0f, impulseControl - 0.3f);
    level = std::max(0.0f, level - 0.1f);
}

float Serotonin::getImpulseControl() const {
    return impulseControl;
}

} // namespace nlm