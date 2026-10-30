#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float targetLevel;
    float moodLevel;
    float impulsivityLevel;
    float socialBehaviorModulation;
    float rapheNucleusActivity;
    float anxietyLevel;
    float rewardProcessingModulation;
    float seroSleepModulation;
    
    static constexpr float DEFAULT_BASELINE = 0.03f;
    static constexpr float DEFAULT_MOOD = 0.5f;
    static constexpr float DEFAULT_IMPEDANCE = 0.6f;
    static constexpr float DEFAULT_SOCIAL_MODULATION = 0.4f;
    static constexpr float DEFAULT_RAPHE_ACTIVITY = 0.7f;
    static constexpr float DEFAULT_ANXIETY = 0.2f;
    static constexpr float DEFAULT_REWARD_MODULATION = 0.5f;
    static constexpr float DEFAULT_SERO_SLEEP = 0.3f;
    
    Impl()
        : level(DEFAULT_BASELINE)
        , baseline(DEFAULT_BASELINE)
        , targetLevel(DEFAULT_BASELINE)
        , moodLevel(DEFAULT_MOOD)
        , impulsivityLevel(1.0f - DEFAULT_IMPEDANCE)
        , socialBehaviorModulation(DEFAULT_SOCIAL_MODULATION)
        , rapheNucleusActivity(DEFAULT_RAPHE_ACTIVITY)
        , anxietyLevel(DEFAULT_ANXIETY)
        , rewardProcessingModulation(DEFAULT_REWARD_MODULATION)
        , seroSleepModulation(DEFAULT_SERO_SLEEP) {}
};

Serotonin::Serotonin() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Serotonin system initialized");
}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->targetLevel = pImpl->level;
    updateDerivedFunctions();
}

float Serotonin::getPlasticityFactor() const {
    float moodEffect = pImpl->moodLevel * 0.25f;
    float impulsivityEffect = (1.0f - pImpl->impulsivityLevel) * 0.25f;
    float socialEffect = pImpl->socialBehaviorModulation * 0.2f;
    float anxietyEffect = (1.0f - pImpl->anxietyLevel) * 0.15f;
    float rewardEffect = pImpl->rewardProcessingModulation * 0.15f;
    
    float totalEffect = moodEffect + impulsivityEffect + socialEffect + 
                       anxietyEffect + rewardEffect;
    
    float optimalLevel = 0.5f;
    float deviationFromOptimal = std::abs(pImpl->level - optimalLevel);
    float nonLinearFactor = 1.0f - deviationFromOptimal * 2.0f;
    if (nonLinearFactor < 0.5f) nonLinearFactor = 0.5f;
    
    float plasticityFactor = 1.0f + totalEffect * nonLinearFactor;
    
    return std::max(0.5f, std::min(2.0f, plasticityFactor));
}

void Serotonin::update(TimestepDuration dt) {
    float timeConstant = 1.0f;
    
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, 
                              pImpl->level - (pImpl->level - pImpl->baseline) * 
                              static_cast<float>(dt) / timeConstant);
    } else {
        pImpl->level = std::min(pImpl->targetLevel,
                              pImpl->level + (pImpl->targetLevel - pImpl->level) * 
                              static_cast<float>(dt) / (timeConstant * 1.2f));
    }
}

float Serotonin::getBaselineLevel() const {
    return pImpl->baseline;
}

void Serotonin::setTargetLevel(float target) {
    pImpl->targetLevel = std::clamp(target, 0.0f, 1.0f);
}

bool Serotonin::isActive() const {
    return pImpl->level > pImpl->baseline + 0.008f;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->targetLevel = pImpl->baseline;
    updateDerivedFunctions();
}

void Serotonin::updateDerivedFunctions() {
    pImpl->moodLevel = std::clamp(pImpl->level * 2.0f, 0.0f, 1.0f);
    pImpl->impulsivityLevel = 1.0f - std::max(0.0f, pImpl->level - 0.5f) * 2.0f;
    pImpl->socialBehaviorModulation = std::min(1.0f, pImpl->level * 2.5f);
    pImpl->anxietyLevel = std::max(0.0f, 0.5f - pImpl->level * 2.0f);
    pImpl->rewardProcessingModulation = std::min(1.0f, pImpl->level * 2.0f);
    pImpl->seroSleepModulation = std::clamp(pImpl->level * 2.0f - 0.5f, 0.0f, 1.0f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

void Serotonin::setMoodLevel(float mood) {
    pImpl->moodLevel = std::clamp(mood, 0.0f, 1.0f);
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

void Serotonin::setImpulsivityLevel(float level) {
    pImpl->impulsivityLevel = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getSocialBehaviorModulation() const {
    return pImpl->socialBehaviorModulation;
}

void Serotonin::setSocialBehaviorModulation(float modulation) {
    pImpl->socialBehaviorModulation = std::clamp(modulation, 0.0f, 1.0f);
}

void Serotonin::setRapheNucleusActivity(float activity) {
    pImpl->rapheNucleusActivity = std::clamp(activity, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + activity * 0.3f);
}

float Serotonin::getRapheNucleusActivity() const {
    return pImpl->rapheNucleusActivity;
}

float Serotonin::getAnxietyLevel() const {
    return pImpl->anxietyLevel;
}

void Serotonin::setAnxietyLevel(float level) {
    pImpl->anxietyLevel = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::processReward(float rewardValue) {
    pImpl->rewardProcessingModulation = std::min(1.0f, 
        pImpl->rewardProcessingModulation + rewardValue * 0.1f);
    
    float rewardResponse = std::min(0.3f, rewardValue * 0.2f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardResponse);
    
    NLM_LOG_INFO("Serotonin reward processing: modulation " + std::to_string(pImpl->rewardProcessingModulation));
}

float Serotonin::getRewardProcessingModulation() const {
    return pImpl->rewardProcessingModulation;
}

void Serotonin::setSeroSleepModulation(float modulation) {
    pImpl->seroSleepModulation = std::clamp(modulation, 0.0f, 1.0f);
}

float Serotonin::getSeroSleepModulation() const {
    return pImpl->seroSleepModulation;
}

} // namespace nlm
