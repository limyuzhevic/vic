#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // 5-HT-specific state
    float mood;                  // Mood regulation
    float sleepWake;             // Sleep/wake cycle regulation
    float rewardProcessing;      // Reward processing
    float anxiety;               // Anxiety modulation
    float impulsivity;           // Impulsivity control
    float socialBehavior;        // Social behavior modulation
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.8f)              // 5-HT can have strong effects
        , decayRate(0.04f)
        , releaseRate(1.2f)
        , mood(1.0f)
        , sleepWake(1.0f)
        , rewardProcessing(1.0f)
        , anxiety(0.5f)
        , impulsivity(1.0f)
        , socialBehavior(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

void Serotonin::signalMood(float level) {
    // Regulate mood state
    pImpl->mood = std::clamp(level, 0.3f, 2.0f);
}

void Serotonin::signalSleepWake(float cycle) {
    // Regulate sleep/wake cycle
    pImpl->sleepWake = std::clamp(cycle, 0.1f, 2.0f);
}

void Serotonin::signalRewardProcessing(float level) {
    // Enhance reward processing and learning
    pImpl->rewardProcessing = std::clamp(level, 0.5f, 2.0f);
}

void Serotonin::signalAnxiety(float level) {
    // Modulate anxiety and stress response
    pImpl->anxiety = std::clamp(level, 0.1f, 1.5f);
}

void Serotonin::signalImpulsivity(float level) {
    // Control impulsivity and decision-making
    pImpl->impulsivity = std::clamp(level, 0.3f, 1.5f);
}

void Serotonin::signalSocialBehavior(float level) {
    // Modulate social behavior and bonding
    pImpl->socialBehavior = std::clamp(level, 0.5f, 2.0f);
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update mood based on 5-HT level
    pImpl->mood = std::clamp(1.0f + pImpl->level * 0.5f, 0.3f, 2.0f);
    // Update reward processing based on 5-HT level
    pImpl->rewardProcessing = std::clamp(1.0f + pImpl->level * 0.3f, 0.5f, 2.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT enhances certain types of plasticity
    return 0.8f + 0.2f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay 5-HT level
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Get current mood level
float Serotonin::getMood() const {
    return pImpl->mood;
}

// Get sleep/wake regulation
float Serotonin::getSleepWake() const {
    return pImpl->sleepWake;
}

// Get reward processing enhancement
float Serotonin::getRewardProcessing() const {
    return pImpl->rewardProcessing;
}

// Get anxiety level
float Serotonin::getAnxiety() const {
    return pImpl->anxiety;
}

// Get impulsivity control
float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

// Get social behavior modulation
float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

} // namespace nlm