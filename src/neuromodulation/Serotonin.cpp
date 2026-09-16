#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float plasticityFactor;
    float impulsivityLevel;
    float socialBehaviorLevel;
    float moodLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , plasticityFactor(1.0f)
        , impulsivityLevel(0.0f)
        , socialBehaviorLevel(0.0f)
        , moodLevel(0.0f) {
    }
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
    pImpl->level = level;
}

float Serotonin::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin modulates mood, impulsivity, and social behavior
    pImpl->impulsivityLevel = pImpl->level * 1.5f;  // Higher serotonin reduces impulsivity
    pImpl->socialBehaviorLevel = pImpl->level * 2.0f;  // Higher serotonin increases social behavior
    pImpl->moodLevel = pImpl->level * 1.5f;  // Higher serotonin improves mood
    
    // Plasticity modulation (Serotonin affects both LTP and LTD)
    pImpl->plasticityFactor = 1.0f - pImpl->level * 0.2f;  // Higher serotonin stabilizes
    
    // Natural decay
    pImpl->level *= (1.0f - dt * 0.01f);
    
    NLM_LOG_INFO(std::string("Serotonin level: ") + std::to_string(pImpl->level) +
                 std::string(" | Impulsivity: ") + std::to_string(1.0f - pImpl->impulsivityLevel) +
                 std::string(" | Social behavior: ") + std::to_string(pImpl->socialBehaviorLevel));
}

} // namespace nlm
