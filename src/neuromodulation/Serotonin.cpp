#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodScore;
    float behavioralFlexibility;
    float socialConduct;
    float punishmentSensitivity;
    float rewardProcessing;
    
    // Impulse control components
    float impulseControl;
    float patience;
    float frustrationTolerance;
    
    // Sexual/aggression regulation
    float sexualDrive;
    float aggressionLevel;
    
    // Sleep/wake cycle integration
    float sleepPressure;
    float circadianPhase;
    
    Impl() : level(0.0f), baseline(0.03f), peak(0.7f), decayRate(0.04f), releaseRate(1.8f),
             moodScore(0.5f), behavioralFlexibility(1.0f), socialConduct(1.0f),
             punishmentSensitivity(1.0f), rewardProcessing(1.0f),
             impulseControl(1.0f), patience(1.0f), frustrationTolerance(1.0f),
             sexualDrive(1.0f), aggressionLevel(1.0f), sleepPressure(0.0f),
             circadianPhase(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    // Add realistic fluctuations and feedback loops
    float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.015f;
    float feedback = -pImpl->punishmentSensitivity * 0.05f;  // Negative feedback from punishment
    return pImpl->level + noise + feedback;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update mood and behavioral states based on level
    updateBehavioralState();
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity through punishment prediction error
    // Based on reinforcement learning theories
    
    // Validate state before calculating
    if (std::isnan(pImpl->punishmentSensitivity) || std::isinf(pImpl->punishmentSensitivity) ||
        std::isnan(pImpl->rewardProcessing) || std::isinf(pImpl->rewardProcessing)) {
        NLM_LOG_WARNING("Invalid serotonin state for plasticity calculation");
        return 1.0f;  // Safe default
    }
    
    float punishmentError = pImpl->punishmentSensitivity - 1.0f;  // Negative for punishment
    float rewardError = pImpl->rewardProcessing - 1.0f;  // Positive for reward
    
    // Serotonin enhances plasticity for punishment-paired stimuli
    // Reduces plasticity for reward-paired stimuli when too high
    float plasticityFactor = 1.0f + punishmentError * 0.3f - rewardError * 0.2f;
    
    // Ensure reasonable bounds
    if (std::isnan(plasticityFactor) || std::isinf(plasticityFactor)) {
        NLM_LOG_ERROR("Invalid serotonin plasticity calculation");
        return 1.0f;
    }
    
    return std::clamp(plasticityFactor, 0.3f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics based on behavioral feedback and circadian rhythms
    
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Sleep/wake cycle modulation
    updateCircadianModulation(dt);
    
    // Behavioral feedback integration
    if (pImpl->punishmentSensitivity > 1.2f) {
        // High punishment sensitivity: drives level up
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f * static_cast<float>(dt));
    }
    
    if (pImpl->rewardProcessing < 0.8f) {
        // Low reward processing: reduces level (depression-like state)
        pImpl->level *= 0.98f;
    }
    
    // Impulse control feedback
    updateImpulseControl(dt);
}

void Serotonin::signalReward(float reward, float prediction) {
    // Reward signaling based on reward prediction error
    // Serotonin responds to both reward and punishment prediction errors
    
    float rpe = reward - prediction;
    
    if (rpe > 0.0f) {
        // Positive prediction error: reduce serotonin (disappointment/dysphoria)
        pImpl->level = std::max(pImpl->baseline, pImpl->level - rpe * 0.3f);
    } else {
        // Negative prediction error (punishment): increase serotonin (relief)
        pImpl->level = std::min(pImpl->peak, pImpl->level + std::abs(rpe) * 0.2f);
    }
    
    // Update reward processing efficiency
    pImpl->rewardProcessing = std::min(1.0f, pImpl->rewardProcessing + rpe * 0.1f);
}

void Serotonin::signalPunishment(float punishment, float prediction) {
    // Punishment signaling based on punishment prediction error
    // Strong serotonin response to unexpected punishment
    
    float ppe = punishment - prediction;
    
    if (ppe > 0.0f) {
        // Unexpected punishment: strong serotonin increase
        pImpl->level = std::min(pImpl->peak, pImpl->level + ppe * 0.5f);
    } else {
        // Expected punishment: smaller serotonin increase (relief)
        pImpl->level = std::min(pImpl->peak, pImpl->level + std::abs(ppe) * 0.2f);
    }
    
    // Update punishment sensitivity
    pImpl->punishmentSensitivity = std::min(1.5f, pImpl->punishmentSensitivity + ppe * 0.05f);
}

void Serotonin::signalSocialEvent(float socialValue, bool positive, bool novel) {
    // Social behavior modulation based on social feedback
    // Serotonin involved in social hierarchies and status
    
    float socialResponse = socialValue * (positive ? 0.3f : -0.2f);
    
    if (novel) {
        // Novel social interactions get stronger response
        socialResponse *= 1.5f;
    }
    
    // Update social conduct and mood
    pImpl->socialConduct += socialResponse * 0.1f;
    pImpl->moodScore += socialResponse * 0.15f;
    
    // Update level
    pImpl->level = std::clamp(pImpl->level + socialResponse * pImpl->releaseRate * 0.1f,
                              pImpl->baseline, pImpl->peak);
}

void Serotonin::updateCircadianModulation(TimestepDuration dt) {
    // Serotonin follows circadian rhythms
    // Highest during day, lower at night
    
    pImpl->circadianPhase += dt * 0.001f;  // Time passes
    
    // Sinusoidal modulation (24-hour cycle)
    float circadianMod = (std::sin(pImpl->circadianPhase * 2.0f * M_PI / 24000.0f) + 1.0f) * 0.5f;
    
    // Apply circadian modulation
    float targetLevel = pImpl->level * circadianMod;
    pImpl->level = pImpl->level * 0.9f + targetLevel * 0.1f;  // Smooth transition
}

void Serotonin::updateImpulseControl(TimestepDuration dt) {
    // Update impulse control based on serotonin level
    // Low serotonin = poor impulse control
    
    if (pImpl->level < 0.15f) {
        // Low serotonin state: impulse control deteriorates
        pImpl->impulseControl = std::max(0.3f, pImpl->impulseControl - 0.02f * static_cast<float>(dt));
        pImpl->patience *= 0.95f;
        pImpl->frustrationTolerance *= 0.9f;
    } else {
        // Normal/high serotonin: impulse control improves
        pImpl->impulseControl = std::min(1.0f, pImpl->impulseControl + 0.01f * static_cast<float>(dt));
        if (pImpl->level > 0.3f) {
            pImpl->patience *= 1.005f;
            pImpl->frustrationTolerance *= 1.005f;
        }
    }
}

void Serotonin::updateBehavioralState() {
    // Update mood and behavioral states based on current level
    
    if (pImpl->level < 0.15f) {
        // Low serotonin state: depression-like symptoms
        pImpl->moodScore = std::max(0.1f, pImpl->moodScore - 0.05f);
        pImpl->behavioralFlexibility *= 0.9f;
        pImpl->sexualDrive *= 0.9f;
        pImpl->aggressionLevel *= 1.1f;  // Increased aggression
    } else if (pImpl->level > 0.5f) {
        // High serotonin state: elevated mood
        pImpl->moodScore = std::min(1.0f, pImpl->moodScore + 0.03f);
        pImpl->behavioralFlexibility *= 1.05f;
        pImpl->sexualDrive *= 1.05f;
        pImpl->aggressionLevel *= 0.95f;  // Reduced aggression
    } else {
        // Normal serotonin state
        pImpl->moodScore = std::clamp(pImpl->moodScore, 0.4f, 0.6f);
    }
}

float Serotonin::getMoodScore() const {
    return pImpl->moodScore;
}

float Serotonin::getImpulseControl() const {
    return pImpl->impulseControl;
}

float Serotonin::getBehavioralFlexibility() const {
    return pImpl->behavioralFlexibility;
}

bool Serotonin::isDepressionLike() const {
    return pImpl->level < 0.15f && pImpl->moodScore < 0.4f;
}

bool Serotonin::isElevatedMood() const {
    return pImpl->level > 0.5f && pImpl->moodScore > 0.7f;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodScore = 0.5f;
    pImpl->behavioralFlexibility = 1.0f;
    pImpl->socialConduct = 1.0f;
    pImpl->punishmentSensitivity = 1.0f;
    pImpl->rewardProcessing = 1.0f;
    pImpl->impulseControl = 1.0f;
    pImpl->patience = 1.0f;
    pImpl->frustrationTolerance = 1.0f;
    pImpl->sexualDrive = 1.0f;
    pImpl->aggressionLevel = 1.0f;
    pImpl->sleepPressure = 0.0f;
    pImpl->circadianPhase = 0.0f;
    
    // Validate reset state
    if (std::isnan(pImpl->level) || std::isinf(pImpl->level) ||
        std::isnan(pImpl->moodScore) || std::isinf(pImpl->moodScore) ||
        std::isnan(pImpl->behavioralFlexibility) || std::isinf(pImpl->behavioralFlexibility)) {
        NLM_LOG_ERROR("Invalid state after serotonin reset");
        pImpl->level = pImpl->baseline;  // Force recovery
        pImpl->moodScore = 0.5f;
        pImpl->behavioralFlexibility = 1.0f;
    }
}

} // namespace nlm