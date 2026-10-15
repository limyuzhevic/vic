#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float moodLevel;              // 0.0f to 1.0f - positive vs negative mood
    float impulseControl;          // 0.0f to 1.0f - inhibition of impulsive actions
    float socialDominance;        // 0.0f to 1.0f - social engagement level
    float rewardSensitivity;       // 0.0f to 1.0f - response to rewards
    float punishmentSensitivity;   // 0.0f to 1.0f - response to punishments
    
    // Time constants
    static constexpr float MOOD_TIME_CONSTANT = 0.2f;          // seconds - mood changes slower
    static constexpr float IMPULSE_TIME_CONSTANT = 0.1f;      // seconds - impulse control adjusts faster
    static constexpr float DECAY_RATE = 0.02f;                // per timestep - slow decay
    static constexpr float MOOD_FLUCTUATION = 0.1f;            // mood fluctuation magnitude
    static constexpr float IMPULSE_MAX_CONTROL = 0.95f;       // max impulse control
    
    // Integration weights
    static constexpr float MOOD_REWARD_WEIGHT = 0.7f;
    static constexpr float IMPULSE_PUNISHMENT_WEIGHT = 0.6f;
    static constexpr float SOCIAL_REWARD_WEIGHT = 0.5f;
    
    Impl()
        : level(0.0f)
        , moodLevel(0.5f)           // Start with neutral mood
        , impulseControl(0.5f)
        , socialDominance(0.5f)
        , rewardSensitivity(1.0f)
        , punishmentSensitivity(1.0f) {}
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
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity: enhances consolidation of learned patterns
    // High serotonin promotes stable memory formation
    return 1.0f + pImpl->moodLevel * 0.2f - std::abs(pImpl->moodLevel - 0.5f) * 0.1f;
}

void Serotonin::update(TimestepDuration dt) {
    // Natural mood fluctuations based on internal dynamics
    float moodTarget = 0.5f + (std::rand() % 200 - 100) / 200.0f * pImpl->MOOD_FLUCTUATION;
    moodTarget = std::clamp(moodTarget, 0.0f, 1.0f);
    
    // Smooth mood adjustment
    pImpl->moodLevel += (moodTarget - pImpl->moodLevel) * 
                       pImpl->MOOD_TIME_CONSTANT * static_cast<float>(dt);
    
    // Impulse control adapts based on mood
    float impulseTarget = pImpl->moodLevel;  // Positive mood = better impulse control
    pImpl->impulseControl += (impulseTarget - pImpl->impulseControl) * 
                           pImpl->IMPULSE_TIME_CONSTANT * static_cast<float>(dt);
    
    // Social dominance based on mood
    pImpl->socialDominance = pImpl->moodLevel;  // Positive mood = more social engagement
    
    // Reward and punishment sensitivity modulated by mood
    pImpl->rewardSensitivity = 1.0f - std::abs(pImpl->moodLevel - 0.5f) * 0.5f;
    pImpl->punishmentSensitivity = 1.0f + std::abs(pImpl->moodLevel - 0.5f) * 0.3f;
    
    // Natural decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->DECAY_RATE * static_cast<float>(dt));
    
    // Clamp impulse control
    pImpl->impulseControl = std::min(pImpl->IMPULSE_MAX_CONTROL, pImpl->impulseControl);
    
    NLM_LOG_DEBUG(std::string("5-HT level: ") + std::to_string(pImpl->level) +
                  ", mood: " + std::to_string(pImpl->moodLevel) +
                  ", impulse control: " + std::to_string(pImpl->impulseControl) +
                  ", social dominance: " + std::to_string(pImpl->socialDominance));
}

void Serotonin::modulateMood(float moodLevel) {
    pImpl->moodLevel = std::clamp(moodLevel, 0.0f, 1.0f);
}

void Serotonin::regulateImpulsivity(float impulsivityLevel) {
    float targetImpulseControl = 1.0f - std::clamp(impulsivityLevel, 0.0f, 1.0f);
    pImpl->impulseControl = std::min(targetImpulseControl, pImpl->IMPULSE_MAX_CONTROL);
}

void Serotonin::modulateSocialBehavior(const Brain* brain) {
    if (!brain || pImpl->socialDominance <= 0.01f) return;
    
    // Enhance social behavior when dominance is high
    NLM_LOG_DEBUG(std::string("5-HT modulating social behavior (dominance: ") + 
                  std::to_string(pImpl->socialDominance) + ")");
}

void Serotonin::facilitatePatternLearning(const Brain* brain) {
    if (!brain || pImpl->rewardSensitivity <= 0.01f) return;
    
    // Serotonin facilitates pattern learning through reward modulation
    NLM_LOG_DEBUG(std::string("5-HT facilitating pattern learning (sensitivity: ") + 
                  std::to_string(pImpl->rewardSensitivity) + ")");
}

void Serotonin::modulatePunishmentLearning(const Brain* brain) {
    if (!brain || pImpl->punishmentSensitivity <= 0.01f) return;
    
    // Serotonin modulates punishment-based learning
    NLM_LOG_DEBUG(std::string("5-HT modulating punishment learning (sensitivity: ") + 
                  std::to_string(pImpl->punishmentSensitivity) + ")");
}

void Serotonin::enhanceMemoryPrecision(const Brain* brain) {
    if (!brain || pImpl->moodLevel <= 0.0f) return;
    
    // High serotonin enhances memory precision (engram formation)
    NLM_LOG_DEBUG(std::string("5-HT enhancing memory precision (mood: ") + 
                  std::to_string(pImpl->moodLevel) + ")");
}

void Serotonin::facilitatePlanEvaluation(const Brain* brain) {
    if (!brain || pImpl->socialDominance <= 0.01f) return;
    
    // Serotonin facilitates evaluation of action plans
    NLM_LOG_DEBUG(std::string("5-HT facilitating plan evaluation (dominance: ") + 
                  std::to_string(pImpl->socialDominance) + ")");
}

void Serotonin::modulateRiskAssessment(const Brain* brain) {
    if (!brain || pImpl->moodLevel <= 0.0f) return;
    
    // Modulate risk assessment based on mood
    NLM_LOG_DEBUG(std::string("5-HT modulating risk assessment (mood: ") + 
                  std::to_string(pImpl->moodLevel) + ")");
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->moodLevel = 0.5f;
    pImpl->impulseControl = 0.5f;
    pImpl->socialDominance = 0.5f;
    pImpl->rewardSensitivity = 1.0f;
    pImpl->punishmentSensitivity = 1.0f;
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulseControl() const {
    return pImpl->impulseControl;
}

float Serotonin::getSocialDominance() const {
    return pImpl->socialDominance;
}

} // namespace nlm