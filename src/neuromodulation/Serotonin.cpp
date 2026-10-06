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
    float releaseRate;
    float mood;
    bool socialApproach;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f), releaseRate(1.0f),
             mood(0.5f), socialApproach(false) {}
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
    // Update mood based on serotonin level
    pImpl->mood = std::max(0.0f, std::min(1.0f, 0.3f + pImpl->level * 0.6f));
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects mood and social behavior
    // Moderate levels optimize learning and reduce impulsivity
    float factor = 1.0f;
    if (pImpl->level > 0.5f) {
        // High serotonin reduces impulsivity, stabilizes learning
        factor = std::max(0.7f, 1.0f - (pImpl->level - 0.5f) * 0.2f);
    }
    return factor;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::modulateMood(float mood) {
    pImpl->mood = std::clamp(mood, 0.0f, 1.0f);
    // Set serotonin level based on mood
    setLevel(std::max(getLevel(), mood * 0.7f));
    NLM_LOG_INFO("Serotonin modulated mood: level=" + std::to_string(getLevel()) + ", mood=" + std::to_string(mood));
}

void Serotonin::affectSocialBehavior(bool socialApproach) {
    pImpl->socialApproach = socialApproach;
    if (socialApproach) {
        setLevel(std::max(getLevel(), 0.6f));
        NLM_LOG_INFO("Serotonin promoted social approach behavior");
    } else {
        NLM_LOG_INFO("Serotonin promoted social avoidance behavior");
    }
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

bool Serotonin::getSocialApproach() const {
    return pImpl->socialApproach;
}

} // namespace nlm