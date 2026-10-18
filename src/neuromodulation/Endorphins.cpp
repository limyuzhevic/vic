#include "Endorphins.hpp"

namespace nlm {

struct Endorphins::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Pain modulation
    float painLevel;
    float analgesicEffect;
    std::deque<float> painHistory;
    size_t maxPainHistorySize;
    
    // Stress adaptation
    float stressLevel;
    float stressRecovery;
    float stressAdaptationRate;
    
    // Social bonding and reward balance
    float socialBondingStrength;
    float rewardValueModulation;
    std::unordered_map<uint64_t, float> socialBondingTags; // EntityId -> bonding strength
    
    // Exercise-induced analgesia
    float exerciseDuration;
    float exerciseIntensity;
    float exerciseAnalgesia;
    bool exerciseActive;
    
    // Opioid receptor dynamics
    float receptorOccupancy;
    float receptorAffinity;
    float receptorInternalizationRate;
    
    // Homeostatic balance
    float homeostaticTarget;
    float homeostaticError;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , painLevel(0.0f), analgesicEffect(1.0f)
        , stressLevel(0.0f), stressRecovery(1.0f), stressAdaptationRate(0.05f)
        , socialBondingStrength(1.0f), rewardValueModulation(1.0f)
        , exerciseDuration(0.0f), exerciseIntensity(0.0f), exerciseAnalgesia(0.0f), exerciseActive(false)
        , receptorOccupancy(0.0f), receptorAffinity(1.0f), receptorInternalizationRate(0.01f)
        , homeostaticTarget(1.0f), homeostaticError(0.0f)
        , maxPainHistorySize(50) {}
};

Endorphins::Endorphins() : pImpl(new Impl) {}

Endorphins::~Endorphins() = default;

const char* Endorphins::getName() const {
    return "Endorphins";
}

float Endorphins::getLevel() const {
    return pImpl->level;
}

void Endorphins::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Endorphins::getPlasticityFactor() const {
    // Endorphins modulate plasticity through stress-reward balance
    float stressModulation = 1.0f - pImpl->stressLevel * 0.5f; // High stress -> reduced plasticity
    float rewardModulation = 1.0f + pImpl->rewardValueModulation * 0.3f; // Reward -> enhanced plasticity
    return 0.5f * stressModulation * rewardModulation;
}

void Endorphins::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Receptor dynamics
    pImpl->receptorOccupancy = std::max(0.0f, pImpl->receptorOccupancy - pImpl->receptorInternalizationRate * static_cast<float>(dt));
    
    // Clean up pain history
    while (pImpl->painHistory.size() > pImpl->maxPainHistorySize) {
        pImpl->painHistory.pop_front();
    }
}

void Endorphins::reducePain(float painLevel) {
    pImpl->painLevel = std::clamp(painLevel, 0.0f, 1.0f);
    // Analgesic effect increases with endorphin level
    pImpl->analgesicEffect = 1.0f - pImpl->level * 0.9f;
}

void Endorphins::blockPainSignals(float blockStrength) {
    // Block pain signal transmission
    pImpl->analgesicEffect = std::min(pImpl->analgesicEffect, 1.0f - blockStrength);
}

void Endorphins::signalStress(float stressLevel) {
    pImpl->stressLevel = std::clamp(stressLevel, 0.0f, 1.0f);
    // Stress triggers endorphin release as adaptation mechanism
    if (stressLevel > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (stressLevel - 0.5f) * pImpl->releaseRate * 2.0f);
    }
}

void Endorphins::facilitateStressRecovery() {
    // Enhance recovery from stress
    pImpl->stressRecovery *= 1.1f;
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - 0.2f);
}

void Endorphins::facilitateSocialBonding(float socialReward) {
    pImpl->socialBondingStrength = std::clamp(socialReward, 0.0f, 1.0f);
}

void Endorphins::modulateRewardValue(float rewardValue) {
    pImpl->rewardValueModulation = std::clamp(rewardValue, 0.5f, 2.0f);
}

void Endorphins::registerExerciseActivity(float duration, float intensity) {
    pImpl->exerciseDuration = duration;
    pImpl->exerciseIntensity = std::clamp(intensity, 0.0f, 1.0f);
    pImpl->exerciseAnalgesia = duration * intensity * 0.5f;
    pImpl->exerciseActive = true;
    
    // Exercise-induced analgesia
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->exerciseAnalgesia * pImpl->releaseRate);
}

void Endorphins::releaseEndorphins(float magnitude) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + magnitude * pImpl->releaseRate);
    pImpl->receptorOccupancy = std::min(1.0f, pImpl->receptorOccupancy + magnitude * 0.5f);
}

void Endorphins::bindToReceptors(float receptorAffinity) {
    pImpl->receptorAffinity = std::clamp(receptorAffinity, 0.0f, 1.0f);
    pImpl->receptorOccupancy = std::min(1.0f, pImpl->receptorOccupancy + receptorAffinity * 0.1f);
}

void Endorphins::restoreHomeostaticBalance() {
    // Restore homeostatic balance based on current state
    pImpl->homeostaticError = std::abs(pImpl->homeostaticTarget - pImpl->level);
    if (pImpl->homeostaticError > 0.1f) {
        pImpl->level = pImpl->homeostaticTarget;
    }
}

} // namespace nlm
