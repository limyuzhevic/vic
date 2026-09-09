#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float plasticityModulation;
    
    // Mood and social behavior
    float mood;
    float socialBehavior;
    float impulseControl;
    float anxietyLevel;
    
    // Temporal dynamics
    std::vector<float> moodHistory;
    std::vector<float> socialHistory;
    
    Impl()
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.05f)
        , plasticityModulation(1.0f)
        , mood(0.5f)
        , socialBehavior(0.5f)
        , impulseControl(0.5f)
        , anxietyLevel(0.0f) {}
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
    // 5-HT modulates mood, impulsivity, and social behavior
    // Complex effects on plasticity
    float moodEffect = pImpl->mood * 0.3f;  // Positive mood enhances certain plasticity
    float socialEffect = pImpl->socialBehavior * 0.2f;  // Social behavior influences learning
    float impulseEffect = (1.0f - pImpl->impulseControl) * 0.4f;  // Low impulse control can increase risky learning
    float anxietyEffect = pImpl->anxietyLevel * 0.3f;  // Anxiety can inhibit or enhance learning
    
    // Combine effects
    pImpl->plasticityModulation = 1.0f + moodEffect + socialEffect - impulseEffect + anxietyEffect;
    pImpl->plasticityModulation = std::clamp(pImpl->plasticityModulation, 0.5f, 2.0f);
    
    return pImpl->plasticityModulation;
}

void Serotonin::update(TimestepDuration dt) {
    // Real 5-HT dynamics with exponential decay
    if (dt > 0.0) {
        float decayFactor = std::exp(-pImpl->decayRate * dt);
        pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
        pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    }
}

void Serotonin::signalMood(float moodValue) {
    // Mood modulation
    pImpl->mood = std::clamp(moodValue, 0.0f, 1.0f);
    
    // Positive mood increases 5-HT level
    if (moodValue > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (moodValue - 0.5f) * 1.0f);
    }
    
    // Store mood history
    pImpl->moodHistory.push_back(moodValue);
    if (pImpl->moodHistory.size() > 1000) {
        pImpl->moodHistory.erase(pImpl->moodHistory.begin());
    }
}

void Serotonin::signalSocialBehavior(float socialValue) {
    // Social behavior modulation
    pImpl->socialBehavior = std::clamp(socialValue, 0.0f, 1.0f);
    
    // Social interactions can increase or decrease 5-HT
    if (socialValue > 0.7f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f);
    } else if (socialValue < 0.3f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.2f);
    }
    
    // Store social history
    pImpl->socialHistory.push_back(socialValue);
    if (pImpl->socialHistory.size() > 1000) {
        pImpl->socialHistory.erase(pImpl->socialHistory.begin());
    }
}

void Serotonin::signalAnxiety(float anxietyValue) {
    // Anxiety modulation
    pImpl->anxietyLevel = std::clamp(anxietyValue, 0.0f, 1.0f);
    
    // Anxiety can increase 5-HT level
    if (anxietyValue > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (anxietyValue - 0.5f) * 1.2f);
    }
}
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

float Serotonin::getImpulseControl() const {
    return pImpl->impulseControl;
}

float Serotonin::getAnxietyLevel() const {
    return pImpl->anxietyLevel;
}

float Serotonin::getPlasticityModulation() const {
    return pImpl->plasticityModulation;
}

const std::vector<float>& Serotonin::getMoodHistory() const {
    return pImpl->moodHistory;
}

const std::vector<float>& Serotonin::getSocialHistory() const {
    return pImpl->socialHistory;
}

} // namespace nlm
