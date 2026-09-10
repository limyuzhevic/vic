#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float mood;
    float socialBehavior;
    float impulsivity;
    float serotonergicGain;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.2f)
        , peak(1.0f)
        , decayRate(0.08f)
        , mood(0.5f)
        , socialBehavior(0.5f)
        , impulsivity(0.5f)
        , serotonergicGain(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin (5-HT) system initialized - mood and social behavior modulator");
}

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
    // 5-HT reduces impulsive actions and stabilizes decision making
    // Moderate 5-HT improves plasticity, high 5-HT stabilizes behavior
    float moodFactor = pImpl->mood / 0.5f;
    if (pImpl->impulsivity > 0.7f) {
        // High impulsivity: 5-HT reduces it and enhances stability
        return 0.9f * (1.0f - pImpl->impulsivity) + 0.2f;
    } else {
        // Stable behavior: 5-HT maintains moderate plasticity
        return 0.8f + moodFactor * 0.4f;
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update associated mood and behavior
    pImpl->mood = std::clamp(pImpl->mood, 0.0f, 1.0f);
    pImpl->socialBehavior = std::clamp(pImpl->socialBehavior, 0.0f, 1.0f);
    pImpl->impulsivity = std::clamp(pImpl->impulsivity, 0.0f, 1.0f);
}

void Serotonin::signalSocialBehavior(float socialStrength) {
    // 5-HT release associated with positive social interactions
    float boost = socialStrength * pImpl->serotonergicGain;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + socialStrength * 0.3f);
    
    NLM_LOG_INFO("Serotonin signal: social behavior = " + std::to_string(socialStrength) + 
                 ", 5-HT level = " + std::to_string(pImpl->level) + 
                 ", social behavior = " + std::to_string(pImpl->socialBehavior));
}

void Serotonin::enhanceMood(float moodImprovement) {
    // Positive experiences increase mood and 5-HT tone
    float boost = moodImprovement * pImpl->serotonergicGain * 0.8f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
    pImpl->mood = std::min(1.0f, pImpl->mood + moodImprovement * 0.4f);
    
    NLM_LOG_INFO("Serotonin enhancing mood: " + std::to_string(moodImprovement) + 
                 ", 5-HT level = " + std::to_string(pImpl->level) + 
                 ", mood = " + std::to_string(pImpl->mood));
}

void Serotonin::reduceImpulsivity(float reductionStrength) {
    // 5-HT reduces impulsive actions and promotes patient decision making
    float reduction = reductionStrength * pImpl->serotonergicGain;
    pImpl->level = std::min(pImpl->peak, pImpl->level + reduction * 0.5f);
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - reductionStrength * 0.6f);
    
    NLM_LOG_INFO("Serotonin reducing impulsivity: " + std::to_string(reductionStrength) + 
                 ", 5-HT level = " + std::to_string(pImpl->level) + 
                 ", impulsivity = " + std::to_string(pImpl->impulsivity));
}

float Serotonin::getMoodModulation() const {
    return pImpl->mood * (1.0f + pImpl->level);
}

float Serotonin::getImpulsivityReduction() const {
    return 1.0f - pImpl->impulsivity;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->mood = 0.5f;
    pImpl->socialBehavior = 0.5f;
    pImpl->impulsivity = 0.5f;
}

} // namespace nlm