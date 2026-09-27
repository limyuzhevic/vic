#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

namespace nlm {

// Base Neuromodulator implementation

class Neuromodulator::ImplBase {
public:
    float level;
    float baseline;
    float maxLevel;
    float decayRate;
    float releaseRate;
    
    ImplBase() : level(0.0f), baseline(0.0f), maxLevel(1.0f), 
                 decayRate(0.1f), releaseRate(1.0f) {}
};

// Abstract base class default implementations

void Neuromodulator::reset() {}

float Neuromodulator::getExcitabilityModifier() const {
    return 1.0f + level;
}

float Neuromodulator::getLearningRateModifier() const {
    return 1.0f;
}

float Neuromodulator::getMemoryConsolidationStrength() const {
    return 1.0f;
}

// Dopamine: Reward and reinforcement learning signal

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionErrorGain;
    float rewardSensitivity;
    float noveltyWeight;
    float historySize;
    std::vector<float> rewardHistory;
    std::vector<float> predictionErrorHistory;
    
    Impl() : level(0.0f), baseline(0.01f), peak(2.0f), 
             decayRate(0.05f), releaseRate(0.3f),
             predictionErrorGain(1.0f), rewardSensitivity(1.0f),
             noveltyWeight(0.2f), historySize(100) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine-modulated plasticity factor based on concentration
    // Higher dopamine enhances learning, but inverted-U relationship
    float normalizedLevel = std::min(1.0f, pImpl->level / pImpl->peak);
    float modulation = 1.0f - std::abs(0.5f - normalizedLevel) * 2.0f;
    return 1.0f + modulation * 0.5f;  // 1.0x to 1.5x plasticity enhancement
}

float Dopamine::getExcitabilityModifier() const {
    // Dopamine increases neural excitability
    return 1.0f + pImpl->level * 0.5f;
}

float Dopamine::getLearningRateModifier() const {
    // Optimized learning rate modulation
    float normalizedLevel = std::min(1.0f, pImpl->level / pImpl->peak);
    return 0.5f + normalizedLevel * 1.5f;  // 0.5x to 2.0x learning rate
}

float Dopamine::getMemoryConsolidationStrength() const {
    // Dopamine enhances memory consolidation
    return 1.0f + pImpl->level * 0.3f;
}

void Dopamine::update(TimestepDuration dt) {
    // Biologically realistic dopamine dynamics: exponential decay to baseline
    if (dt > 0.0) {
        float decayAmount = pImpl->decayRate * static_cast<float>(dt);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    }
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->rewardHistory.clear();
    pImpl->predictionErrorHistory.clear();
}

void Dopamine::signalReward(float reward) {
    if (reward <= 0.0f) return;
    
    // Phasic dopamine release: burst proportional to reward magnitude
    float burstSize = reward * pImpl->rewardSensitivity * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstSize);
    
    // Maintain reward history for prediction error computation
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > pImpl->historySize) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    if (std::abs(error) < 1e-6f) return;
    
    // Reward prediction error signaling: positive error -> increase dopamine
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->predictionErrorGain);
    
    // Maintain prediction error history
    pImpl->predictionErrorHistory.push_back(error);
    if (pImpl->predictionErrorHistory.size() > pImpl->historySize) {
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
}

void Dopamine::integrateNovelty(float novelty) {
    // Dopamine responds to novelty as well (prediction error for unexpected events)
    if (novelty > 0.0f) {
        float noveltyContribution = novelty * pImpl->noveltyWeight;
        pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyContribution);
    }
}

float Dopamine::getAveragePredictionError() const {
    if (pImpl->predictionErrorHistory.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float error : pImpl->predictionErrorHistory) {
        sum += error;
    }
    return sum / static_cast<float>(pImpl->predictionErrorHistory.size());
}

const std::vector<float>& Dopamine::getRewardHistory() const {
    return pImpl->rewardHistory;
}

const std::vector<float>& Dopamine::getPredictionErrorHistory() const {
    return pImpl->predictionErrorHistory;
}

// Acetylcholine: Attention and memory consolidation

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryGain;
    float arousalCoupling;
    float cholinergicNoise;
    
    Impl() : level(0.0f), baseline(0.02f), peak(1.5f),
             decayRate(0.03f), releaseRate(0.4f),
             attentionGain(2.0f), memoryGain(0.5f),
             arousalCoupling(0.7f), cholinergicNoise(0.01f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity for memory encoding
    float attentionEffect = std::min(1.0f, pImpl->level / pImpl->peak);
    return 1.0f + attentionEffect * 0.3f;  // 1.0x to 1.3x enhancement
}

float Acetylcholine::getExcitabilityModifier() const {
    // ACh enhances cortical excitability and attention
    return 1.0f + pImpl->level * 0.8f;
}

float Acetylcholine::getLearningRateModifier() const {
    // ACh boosts learning for attended stimuli
    float attentionEffect = std::min(1.0f, pImpl->level / pImpl->peak);
    return 1.0f + attentionEffect * 2.0f;  // 1.0x to 3.0x learning rate
}

float Acetylcholine::getMemoryConsolidationStrength() const {
    // ACh is crucial for memory consolidation
    float memoryEffect = std::min(1.0f, pImpl->level / pImpl->peak);
    return 1.0f + memoryEffect * 1.5f;  // 1.0x to 2.5x consolidation strength
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: slower decay, tied to arousal state
    if (dt > 0.0) {
        float decayAmount = pImpl->decayRate * static_cast<float>(dt);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    }
    
    // Add small stochastic fluctuations
    pImpl->level += (std::rand() % 100 / 100.0f - 0.5f) * pImpl->cholinergicNoise;
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Focused attention triggers acetylcholine release
    if (attentionStrength > 0.0f) {
        float attentionBurst = attentionStrength * pImpl->attentionGain;
        pImpl->level = std::min(pImpl->peak, pImpl->level + attentionBurst);
    }
}

void Acetylcholine::signalMemoryDemand(float memoryLoad) {
    // Memory processing requirements increase ACh
    if (memoryLoad > 0.0f) {
        float memoryBurst = memoryLoad * pImpl->memoryGain;
        pImpl->level = std::min(pImpl->peak, pImpl->level + memoryBurst);
    }
}

void Acetylcholine::coupleWithArousal(float arousalLevel) {
    // ACh couples with noradrenergic arousal system
    if (arousalLevel > 0.0f) {
        float couplingEffect = arousalLevel * pImpl->arousalCoupling;
        pImpl->level = std::min(pImpl->peak, pImpl->level + couplingEffect);
    }
}

// Norepinephrine: Arousal and vigilance

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float vigilanceGain;
    float alertnessGain;
    float stressResponse;
    float locusCoeruleusInput;
    float adaptiveGain;
    
    Impl() : level(0.0f), baseline(0.015f), peak(2.0f),
             decayRate(0.04f), releaseRate(0.5f),
             vigilanceGain(1.5f), alertnessGain(1.0f),
             stressResponse(1.0f), locusCoeruleusInput(1.0f),
             adaptiveGain(0.8f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for arousal-dependent learning
    float vigilanceEffect = std::min(1.0f, pImpl->level / pImpl->peak);
    return 1.0f + vigilanceEffect * 0.4f;  // 1.0x to 1.4x enhancement
}

float Norepinephrine::getExcitabilityModifier() const {
    // NE increases neural excitability and signal-to-noise ratio
    return 1.0f + pImpl->level * 0.6f;
}

float Norepinephrine::getLearningRateModifier() const {
    // NE enhances learning for salient/novel events
    float alertnessEffect = std::min(1.0f, pImpl->level / pImpl->peak);
    return 1.0f + alertnessEffect * 1.2f;  // 1.0x to 2.2x learning rate
}

float Norepinephrine::getMemoryConsolidationStrength() const {
    // NE modulates memory strength based on arousal
    float stressEffect = std::min(1.0f, pImpl->level / pImpl->peak);
    return 1.0f + stressEffect * 1.2f;  // 1.0x to 2.2x consolidation strength
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: phasic and tonic components
    if (dt > 0.0) {
        float decayAmount = pImpl->decayRate * static_cast<float>(dt);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    }
    
    // Adaptive gain control based on information load
    pImpl->level *= (1.0f - pImpl->adaptiveGain * static_cast<float>(dt));
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
}

void Norepinephrine::signalVigilance(float vigilanceLevel) {
    // High vigilance triggers locus coeruleus firing
    if (vigilanceLevel > 0.0f) {
        float vigilanceBurst = vigilanceLevel * pImpl->vigilanceGain;
        pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceBurst);
    }
}

void Norepinephrine::signalAlertness(float alertness) {
    // General alertness increases NE
    if (alertness > 0.0f) {
        float alertnessBurst = alertness * pImpl->alertnessGain;
        pImpl->level = std::min(pImpl->peak, pImpl->level + alertnessBurst);
    }
}

void Norepinephrine::signalStressResponse(float stressLevel) {
    // Stress hormones amplify NE release
    if (stressLevel > 0.0f) {
        float stressAmplification = stressLevel * pImpl->stressResponse;
        pImpl->level = std::min(pImpl->peak, pImpl->level * stressAmplification);
    }
}

void Norepinephrine::adaptToInputLoad(float inputLoad) {
    // LC-NE system adapts gain based on input statistics
    if (inputLoad > 0.0f) {
        float adaptation = inputLoad * pImpl->adaptiveGain;
        pImpl->level *= (1.0f + adaptation);
    }
}

// Serotonin: Mood, impulsivity, and social behavior

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodGain;
    float impulsivityGain;
    float socialBondingGain;
    float rapheInput;
    float homeostaticSetpoint;
    float satietyLevel;
    
    Impl() : level(0.0f), baseline(0.005f), peak(1.0f),
             decayRate(0.06f), releaseRate(0.25f),
             moodGain(1.0f), impulsivityGain(1.5f),
             socialBondingGain(0.8f), rapheInput(1.0f),
             homeostaticSetpoint(0.5f), satietyLevel(0.5f) {}
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
    // Serotonin modulates plasticity for mood-dependent learning
    float moodEffect = std::min(1.0f, pImpl->level / pImpl->peak);
    return 1.0f - moodEffect * 0.2f;  // 0.8x to 1.0x modulation
}

float Serotonin::getExcitabilityModifier() const {
    // Serotonin modulates neural excitability - complex inverted-U
    float normalizedLevel = std::min(1.0f, pImpl->level / pImpl->peak);
    // Inverted-U: optimal at intermediate levels
    return 1.0f + 0.3f * (1.0f - std::abs(0.5f - normalizedLevel) * 2.0f);
}

float Serotonin::getLearningRateModifier() const {
    // Serotonin modulates learning rate based on mood and satiety
    float moodEffect = std::min(1.0f, pImpl->level / pImpl->peak);
    float satietyEffect = 1.0f - std::abs(pImpl->satietyLevel - 0.5f) * 2.0f;
    return 1.0f - moodEffect * 0.3f + satietyEffect * 0.2f;  // Modulated learning
}

float Serotonin::getMemoryConsolidationStrength() const {
    // Serotonin enhances consolidation of negative memories, suppresses positive
    float normalizedLevel = std::min(1.0f, pImpl->level / pImpl->peak);
    return 1.0f + normalizedLevel * 0.2f;  // 1.0x to 1.2x consolidation
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: homeostatic regulation
    if (dt > 0.0) {
        float decayAmount = pImpl->decayRate * static_cast<float>(dt);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    }
    
    // Homeostatic adjustment towards setpoint
    float homeostaticError = pImpl->homeostaticSetpoint - pImpl->level;
    if (std::abs(homeostaticError) > 0.01f) {
        pImpl->level += homeostaticError * 0.01f * static_cast<float>(dt);
    }
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->satietyLevel = 0.5f;
}

void Serotonin::signalMoodChange(float moodChange) {
    // Mood fluctuations affect serotonin release
    if (moodChange > 0.0f) {
        float moodBurst = moodChange * pImpl->moodGain;
        pImpl->level = std::min(pImpl->peak, pImpl->level + moodBurst);
    }
}

void Serotonin::signalImpulsivity(float impulsivityLevel) {
    // High impulsivity increases serotonin turnover
    if (impulsivityLevel > 0.0f) {
        float impulsivityBurst = impulsivityLevel * pImpl->impulsivityGain;
        pImpl->level = std::min(pImpl->peak, pImpl->level + impulsivityBurst);
    }
}

void Serotonin::signalSocialBonding(float bondingLevel) {
    // Social bonding and affiliation increase serotonin
    if (bondingLevel > 0.0f) {
        float bondingBurst = bondingLevel * pImpl->socialBondingGain;
        pImpl->level = std::min(pImpl->peak, pImpl->level + bondingBurst);
    }
}

void Serotonin::adjustHomeostasis(float energyLevel, float nutrients) {
    // Serotonergic homeostasis responds to metabolic state
    float energyEffect = energyLevel * 0.5f + nutrients * 0.5f;
    pImpl->homeostaticSetpoint = energyEffect;
    
    // Update satiety
    pImpl->satietyLevel = std::min(1.0f, pImpl->satietyLevel + energyLevel * 0.1f);
}

float Serotonin::getSatietyLevel() const {
    return pImpl->satietyLevel;
}

void Serotonin::incrementSatiety(float amount) {
    pImpl->satietyLevel = std::min(1.0f, pImpl->satietyLevel + amount);
}

} // namespace nlm