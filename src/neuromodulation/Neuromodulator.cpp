#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// Forward declarations
class NeuralWorkingMemory;
class NeuralEpisodicMemory;

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionErrorGain;
    float rewardPredictionGain;
    float timeConstant;
    float eligibilityTrace;
    float neuralExcitabilityFactor;
    float plasticityBaseline;
    float learningRateModulation;
    float workingMemoryEnhancement;
    float attentionalBoost;
    float predictionLearning;
    float rewardPaving;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , predictionErrorGain(1.0f)
        , rewardPredictionGain(2.0f)
        , timeConstant(0.5f)
        , eligibilityTrace(0.0f)
        , neuralExcitabilityFactor(1.0f)
        , plasticityBaseline(1.0f)
        , learningRateModulation(1.0f)
        , workingMemoryEnhancement(1.0f)
        , attentionalBoost(1.0f)
        , predictionLearning(1.0f)
        , rewardPaving(1.0f) {}
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
    // Enhanced dopamine-modulated plasticity factor
    // Combines prediction error, reward sensitivity, and learning state
    float predictionMod = std::pow(pImpl->level, 0.7f) * pImpl->predictionErrorGain;
    float rewardMod = (1.0f - std::pow(pImpl->level, 2.0f)) * pImpl->rewardPredictionGain;
    
    // Memory consolidation factor during learning
    float memoryConsolidation = 0.5f + 0.5f * pImpl->level * pImpl->timeConstant;
    
    // Eligibility trace accumulation
    float eligibilityFactor = 0.8f + 0.2f * pImpl->eligibilityTrace;
    
    // Neural excitability modulation
    float excitabilityMod = 1.0f + 0.5f * pImpl->neuralExcitabilityFactor;
    
    return plasticityBaseline * predictionMod * rewardMod * memoryConsolidation * 
           eligibilityFactor * excitabilityMod;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with multiple time constants
    float decay = pImpl->decayRate * static_cast<float>(dt);
    float relaxation = 1.0f - std::exp(-decay / pImpl->timeConstant);
    
    // Smooth decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level * (1.0f - relaxation * 0.9f));
    
    // Update eligibility trace for reinforcement learning
    pImpl->eligibilityTrace = std::max(0.0f, 
                                     pImpl->eligibilityTrace - 0.01f * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Robust reward signaling with saturation
    float burst = std::min(reward * pImpl->releaseRate, pImpl->peak - pImpl->level);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Update eligibility trace for reward prediction
    pImpl->eligibilityTrace = std::min(1.0f, 
                                     pImpl->eligibilityTrace + 0.3f * reward);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    float errorMod = error * pImpl->releaseRate * pImpl->predictionErrorGain;
    pImpl->level = std::max(0.0f, 
                          std::min(pImpl->peak, pImpl->level + errorMod * 0.1f));
    
    // Accumulate prediction error for learning
    pImpl->eligibilityTrace = std::min(1.0f, 
                                     pImpl->eligibilityTrace + 0.2f * std::abs(error));
}

void Dopamine::applyToNeuralExcitability(std::vector<Neuron*>& neurons) {
    // Enhanced dopamine effects on neural excitability
    float dopaminergicMod = pImpl->level * pImpl->neuralExcitabilityFactor;
    
    for (auto* neuron : neurons) {
        // Dopamine modulates excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float excitabilityMod = dopaminergicMod * 0.3f;
        
        if (excitabilityMod > 0.0f) {
            // Apply neuromodulatory effect to neuron excitability
            neuron->applyNeuromodulation(pImpl->level, pImpl->predictionLearning);
        }
        
        // Learning rate modulation based on prediction error
        float learningMod = 1.0f + pImpl->learningRateModulation * pImpl->level;
        neuron->setLearningRate(learningMod);
        
        // Working memory enhancement
        if (pImpl->level > 0.7f) {
            neuron->enhanceWorkingMemory(pImpl->workingMemoryEnhancement);
        }
        
        // Attentional boost for salient events
        if (pImpl->level > 0.5f) {
            neuron->applyAttentionalBoost(pImpl->attentionalBoost);
        }
        
        // Prediction-based learning
        if (pImpl->eligibilityTrace > 0.3f) {
            neuron->applyPredictionLearning(pImpl->predictionLearning, pImpl->level);
        }
        
        // Reward paving - strengthen reward-related pathways
        if (pImpl->level > 0.8f) {
            neuron->applyRewardPaving(pImpl->rewardPaving);
        }
    }
}

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float riseTime;
    float reuptakeRate;
    float attentionGain;
    float memoryGain;
    float workingMemoryGain;
    float noiseReduction;
    
    // Temporal dynamics
    float sensoryTone;
    float cholinergicBurst;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , riseTime(0.1f)
        , reuptakeRate(0.3f)
        , attentionGain(2.0f)
        , memoryGain(1.5f)
        , workingMemoryGain(1.8f)
        , noiseReduction(0.7f)
        , sensoryTone(0.0f)
        , cholinergicBurst(0.0f) {}
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
    // ACh enhances plasticity for attention-related learning
    float attentionMod = 1.0f + pImpl->attentionGain * pImpl->level;
    float memoryMod = 1.0f + pImpl->memoryGain * pImpl->level * 0.7f;
    float workingMemoryMod = 1.0f + pImpl->workingMemoryGain * pImpl->level * 0.5f;\n    
    // Noise reduction improves signal-to-noise ratio
    float noiseMod = 1.0f / (pImpl->noiseReduction * pImpl->level + 0.1f);
    
    return attentionMod * memoryMod * workingMemoryMod * noiseMod;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: burst firing followed by slow decay
    float decay = pImpl->decayRate * static_cast<float>(dt);
    
    // Apply cholinergic bursts
    if (pImpl->cholinergicBurst > 0.0f) {
        float rise = 1.0f - std::exp(-decay / pImpl->riseTime);
        pImpl->level = std::min(pImpl->peak, 
                              pImpl->level + rise * pImpl->cholinergicBurst);
        pImpl->cholinergicBurst *= 0.8f;
    }
    
    // Natural decay and reuptake
    float reuptakeFactor = 1.0f - pImpl->reuptakeRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level * reuptakeFactor);
    
    // Sensory tone modulation
    float sensoryUpdate = pImpl->sensoryTone * 0.01f * static_cast<float>(dt);
    pImpl->sensoryTone = std::max(0.0f, pImpl->sensoryTone - sensoryUpdate);
}

void Acetylcholine::signalSensoryInput() {
    // Simulate ACh release in response to novel/important sensory input
    pImpl->cholinergicBurst = std::min(1.0f, pImpl->level + 0.5f);
    pImpl->sensoryTone = 1.0f;
}

void Acetylcholine::signalAttentionCommand(float priority) {
    // Simulate ACh modulation of attention
    float attentionBurst = std::pow(priority, 1.5f) * 0.8f;
    pImpl->cholinergicBurst = std::min(1.0f, pImpl->level + attentionBurst);
}

void Acetylcholine::signalWorkingMemoryAccess() {
    // Simulate ACh release during working memory operations
    pImpl->cholinergicBurst = std::min(0.6f, pImpl->level + 0.4f);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float riseTime;
    float reuptakeRate;
    float arousalGain;
    float vigilanceGain;
    float memoryEncodingGain;
    float uncertaintyGain;
    float plasticityEnhancement;
    
    // Alertness state variables
    float alertnessState;
    float locusCoeruleusBurst;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(1.5f)
        , riseTime(0.15f)
        , reuptakeRate(0.4f)
        , arousalGain(2.0f)
        , vigilanceGain(1.8f)
        , memoryEncodingGain(1.6f)
        , uncertaintyGain(1.2f)
        , plasticityEnhancement(1.3f)
        , alertnessState(0.0f)
        , locusCoeruleusBurst(0.0f) {}
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
    // NE enhances plasticity for arousal-dependent learning
    float arousalMod = 1.0f + pImpl->arousalGain * pImpl->level;
    float vigilanceMod = 1.0f + pImpl->vigilanceGain * pImpl->level * 0.8f;
    float encodingMod = 1.0f + pImpl->memoryEncodingGain * pImpl->level * 0.7f;
    
    // Uncertainty-based learning enhancement
    float uncertaintyMod = 1.0f + pImpl->uncertaintyGain * (1.0f - pImpl->level);
    
    // Overall plasticity enhancement
    return arousalMod * vigilanceMod * encodingMod * uncertaintyMod * 
           pImpl->plasticityEnhancement;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: phasic bursts followed by tonic decay
    float decay = pImpl->decayRate * static_cast<float>(dt);
    
    // Handle phasic locus coeruleus bursts
    if (pImpl->locusCoeruleusBurst > 0.0f) {
        float rise = 1.0f - std::exp(-decay / pImpl->riseTime);
        pImpl->level = std::min(pImpl->peak, 
                              pImpl->level + rise * pImpl->locusCoeruleusBurst);
        pImpl->locusCoeruleusBurst *= 0.85f;
    }
    
    // Tonic decay and reuptake
    float reuptakeFactor = 1.0f - pImpl->reuptakeRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level * reuptakeFactor);
    
    // Alertness state dynamics
    pImpl->alertnessState = std::max(0.0f, 
                                   pImpl->alertnessState - 0.02f * static_cast<float>(dt));
}

void Norepinephrine::signalArousalEvent(float intensity) {
    // Simulate NE release in response to salient events
    float burst = std::pow(intensity, 1.2f) * 0.9f;
    pImpl->locusCoeruleusBurst = std::min(1.0f, pImpl->level + burst);
}

void Norepinephrine::signalVigilanceUpdate(float vigilanceLevel) {
    // Update vigilance state
    pImpl->alertnessState = std::min(1.0f, pImpl->alertnessState + 0.1f * vigilanceLevel);
}

void Norepinephrine::signalDecisionUncertainty(float uncertainty) {
    // NE signals uncertainty and modulates decision-making
    float uncertaintyMod = 1.0f + pImpl->uncertaintyGain * uncertainty;
    // Store for use in decision modulation
    pImpl->alertnessState = std::min(1.0f, pImpl->alertnessState + 0.05f * uncertainty);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float riseTime;
    float reuptakeRate;
    float moodGain;
    float impulsivityGain;
    float socialBehaviorGain;
    float learningRateModulation;
    float rewardSensitivity;
    
    // Social and emotional state
    float socialBondingState;
    float satietyState;
    float threatState;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.06f)
        , releaseRate(1.2f)
        , riseTime(0.12f)
        , reuptakeRate(0.35f)
        , moodGain(1.8f)
        , impulsivityGain(1.4f)
        , socialBehaviorGain(1.6f)
        , learningRateModulation(0.8f)
        , rewardSensitivity(1.0f)
        , socialBondingState(0.0f)
        , satietyState(0.0f)
        , threatState(0.0f) {}
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
    // 5-HT modulates plasticity: slows learning under uncertainty
    float moodMod = 1.0f + pImpl->moodGain * pImpl->level * 0.6f;
    float socialMod = 1.0f + pImpl->socialBehaviorGain * pImpl->level * 0.5f;
    
    // Learning rate modulation: slower when threat is high
    float threatMod = 1.0f - pImpl->threatState * 0.5f * pImpl->learningRateModulation;
    
    // Reward sensitivity modulation
    float rewardMod = 1.0f + pImpl->rewardSensitivity * pImpl->level * 0.4f;
    
    return moodMod * socialMod * threatMod * rewardMod;
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics: slower, more sustained release
    float decay = pImpl->decayRate * static_cast<float>(dt);
    
    // Apply slow, sustained release
    float rise = 1.0f - std::exp(-decay / pImpl->riseTime);
    // Note: level changes are applied through signal methods
    
    // State transitions
    pImpl->satietyState = std::max(0.0f, 
                                   pImpl->satietyState - 0.005f * static_cast<float>(dt));
    pImpl->threatState = std::max(0.0f, 
                                   pImpl->threatState - 0.01f * static_cast<float>(dt));
}

void Serotonin::signalMoodChange(float moodDelta) {
    // Simulate 5-HT release in response to mood changes
    float moodBurst = std::pow(moodDelta, 1.5f) * 0.8f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodBurst);
}

void Serotonin::signalSocialInteraction(float socialQuality) {
    // 5-HT responds to social experiences
    float socialBurst = std::pow(socialQuality, 1.8f) * 0.7f;
    pImpl->socialBondingState = std::min(1.0f, 
                                         pImpl->socialBondingState + 0.2f * socialQuality);
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialBurst);
}

void Serotonin::signalImpulsivityChange(float impulsivityDelta) {
    // 5-HT modulates impulsivity: higher levels reduce impulsivity
    float delta = -0.3f * impulsivityDelta; // Reduce impulsivity
    pImpl->level = std::max(pImpl->baseline, pImpl->level + delta);
}

void Serotonin::signalRiskTaking(float riskLevel) {
    // 5-HT influences risk-taking behavior
    float riskMod = 1.0f - 0.5f * riskLevel; // Higher risk = lower 5-HT tone
    // Store for use in risk assessment
    pImpl->threatState = std::min(1.0f, pImpl->threatState + 0.05f * riskLevel);
}

void Serotonin::signalReward(float reward, bool positiveOutcome) {
    // Reward prediction in serotonergic system
    if (positiveOutcome) {
        float rewardBurst = std::min(reward * pImpl->releaseRate, 0.5f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + rewardBurst);
        pImpl->rewardSensitivity = std::min(1.5f, pImpl->rewardSensitivity + 0.1f);
    } else {
        // Negative outcomes may trigger 5-HT response
        pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.3f * reward);
        pImpl->rewardSensitivity = std::max(0.5f, pImpl->rewardSensitivity - 0.1f);
    }
}

} // namespace nlm