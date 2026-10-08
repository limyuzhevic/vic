#include "../neuromodulation/Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;                    // Current serotonin concentration
    float mood;                     // Overall mood state
    float impulsivity;              // Effect on impulsivity
    float socialLearning;           // Effect on social learning
    float creativityBoost;         // Effect on creative thinking
    SimulationStep lastUpdate;      // Last update time
    
    Impl()
        : level(0.0f)
        , mood(1.0f)
        , impulsivity(1.0f)
        , socialLearning(1.0f)
        , creativityBoost(1.0f)
        , lastUpdate(0)
    {}
};

Serotonin::Serotonin()
    : pImpl(new Impl)
{
    NLM_LOG_INFO("Serotonin system initialized");
}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::max(0.0f, std::min(level, 1.0f));
    
    // Higher 5-HT improves mood and creativity, reduces impulsivity
    pImpl->mood = 1.0f + pImpl->level * 0.4f;
    pImpl->creativityBoost = 1.0f + pImpl->level * 0.5f;
    pImpl->impulsivity = std::max(0.3f, 1.0f - pImpl->level * 0.7f);  // Reduced impulsivity
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and social context
    return pImpl->creativityBoost * pImpl->socialLearning;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay serotonin level
    pImpl->level *= (1.0f - dt * 0.03f);
    
    // Mood and social effects decay more slowly
    pImpl->mood = 1.0f + (pImpl->mood - 1.0f) * (1.0f - dt * 0.1f);
    pImpl->socialLearning = 1.0f + (pImpl->socialLearning - 1.0f) * (1.0f - dt * 0.1f);
}

void Serotonin::signalPositiveExperience(float reward, float socialContext) {
    // Signal positive experience for social learning
    if (reward > 0.0f) {
        float burst = std::min(1.0f, reward * 0.4f + socialContext * 0.3f);
        setLevel(getLevel() + burst);
        
        pImpl->socialLearning += 0.2f;
        
        NLM_LOG_INFO("Serotonin burst: reward=" + std::to_string(reward) + 
                     " social=" + std::to_string(socialContext) + 
                     " level=" + std::to_string(getLevel()) + 
                     " mood=" + std::to_string(pImpl->mood) + " social_learning=" + std::to_string(pImpl->socialLearning));
    }
}

void Serotonin::signalSocialBonding(float bondingStrength) {
    // Signal social bonding experience
    if (bondingStrength > 0.0f) {
        setLevel(getLevel() + bondingStrength * 0.25f);
        pImpl->socialLearning *= 1.2f;
        
        NLM_LOG_INFO("Social bonding: strength=" + std::to_string(bondingStrength) + 
                     " level=" + std::to_string(getLevel()) + 
                     " social_learning=" + std::to_string(pImpl->socialLearning));
    }
}

float Serotonin::getSocialLearningBoost() const {
    return pImpl->socialLearning;
}

float Serotonin::getCreativityBoost() const {
    return pImpl->creativityBoost;
}

} // namespace nlm
