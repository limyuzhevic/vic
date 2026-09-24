#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float mood;
    float impulsivity;
    float socialBehavior;
    float memoryModulation;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , mood(0.5f)
        , impulsivity(0.5f)
        , socialBehavior(0.5f)
        , memoryModulation(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() {
    delete pImpl;
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Update dependent factors
    pImpl->mood = 0.5f + 0.4f * pImpl->level;
    pImpl->impulsivity = 0.5f - 0.3f * pImpl->level;
    pImpl->socialBehavior = 0.5f + 0.3f * pImpl->level;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates learning rate and memory stability
    // Higher serotonin reduces plasticity (stabilizes memories)
    return 0.3f + 0.7f * (1.0f - pImpl->level);
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin fluctuates with mood and social context
    // Simplified model: baseline decay with occasional bursts
    float targetLevel = 0.3f + 0.2f * std::sin(static_cast<float>(dt) * 0.01f);
    pImpl->level = pImpl->level * 0.95f + targetLevel * 0.05f;
}

void Serotonin::signalMoodChange(float moodShift) {
    pImpl->mood += moodShift * 0.1f;
    pImpl->mood = std::clamp(pImpl->mood, 0.0f, 1.0f);
    setLevel(std::min(1.0f, pImpl->level + moodShift * 0.05f));
    NLM_LOG_INFO(std::string("Serotonin mood change: ") + std::to_string(moodShift));
}

void Serotonin::signalSocialExperience(float socialValue) {
    pImpl->socialBehavior = (pImpl->socialBehavior + socialValue * 0.1f);
    pImpl->socialBehavior = std::clamp(pImpl->socialBehavior, 0.0f, 1.0f);
    setLevel(std::min(1.0f, pImpl->level + socialValue * 0.03f));
    NLM_LOG_INFO(std::string("Serotonin social experience: ") + std::to_string(socialValue));
}

} // namespace nlm
