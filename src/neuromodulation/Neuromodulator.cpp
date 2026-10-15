// Neuromodulator Implementation
// This file provides comprehensive neuromodulation implementations
// with real biological mechanisms and interactions

#include "Neuromodulator.hpp"
#include "Dopamine.hpp"
#include "Reward.hpp"
#include "PredictionError.hpp"
#include "Novelty.hpp"
#include "Curiosity.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>
#include <string>

namespace nlm {

// Neuromodulator base implementation
struct Neuromodulator::Impl {
    std::string name;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationFactor;
    float learningRate;
    
    // State history for analysis
    std::vector<float> levelHistory;
    std::vector<float> externalInputHistory;
    std::vector<float> plasticityHistory;
    
    // Integration with other systems
    bool isIntegratedWithPlasticity;
    bool isIntegratedWithAttention;
    bool isIntegratedWithMemory;
    
    // Timing and dynamics
    SimulationStep lastUpdateStep;
    float phaseShift;  // For oscillatory dynamics
    float coherence;   // With other neuromodulators
    
    Impl() : name("Unknown"), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.1f), releaseRate(1.0f), adaptationFactor(1.0f),
             learningRate(0.01f), isIntegratedWithPlasticity(false),
             isIntegratedWithAttention(false), isIntegratedWithMemory(false),
             lastUpdateStep(0), phaseShift(0.0f), coherence(1.0f) {}
};

Neuromodulator::Neuromodulator() : pImpl(new Impl) {
    NLM_LOG_INFO("Neuromodulator base class initialized");
}

Neuromodulator::~Neuromodulator() = default;

const char* Neuromodulator::getName() const {
    return pImpl->name.c_str();
}

float Neuromodulator::getLevel() const {
    return pImpl->level;
}

void Neuromodulator::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Neuromodulator::update(TimestepDuration dt) {
    // Base neuromodulator update with homeostatic regulation
    // Apply adaptation towards baseline
    pImpl->level += (pImpl->baseline - pImpl->level) * pImpl->adaptationFactor * 0.01f * static_cast<float>(dt);
    
    // Apply decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update history
    pImpl->levelHistory.push_back(pImpl->level);
    if (pImpl->levelHistory.size() > 1000) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
    }
}

// Dopamine implementation
struct Dopamine::Impl : Neuromodulator::Impl {
    // Reward prediction error components
    float predictionError;
    float expectedReward;
    float rewardHistoryFactor;
    
    // Dopamine dynamics parameters
    float phasicBurstRate;
    float tonicBaseline;
    float burstAmplitude;
    float forgettingTimeConstant;
    
    // Reinforcement learning components
    float eligibilityTrace;
    float rewardAttentionGain;
    float predictionErrorGain;
    
    // Neural population effects
    float firingRateModulation;
    float synapticPlasticity;
    float neuronalExcitability;
    
    // Temporal dynamics
    float phasicComponent;
    float tonicComponent;
    
    Impl() : Neuromodulator::Impl(), predictionError(0.0f), expectedReward(0.0f),
             rewardHistoryFactor(0.5f), phasicBurstRate(0.1f), tonicBaseline(0.01f),
             burstAmplitude(1.0f), forgettingTimeConstant(1.0f),
             eligibilityTrace(0.0f), rewardAttentionGain(1.0f),
             predictionErrorGain(0.5f), firingRateModulation(1.0f),
             synapticPlasticity(1.0f), neuronalExcitability(1.0f),
             phasicComponent(0.0f), tonicComponent(0.0f) {
        pImpl->name = "DA";
        pImpl->isIntegratedWithPlasticity = true;
        pImpl->isIntegratedWithAttention = true;
        pImpl->isIntegratedWithMemory = true;
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->phasicComponent + pImpl->tonicComponent;
}

void Dopamine::setLevel(float level) {
    // Split between phasic and tonic components based on level
    if (level > 0.5f) {
        pImpl->phasicComponent = std::min(pImpl->burstAmplitude, level);
        pImpl->tonicComponent = level - pImpl->phasicComponent;
    } else {
        pImpl->phasicComponent = 0.0f;
        pImpl->tonicComponent = level;
    }
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Positive dopamine increases LTP, negative decreases it
    // The relationship is nonlinear with saturation
    
    float daLevel = getLevel();
    
    // Phasic dopamine strongly affects plasticity
    float phasicEffect = pImpl->phasicComponent * 2.0f;
    
    // Tonic dopamine provides baseline modulation
    float tonicEffect = pImpl->tonicComponent * 0.5f;
    
    // Combined effect with saturation
    float plasticityMod = 1.0f + phasicEffect + tonicEffect;
    
    // Non-linear response with inverted U-curve for phasic DA
    float phasicNonlinear = 1.0f - std::pow(std::abs(pImpl->phasicComponent - 0.5f), 2.0f) * 2.0f;
    
    float totalFactor = plasticityMod * phasicNonlinear;
    
    // Clamp to reasonable bounds
    return std::clamp(totalFactor, 0.1f, 5.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with two components
    
    // Tonic component: slow adaptation towards baseline
    float tonicTarget = pImpl->baseline + pImpl->tonicBaseline * getLevel();
    pImpl->tonicComponent += (tonicTarget - pImpl->tonicComponent) * 0.01f * static_cast<float>(dt);
    
    // Phasic component: responds to rewards and prediction errors
    float predictionErrorSignal = pImpl->predictionError * pImpl->predictionErrorGain;
    float rewardSignal = pImpl->expectedReward * pImpl->rewardHistoryFactor;
    
    // Integrate signals with temporal filtering
    pImpl->phasicComponent *= (1.0f - 0.5f * static_cast<float>(dt));  // Decay
    pImpl->phasicComponent += (predictionErrorSignal + rewardSignal) * pImpl->burstAmplitude * 0.1f * static_cast<float>(dt);
    
    // Apply forgetting curve
    pImpl->phasicComponent *= std::exp(-static_cast<float>(dt) / pImpl->forgettingTimeConstant);
    
    // Clamp phasic component
    pImpl->phasicComponent = std::clamp(pImpl->phasicComponent, 0.0f, pImpl->burstAmplitude);
    
    // Update plasticity effects based on dopamine level
    float daLevel = getLevel();
    pImpl->synapticPlasticity = 1.0f + daLevel * 0.5f;
    pImpl->neuronalExcitability = 1.0f + daLevel * 0.3f;
    
    // Update eligibility traces (reward-modulated learning)
    pImpl->eligibilityTrace *= (1.0f - 0.2f * static_cast<float>(dt));  // Decay
    pImpl->eligibilityTrace += pImpl->predictionError * pImpl->rewardAttentionGain * 0.01f * static_cast<float>(dt);
    
    // Update history
    pImpl->levelHistory.push_back(getLevel());
    if (pImpl->levelHistory.size() > 1000) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with temporal dynamics
    
    // Store reward in history for prediction
    pImpl->externalInputHistory.push_back(reward);
    if (pImpl->externalInputHistory.size() > 100) {
        pImpl->externalInputHistory.erase(pImpl->externalInputHistory.begin());
    }
    
    // Compute reward prediction error
    float predictionError = reward - pImpl->expectedReward;
    pImpl->predictionError = predictionError;
    
    // Update expected reward (credit assignment)
    pImpl->expectedReward = pImpl->expectedReward * 0.9f + reward * 0.1f;
    
    // Generate phasic dopamine response
    float burstStrength = std::min(1.0f, std::abs(predictionError) * 2.0f);
    pImpl->phasicComponent = std::min(pImpl->burstAmplitude, burstStrength);
    
    // For negative prediction errors (punishment), reduce response
    if (predictionError < 0.0f) {
        pImpl->phasicComponent *= 0.5f;
    }
    
    NLM_LOG_INFO("Dopamine reward signal: reward=" + std::to_string(reward) +
                 ", prediction error=" + std::to_string(predictionError) +
                 ", phasic response=" + std::to_string(pImpl->phasicComponent));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct prediction error signaling
    pImpl->predictionError = error;
    
    // Generate phasic response proportional to prediction error
    float burstStrength = std::min(1.0f, std::abs(error) * 1.5f);
    pImpl->phasicComponent = std::min(pImpl->burstAmplitude, burstStrength);
    
    // Apply sign of error
    if (error < 0.0f) {
        pImpl->phasicComponent = -pImpl->phasicComponent;
    }
    
    NLM_LOG_INFO("Dopamine prediction error signal: error=" + std::to_string(error) +
                 ", response=" + std::to_string(pImpl->phasicComponent));
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Dopamine::getPeak() const {
    return pImpl->burstAmplitude;
}

void Dopamine::setPeak(float peak) {
    pImpl->burstAmplitude = peak;
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

void Dopamine::setDecayRate(float decayRate) {
    pImpl->decayRate = decayRate;
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

void Dopamine::setReleaseRate(float releaseRate) {
    pImpl->releaseRate = releaseRate;
}

const std::vector<float>& Dopamine::getRecentRewards() const {
    return pImpl->externalInputHistory;
}

size_t Dopamine::getRewardHistorySize() const {
    return pImpl->externalInputHistory.size();
}

void Dopamine::reset() {
    pImpl->phasicComponent = 0.0f;
    pImpl->tonicComponent = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->expectedReward = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->synapticPlasticity = 1.0f;
    pImpl->neuronalExcitability = 1.0f;
}

// Acetylcholine implementation - attention and memory consolidation
struct Acetylcholine::Impl : Neuromodulator::Impl {
    // Attention modulation parameters
    float attentionGain;
    float noveltyGain;
    float memoryConsolidationRate;
    
    // Working memory effects
    float memoryMaintenanceStrength;
    float interferenceReduction;
    
    // Cognitive flexibility
    float explorationBias;
    float attentionalFocus;
    
    Impl() : Neuromodulator::Impl(), attentionGain(1.0f), noveltyGain(0.8f),
             memoryConsolidationRate(0.5f), memoryMaintenanceStrength(1.2f),
             interferenceReduction(0.7f), explorationBias(0.6f),
             attentionalFocus(0.8f) {
        pImpl->name = "ACh";
        pImpl->isIntegratedWithAttention = true;
        pImpl->isIntegratedWithMemory = true;
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level * pImpl->attentionGain;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine modulates plasticity for memory encoding
    // Higher ACh enhances LTP during novel experiences
    float noveltyAmplification = 1.0f + pImpl->noveltyGain * getLevel();
    return pImpl->learningRate * noveltyAmplification;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics related to attention and novelty detection
    
    // Update based on external stimulation
    pImpl->level = std::min(1.0f, pImpl->level + 0.1f * static_cast<float>(dt));
    
    // Decay when attention is not maintained
    pImpl->level *= (1.0f - 0.05f * static_cast<float>(dt));
    
    // Integrate with memory systems
    float memoryModulation = getLevel() * pImpl->memoryConsolidationRate;
    
    // Enhance working memory traces through neuromodulation
    pImpl->level *= (1.0f + memoryModulation * 0.1f);
}

// Norepinephrine implementation - arousal and vigilance
struct Norepinephrine::Impl : Neuromodulator::Impl {
    // Arousal and alertness parameters
    float vigilanceLevel;
    float stressResponse;
    float fightOrFlightGain;
    
    // Cognitive effects
    float attentionalFocus;
    float reactionSpeed;
    float workingMemoryCapacity;
    
    Impl() : Neuromodulator::Impl(), vigilanceLevel(0.5f), stressResponse(0.0f),
             fightOrFlightGain(1.0f), attentionalFocus(0.8f),
             reactionSpeed(1.0f), workingMemoryCapacity(1.0f) {
        pImpl->name = "NE";
        pImpl->isIntegratedWithAttention = true;
        pImpl->isIntegratedWithMemory = true;
    }
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->vigilanceLevel + pImpl->stressResponse;
}

void Norepinephrine::setLevel(float level) {
    pImpl->vigilanceLevel = std::clamp(level, 0.0f, 1.0f);
    // Stress response is separate component
    pImpl->stressResponse = (level > 0.7f) ? (level - 0.7f) * 2.0f : 0.0f;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal and plasticity
    // High arousal enhances plasticity for salient experiences
    float vigilanceEffect = pImpl->vigilanceLevel * 0.5f;
    float stressEffect = pImpl->stressResponse * 0.3f;
    
    return 1.0f + vigilanceEffect + stressEffect;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics based on environmental demands and stress
    
    // Vigilance increases with novel or challenging stimuli
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + 0.05f * static_cast<float>(dt));
    
    // Stress response builds up with sustained challenge
    pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + 0.02f * static_cast<float>(dt));
    
    // Recovery from stress
    if (pImpl->stressResponse > 0.0f) {
        pImpl->stressResponse *= (1.0f - 0.1f * static_cast<float>(dt));
    }
    
    // Fight-or-flight response
    if (pImpl->vigilanceLevel > 0.8f) {
        pImpl->level += pImpl->fightOrFlightGain * 0.01f * static_cast<float>(dt);
    }
}

// Serotonin implementation - mood, impulsivity, and social behavior
struct Serotonin::Impl : Neuromodulator::Impl {
    // Mood and affect regulation
    float moodLevel;
    float inhibitionLevel;
    float rewardSensitivity;
    
    // Social behavior effects
    float socialAffiliation;
    float socialDominance;
    float anxietyLevel;
    
    // Behavioral state
    float impulsivity;
    float patience;
    float aggression;
    
    Impl() : Neuromodulator::Impl(), moodLevel(0.5f), inhibitionLevel(0.5f),
             rewardSensitivity(1.0f), socialAffiliation(0.5f),
             socialDominance(0.5f), anxietyLevel(0.3f), impulsivity(0.5f),
             patience(0.5f), aggression(0.3f) {
        pImpl->name = "5-HT";
        pImpl->isIntegratedWithAttention = true;
        pImpl->isIntegratedWithMemory = true;
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->moodLevel;
}

void Serotonin::setLevel(float level) {
    pImpl->moodLevel = std::clamp(level, 0.0f, 1.0f);
    
    // Update related components based on mood
    pImpl->inhibitionLevel = 1.0f - pImpl->moodLevel * 0.3f;
    pImpl->rewardSensitivity = 1.0f + pImpl->moodLevel * 0.2f;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood and behavioral flexibility
    // High serotonin promotes behavioral inhibition and stability
    float moodEffect = pImpl->moodLevel * 0.3f;
    float inhibitionEffect = pImpl->inhibitionLevel * 0.4f;
    
    return 1.0f - moodEffect + inhibitionEffect;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics related to mood regulation
    
    // Natural oscillations in serotonin levels
    pImpl->moodLevel += 0.01f * static_cast<float>(dt) * std::sin(pImpl->phaseShift);
    
    // Homeostatic regulation
    float targetMood = 0.5f;  // Optimal mood level
    pImpl->moodLevel += (targetMood - pImpl->moodLevel) * 0.001f * static_cast<float>(dt);
    
    // Clamp to reasonable bounds
    pImpl->moodLevel = std::clamp(pImpl->moodLevel, 0.0f, 1.0f);
}

// Novelty implementation
Novelty::Novelty() : pImpl(new Impl) {
    pImpl->name = "Novelty";
    pImpl->isIntegratedWithAttention = true;
}

Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "Novelty";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Novelty::getPlasticityFactor() const {
    // Novelty drives exploration and plasticity
    return pImpl->level * 2.0f;
}

void Novelty::update(TimestepDuration dt) {
    // Novelty detection based on change in environment
    // Simplified: decay towards baseline
    pImpl->level *= (1.0f - 0.1f * static_cast<float>(dt));
}

// Curiosity implementation
Curiosity::Curiosity() : pImpl(new Impl) {
    pImpl->name = "Curiosity";
    pImpl->isIntegratedWithAttention = true;
    pImpl->isIntegratedWithMemory = true;
}

Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity drives exploration and learning
    return 1.0f + pImpl->level * 1.5f;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update with time constant
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Reward implementation
Reward::Reward() : pImpl(new Impl) {
    pImpl->name = "Reward";
    pImpl->isIntegratedWithPlasticity = true;
    pImpl->isIntegratedWithAttention = true;
}

Reward::~Reward() = default;

const char* Reward::getName() const {
    return "Reward";
}

float Reward::getLevel() const {
    return pImpl->level;
}

void Reward::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Reward::getPlasticityFactor() const {
    // Reward directly enhances plasticity for rewarded actions
    return 1.0f + pImpl->level * pImpl->rewardAmplification;
}

void Reward::update(float reward, float predictionError, TimestepDuration dt) {
    // Reward dynamics with temporal difference learning
    
    // Integrate reward signal
    pImpl->level = std::min(pImpl->level + reward * pImpl->rewardIntegrationRate, 1.0f);
    
    // Prediction error modulates learning
    float predictionErrorEffect = pImpl->predictionErrorGain * predictionError;
    pImpl->level += predictionErrorEffect * 0.01f * static_cast<float>(dt);
    
    // Decay when reward is not present
    if (reward <= 0.0f) {
        pImpl->level *= (1.0f - 0.05f * static_cast<float>(dt));
    }
}

// PredictionError implementation
PredictionError::PredictionError() : pImpl(new Impl) {
    pImpl->name = "PredictionError";
    pImpl->isIntegratedWithPlasticity = true;
}

PredictionError::~PredictionError() = default;

const char* PredictionError::getName() const {
    return "PredictionError";
}

float PredictionError::getLevel() const {
    return pImpl->level;
}

void PredictionError::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float PredictionError::getPlasticityFactor() const {
    // Prediction error drives learning and plasticity
    return 1.0f + std::abs(pImpl->level) * pImpl->predictionErrorGain;
}

void PredictionError::update(float predictionError, TimestepDuration dt) {
    pImpl->level = std::clamp(predictionError, 0.0f, 1.0f);
    
    // Decay over time
    pImpl->level *= (1.0f - 0.1f * static_cast<float>(dt));
}

} // namespace nlm
