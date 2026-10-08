// Enhanced Serotonin implementation
// Comprehensive serotonin neuromodulator for behavioral inhibition and mood

#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float moodModulation;
    float behavioralInhibition;
    float socialBehaviorModulation;
    
    // Enhanced serotonin state tracking
    float impulsivityControl;
    float anxietyReduction;
    float rewardProcessing;
    float socialCognition;
    float emotionalRegulation;
    
    Impl() : 
        level(0.0f), baseline(0.04f), moodModulation(0.6f),
        behavioralInhibition(0.7f), socialBehaviorModulation(0.8f),
        impulsivityControl(1.0f), anxietyReduction(1.0f),
        rewardProcessing(1.0f), socialCognition(1.0f),
        emotionalRegulation(1.0f) {}
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
    // Serotonin modulates plasticity for emotional and social learning
    float baselinePlasticity = 0.2f;
    
    // Impulsivity control influences reward-based plasticity
    float impulsivityEffect = (1.0f - pImpl->impulsivityControl) * 0.4f;
    
    // Emotional regulation enhances contextual learning
    float emotionalEffect = pImpl->emotionalRegulation * 0.3f;
    
    // Social cognition modulates social learning plasticity
    float socialEffect = pImpl->socialCognition * 0.3f;
    
    return baselinePlasticity + impulsivityEffect + emotionalEffect + socialEffect;
}

void Serotonin::update(TimestepDuration dt) {
    // Enhanced serotonin dynamics with multiple components
    float dtFloat = static_cast<float>(dt);
    
    // Maintain mood modulation with slow decay
    pImpl->moodModulation = std::max(pImpl->baseline,
        pImpl->moodModulation - 0.03f * dtFloat);
    
    // Behavioral inhibition adapts based on experience
    pImpl->behavioralInhibition = std::max(pImpl->baseline,
        pImpl->behavioralInhibition - 0.02f * dtFloat);
    
    // Social behavior modulation is more stable
    pImpl->socialBehaviorModulation = std::max(pImpl->baseline,
        pImpl->socialBehaviorModulation - 0.01f * dtFloat);
    
    // Overall level is weighted sum of components
    pImpl->level = pImpl->moodModulation * 0.3f + 
                  pImpl->behavioralInhibition * 0.4f +
                  pImpl->socialBehaviorModulation * 0.3f;
}

// Enhanced serotonin methods implementation
float Serotonin::getBehavioralInhibitionFactor() const {
    // Serotonin modulates behavioral inhibition and self-control
    return pImpl->behavioralInhibition * getLevel() * 1.5f;
}

float Serotonin::getMoodModulationFactor() const {
    // Serotonin regulates mood and emotional state
    return pImpl->moodModulation * (1.0f + pImpl->emotionalRegulation * 0.3f);
}

float Serotonin::getImpulsivityControlFactor() const {
    // Serotonin controls impulsivity and patience
    return pImpl->impulsivityControl;
}

float Serotonin::getSocialBehaviorModulation() const {
    // Serotonin modulates social behavior and bonding
    return pImpl->socialBehaviorModulation * (1.0f + pImpl->socialCognition * 0.4f);
}

float Serotonin::getAnxietyReductionFactor() const {
    // Serotonin reduces anxiety and fear responses
    return pImpl->anxietyReduction;
}

float Serotonin::getRewardProcessingModulation() const {
    // Serotonin indirectly modulates reward processing through inhibition
    return 1.0f - pImpl->behavioralInhibition * 0.3f;
}

void Serotonin::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

void Serotonin::setMoodModulation(float mood) {
    pImpl->moodModulation = std::clamp(mood, 0.0f, 1.0f);
}

void Serotonin::setBehavioralInhibition(float inhibition) {
    pImpl->behavioralInhibition = std::clamp(inhibition, 0.0f, 1.0f);
}

void Serotonin::setSocialBehaviorModulation(float social) {
    pImpl->socialBehaviorModulation = std::clamp(social, 0.0f, 1.0f);
}

float Serotonin::getBaseline() const {
    return pImpl->baseline;
}

float Serotonin::getMoodModulation() const {
    return pImpl->moodModulation;
}

float Serotonin::getBehavioralInhibition() const {
    return pImpl->behavioralInhibition;
}

float Serotonin::getSocialBehaviorModulation() const {
    return pImpl->socialBehaviorModulation;
}

} // namespace nlm