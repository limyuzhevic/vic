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
    float impulsivity;
    float socialBehavior;
    
    Impl() 
        : brain(nullptr)
        , level(0.05f)  // Baseline serotonin
        , baseline(0.05f)
        , peak(0.25f)
        , decayRate(0.04f)
        , releaseRate(0.6f)
        , mood(0.8f)
        , impulsivity(1.0f)
        , socialBehavior(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized for mood and social behavior");
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

void Serotonin::enhanceMood(float amount) {
    pImpl->mood = std::min(1.0f, pImpl->mood + amount);
}

void Serotonin::reduceImpulsivity(float amount) {
    pImpl->impulsivity = std::max(0.1f, pImpl->impulsivity - amount);
}

void Serotonin::enhanceSocialBehavior(float amount) {
    pImpl->socialBehavior = std::min(1.5f, pImpl->socialBehavior + amount);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity for behavioral inhibition and mood regulation
    // Higher 5-HT promotes stable, conservative learning
    float factor = 0.7f + pImpl->level * 0.8f;  // Range: ~0.7 to 1.5
    return std::clamp(factor, 0.3f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics: slow, sustained modulation
    float targetLevel = pImpl->baseline;
    
    // Decay towards baseline (slow)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add small noise
    static float noise = 0.0f;
    noise = noise * 0.95f + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.02f;
    pImpl->level += noise * 0.05f;
}

void Serotonin::signalRewardOrSocialFeedback(float feedback) {
    // 5-HT responds to reward prediction errors and social feedback
    if (feedback > 0.0f) {
        // Positive feedback (reward, acceptance) increases 5-HT
        float increase = std::min(0.08f, feedback * pImpl->releaseRate);
        pImpl->level = std::min(pImpl->peak, pImpl->level + increase);
        
        // Improve mood
        enhanceMood(feedback * 0.2f);
        
        // Enhance social behavior
        enhanceSocialBehavior(feedback * 0.1f);
    } else if (feedback < 0.0f) {
        // Negative feedback (punishment, rejection) decreases 5-HT
        float decrease = std::min(0.05f, std::abs(feedback) * pImpl->decayRate * 2.0f);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decrease);
        
        // Increase impulsivity (negative mood)
        reduceImpulsivity(std::abs(feedback) * 0.2f);
    }
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->mood = 0.8f;
    pImpl->impulsivity = 1.0f;
    pImpl->socialBehavior = 1.0f;
}

} // namespace nlm