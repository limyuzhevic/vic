#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float moodLevel;
    float impulsivityLevel;
    float socialBehaviorModulator;
    
    // For behavioral state modulation
    float behavioralState;
    float rewardSensitivity;
    
    Impl() : level(0.0f), moodLevel(0.5f), impulsivityLevel(0.5f), 
             socialBehaviorModulator(1.0f), behavioralState(0.5f), rewardSensitivity(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}
Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin (5-HT)";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
    
    // 5-HT affects mood and impulsivity
    pImpl->moodLevel = 0.3f + pImpl->level * 0.35f; // 0.3 to 1.0
    pImpl->impulsivityLevel = 0.5f + pImpl->level * 0.3f; // 0.5 to 1.4
    pImpl->socialBehaviorModulator = 0.5f + pImpl->level * 0.5f; // 0.5 to 1.5
    
    // Update reward sensitivity
    pImpl->rewardSensitivity = 0.8f + pImpl->level * 0.4f; // 0.8 to 2.0
    
    NLM_LOG_INFO(std::string("Serotonin: Level = ") + std::to_string(level) + 
                " Mood = " + std::to_string(pImpl->moodLevel) + 
                " Impulsivity = " + std::to_string(pImpl->impulsivityLevel));
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT typically reduces plasticity (stabilizes memories)
    return std::clamp(1.0f - pImpl->level * 0.2f, 0.5f, 1.5f);
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics
    pImpl->level *= 0.96f;
    
    // Update derived measures
    pImpl->moodLevel = 0.3f + pImpl->level * 0.35f;
    pImpl->impulsivityLevel = 0.5f + pImpl->level * 0.3f;
    pImpl->socialBehaviorModulator = 0.5f + pImpl->level * 0.5f;
}

} // namespace nlm