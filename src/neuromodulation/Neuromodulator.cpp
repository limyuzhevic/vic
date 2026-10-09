#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    // Phasic vs tonic dynamics
    float tonicLevel;           // Baseline dopamine level (tonic release)
    float phasicLevel;          // Phasic dopamine bursts (reward prediction error)
    float currentLevel;         // Combined level
    
    // Receptor dynamics and adaptation
    float receptorActivation;   // Dopamine receptor activation (D1, D2 types)
    float receptorAdaptation;   // Receptor desensitization/adaptation
    float receptorSubtypeRatio; // Ratio of D1 to D2 receptor activation
    
    // Working memory integration
    float workingMemoryInfluence; // Influence on working memory stability
    float eligibilityTrace;       // Eligibility trace for learning
    float learningRateModulator;  // Modulation of learning rates
    
    // Plasticity modulation
    float plasticityThreshold;   // Threshold for plasticity induction
    float eligibilityTraceDecay; // Decay rate of eligibility traces
    
    // Time constants and dynamics
    float tonicTimeConstant;    // Time constant for tonic release
    float phasicTimeConstant;   // Time constant for phasic bursts
    float receptorTimeConstant; // Time constant for receptor dynamics
    float eligibilityTraceTimeConstant; // Time constant for eligibility traces
    
    // Reward prediction error history
    float rpeHistory[5];         // Recent reward prediction error history
    size_t rpeHistoryIndex;
    
    // Learning-related parameters
    float predictionError;      // Current reward prediction error
    float predictionConfidence;  // Confidence in prediction
    float actionValue;           // Value of current action
    
    // Activity-dependent release
    float neuralActivityHistory[10]; // Recent neural activity
    size_t activityHistoryIndex;
    
    // Cross-neurotransmitter modulation
    float acetylcholineModulation;   // Interaction with ACh system
    float norepinephrineModulation; // Interaction with NE system
    float serotoninModulation;      // Interaction with serotonin system
    
    // Configuration parameters
    float baselineDA;            // Baseline dopamine level
    float maxDAPhasic;           // Maximum phasic dopamine level
    float DAR releaseRate;        // Release rate for dopamine
    float DADecayRate;            // Decay rate for dopamine
    float receptorSensitivity;    // Receptor sensitivity
    float eligibilityTraceRate;   // Rate of eligibility trace formation
    
    Impl() : tonicLevel(0.02f),
             phasicLevel(0.0f),
             currentLevel(0.02f),
             receptorActivation(0.0f),
             receptorAdaptation(1.0f),
             receptorSubtypeRatio(0.5f),
             workingMemoryInfluence(0.0f),
             eligibilityTrace(0.0f),
             learningRateModulator(1.0f),
             plasticityThreshold(0.1f),
             eligibilityTraceDecay(0.05f),
             tonicTimeConstant(100.0f),
             phasicTimeConstant(20.0f),
             receptorTimeConstant(50.0f),
             eligibilityTraceTimeConstant(10.0f),
             predictionError(0.0f),
             predictionConfidence(0.5f),
             actionValue(0.0f),
             acetylcholineModulation(1.0f),
             norepinephrineModulation(1.0f),
             serotoninModulation(1.0f),
             rpeHistoryIndex(0),
             activityHistoryIndex(0) {
        
        for (int i = 0; i < 5; ++i) {
            rpeHistory[i] = 0.0f;
        }
        
        for (int i = 0; i < 10; ++i) {
            neuralActivityHistory[i] = 0.0f;
        }
        
        // Default biological parameters
        baselineDA = 0.02f;           // Tonic level in nM
        maxDAPhasic = 1.0f;          // Maximum phasic level
        DAR_releaseRate = 0.8f;       // Release rate
        DADecayRate = 0.05f;          // Decay rate
        receptorSensitivity = 0.9f;   // Receptor sensitivity
        eligibilityTraceRate = 0.5f;  // Eligibility trace formation rate
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine neuromodulator initialized (Phase 2 enhanced)");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->currentLevel;
}

void Dopamine::setLevel(float level) {
    pImpl->currentLevel = Neuromodulator::clampLevel(level);
    // Update derived factors based on current level
    pImpl->workingMemoryInfluence = pImpl->currentLevel * 0.7f;
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + pImpl->currentLevel * 0.5f);
    pImpl->learningRateModulator = 1.0f + pImpl->currentLevel * 0.8f;
}

float Dopamine::getBaselineLevel() const {
    return pImpl->tonicLevel;
}

float Dopamine::getReceptorActivation() const {
    return pImpl->receptorActivation;
}

float Dopamine::getExcitabilityModulation() const {
    // Dopamine modulates neural excitability through D1 receptors
    return 1.0f + pImpl->receptorSubtypeRatio * pImpl->currentLevel * 1.0f;
}

float Dopamine::getWorkingMemoryModulation() const {
    return 1.0f + pImpl->workingMemoryInfluence;
}

float Dopamine::getEligibilityTraceModulation() const {
    return 1.0f + pImpl->eligibilityTrace * 0.5f;
}

float Dopamine::getAttentionModulation() const {
    return 1.0f + pImpl->currentLevel * 0.3f;
}

float Dopamine::getLearningRateModulation() const {
    return pImpl->learningRateModulator;
}

float Dopamine::getPlasticityFactor() const {
    return 1.0f + pImpl->plasticityThreshold * pImpl->currentLevel * 0.8f;
}

void Dopamine::update(TimestepDuration dt) {
    // Update reward prediction error history
    pImpl->rpeHistory[pImpl->rpeHistoryIndex] = pImpl->predictionError;
    pImpl->rpeHistoryIndex = (pImpl->rpeHistoryIndex + 1) % 5;
    
    // Update neural activity history
    pImpl->neuralActivityHistory[pImpl->activityHistoryIndex] = pImpl->predictionError * 0.2f;
    pImpl->activityHistoryIndex = (pImpl->activityHistoryIndex + 1) % 10;
    
    // Phasic dopamine release based on reward prediction error
    float avgRPE = 0.0f;
    for (int i = 0; i < 5; ++i) {
        avgRPE += pImpl->rpeHistory[i];
    }
    avgRPE /= 5.0f;
    
    pImpl->phasicLevel = std::min(pImpl->maxDAPhasic, std::abs(avgRPE) * pImpl->DAR_releaseRate);
    
    // Tonic dynamics (slow recovery towards baseline)
    if (pImpl->phasicLevel < pImpl->tonicLevel) {
        pImpl->tonicLevel = std::max(pImpl->tonicLevel, 
                                    pImpl->tonicLevel - pImpl->DADecayRate * static_cast<float>(dt));
    }
    
    // Combine tonic and phasic components
    pImpl->currentLevel = (pImpl->tonicLevel * pImpl->phasicTimeConstant + 
                          pImpl->phasicLevel * pImpl->tonicTimeConstant) / 
                         (pImpl->phasicTimeConstant + pImpl->tonicTimeConstant);
    
    // Update receptor activation with adaptation
    pImpl->receptorActivation = pImpl->currentLevel * pImpl->receptorSensitivity * 
                               pImpl->receptorAdaptation;
    
    // Adapt receptors based on activation level
    pImpl->receptorAdaptation = std::max(0.3f, 1.0f - pImpl->receptorActivation * 0.5f);
    
    // Decay eligibility traces
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - 
                                      pImpl->eligibilityTraceDecay * static_cast<float>(dt));
}

void Dopamine::signalEvent(float value, const std::string& eventType) {
    if (eventType == "reward_prediction_error") {
        pImpl->predictionError = value;
        pImpl->phasicLevel = std::min(pImpl->maxDAPhasic, std::abs(value) * 0.5f);
        // Update action value
        pImpl->actionValue += value * 0.1f;
        // Boost eligibility trace
        pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + value * 0.3f);
        // Update prediction confidence
        pImpl->predictionConfidence = std::min(1.0f, pImpl->predictionConfidence + value * 0.1f);
    } else if (eventType == "reward") {
        // Phasic dopamine burst on actual reward
        pImpl->phasicLevel = std::min(pImpl->maxDAPhasic, value * 0.7f);
        // Stronger eligibility trace for positive outcomes
        pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + value * 0.5f);
        // Update prediction confidence based on reward prediction error
        pImpl->predictionConfidence = std::min(1.0f, pImpl->predictionConfidence + value * 0.2f);
    } else if (eventType == "learning") {
        // Learning signal modulates plasticity
        pImpl->plasticityThreshold = std::max(0.01f, 0.1f - pImpl->currentLevel * 0.05f);
    } else if (eventType == "working_memory_update") {
        // Dopamine modulates working memory stability
        pImpl->workingMemoryInfluence = std::min(1.0f, pImpl->workingMemoryInfluence + value * 0.2f);
    }
}

std::string Dopamine::getNeurotransmitterType() const {
    return "Dopamine (DA)";
}

void Dopamine::applyCrossModulation(float level, const std::string& sourceNeurotransmitter) {
    if (sourceNeurotransmitter == "Acetylcholine") {
        // ACh can modulate dopamine release
        pImpl->acetylcholineModulation = level;
    } else if (sourceNeurotransmitter == "Norepinephrine") {
        // NE can modulate dopamine reward processing
        pImpl->norepinephrineModulation = level;
    } else if (sourceNeurotransmitter == "Serotonin") {
        // Serotonin can modulate dopamine learning rates
        pImpl->serotoninModulation = level;
    }
}

float Dopamine::getActivityDependentRelease(float neuralActivity) const {
    // Activity-dependent release based on neural activity and RPE history
    float activityFactor = neuralActivity * 0.1f;
    
    // Include contribution from activity history
    float activityHistorySum = 0.0f;
    for (int i = 0; i < 10; ++i) {
        activityHistorySum += pImpl->neuralActivityHistory[i];
    }
    float historyFactor = activityHistorySum / 10.0f;
    
    // Stress and arousal amplify release
    float stressAmplification = 1.0f + pImpl->predictionError * 0.5f;
    
    return std::clamp((activityFactor + historyFactor) * stressAmplification, 0.0f, 1.0f);
}

void Dopamine::signalLearning(float predictionError, float confidence) {
    signalEvent(predictionError, "reward_prediction_error");
    pImpl->predictionConfidence = confidence;
}

void Dopamine::signalWorkingMemoryUpdate(float memoryStrength) {
    signalEvent(memoryStrength, "working_memory_update");
}

void Dopamine::signalReward(float reward) {
    signalEvent(reward, "reward");
}

void Dopamine::signalRewardPredictionError(float error) {
    signalEvent(error, "reward_prediction_error");
}

} // namespace nlm
