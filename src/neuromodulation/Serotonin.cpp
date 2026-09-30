#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodLevel;
    float socialCohesion;
    float rapheActivity;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.08f), moodLevel(0.5f), socialCohesion(0.5f), rapheActivity(0.1f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood - enhances stable memory
    float moodFactor = std::min(1.0f, pImpl->moodLevel * 2.0f);
    return 0.4f + 0.6f * moodFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin decays towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::setMood(float mood) {
    pImpl->moodLevel = std::clamp(mood, 0.0f, 1.0f);
}

float Serotonin::getMood() const {
    return pImpl->moodLevel;
}

void Serotonin::modulateSocialBehavior(float socialCohesion) {
    pImpl->socialCohesion = std::clamp(socialCohesion, 0.0f, 1.0f);
    
    // Increase serotonin level with social cohesion
    pImpl->level = std::min(1.0f, pImpl->level + pImpl->socialCohesion * 0.3f);
    
    // Update raphe nucleus activity
    setRapheActivity(pImpl->socialCohesion * 0.8f);
}

void Serotonin::setRapheActivity(float activity) {
    pImpl->rapheActivity = std::clamp(activity, 0.0f, 1.0f);
    // Update baseline based on raphe activity
    pImpl->baseline = pImpl->rapheActivity * 0.15f;
}

float Serotonin::getRapheActivity() const {
    return pImpl->rapheActivity;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm