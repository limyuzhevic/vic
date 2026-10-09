#include "Acetylcholine.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    // Phasic vs tonic dynamics
    float tonicLevel;           // Background ACh level (baseline arousal)
    float phasicLevel;          // Phasic ACh bursts (learning events)
    float currentLevel;         // Combined level
    
    // Receptor dynamics
    float receptorActivation;   // Muscarinic (mAChR) and nicotinic (nAChR) activation
    float receptorAdaptation;   // Receptor desensitization/adaptation
    
    // Memory integration
    float workingMemoryInfluence; // Influence on working memory stability
    float episodicMemoryBoost;   // Boost for episodic memory consolidation
    
    // Attention system interaction
    float attentionGain;        // Modulation of attentional gain
    float attentionalFocus;     // Focus on specific neural populations
    
    // Time constants
    float tonicTimeConstant;    // Time constant for tonic release
    float phasicTimeConstant;   // Time constant for phasic bursts
    float receptorTimeConstant; // Time constant for receptor dynamics
    
    // Learning-related parameters
    float learningRateModulator; // Modulation of learning rates
    float plasticityThreshold;   // Threshold for plasticity induction
    
    // Cross-neurotransmitter modulation
    float dopamineModulation;   // Interaction with dopamine system
    float norepinephrineModulation; // Interaction with norepinephrine system
    
    // Activity-dependent release
    float activityHistory[10];  // Recent neural activity history
    size_t activityHistoryIndex;
    
    // Input weights
    float rewardPredictionErrorWeight; // Response to RPE signals
    float noveltyWeight;               // Response to novel events
    float attentionShiftWeight;        // Response to attention shifts
    
    // Configuration parameters
    float baselineACh;           // Baseline ACh level
    float maxAChLevel;           // Maximum ACh level
    float AChReleaseRate;         // Rate of ACh release
    float AChDecayRate;           // Rate of ACh decay
    float receptorSensitivity;    // Receptor sensitivity
    
    Impl() : tonicLevel(0.05f),
             phasicLevel(0.0f),
             currentLevel(0.05f),
             receptorActivation(0.0f),
             receptorAdaptation(1.0f),
             workingMemoryInfluence(0.0f),
             episodicMemoryBoost(0.0f),
             attentionGain(1.0f),
             attentionalFocus(0.0f),
             tonicTimeConstant(100.0f),
             phasicTimeConstant(20.0f),
             receptorTimeConstant(50.0f),
             learningRateModulator(1.0f),
             plasticityThreshold(0.1f),
             dopamineModulation(1.0f),
             norepinephrineModulation(1.0f),
             activityHistoryIndex(0) {
        
        for (int i = 0; i < 10; ++i) {
            activityHistory[i] = 0.0f;
        }
        
        // Default biological parameters
        baselineACh = 0.05f;           // Tonic level in nM
        maxAChLevel = 1.0f;            // Maximum phasic level
        AChReleaseRate = 0.8f;         // Release rate
        AChDecayRate = 0.05f;          // Decay rate
        receptorSensitivity = 0.9f;    // Receptor sensitivity
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    NLM_LOG_INFO("Acetylcholine neuromodulator initialized");
}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->currentLevel;
}

void Acetylcholine::setLevel(float level) {
    pImpl->currentLevel = Neuromodulator::clampLevel(level);
    // Update derived factors based on current level
    pImpl->workingMemoryInfluence = pImpl->currentLevel * 0.8f;
    pImpl->episodicMemoryBoost = pImpl->currentLevel * 0.6f;
    pImpl->attentionGain = 1.0f + pImpl->currentLevel * 1.5f;
}

float Acetylcholine::getBaselineLevel() const {
    return pImpl->tonicLevel;
}

float Acetylcholine::getReceptorActivation() const {
    return pImpl->receptorActivation;
}

float Acetylcholine::getExcitabilityModulation() const {
    return 1.0f + pImpl->attentionGain * 0.5f;
}

float Acetylcholine::getWorkingMemoryModulation() const {
    return 1.0f + pImpl->workingMemoryInfluence * 2.0f;
}

float Acetylcholine::getEligibilityTraceModulation() const {
    return 1.0f + pImpl->plasticityThreshold * 0.5f;
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getLearningRateModulation() const {
    return 1.0f + pImpl->learningRateModulator * pImpl->currentLevel;
}

float Acetylcholine::getPlasticityFactor() const {
    return 1.0f + pImpl->plasticityThreshold * pImpl->currentLevel;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Update phasic ACh based on recent activity history
    float activityAverage = 0.0f;
    for (int i = 0; i < 10; ++i) {
        activityAverage += pImpl->activityHistory[i];
    }
    activityAverage /= 10.0f;
    
    // Phasic release from activity history
    pImpl->phasicLevel = std::min(pImpl->maxAChLevel, 
                                 activityAverage * pImpl->AChReleaseRate);
    
    // Tonic dynamics (slow decay towards baseline)
    if (pImpl->phasicLevel < pImpl->tonicLevel) {
        pImpl->tonicLevel = std::max(pImpl->tonicLevel, 
                                    pImpl->tonicLevel - pImpl->AChDecayRate * static_cast<float>(dt));
    }
    
    // Combine tonic and phasic components
    pImpl->currentLevel = (pImpl->tonicLevel * pImpl->phasicTimeConstant + 
                          pImpl->phasicLevel * pImpl->tonicTimeConstant) / 
                         (pImpl->phasicTimeConstant + pImpl->tonicTimeConstant);
    
    // Update receptor activation with adaptation
    pImpl->receptorActivation = pImpl->currentLevel * pImpl->receptorSensitivity * 
                               pImpl->receptorAdaptation;
    
    // Adapt receptors based on activation level
    pImpl->receptorAdaptation = std::max(0.3f, 1.0f - pImpl->receptorActivation * 0.7f);
    
    // Update activity history (rolling window)
    pImpl->activityHistory[pImpl->activityHistoryIndex] = 0.0f;
    pImpl->activityHistoryIndex = (pImpl->activityHistoryIndex + 1) % 10;
}

void Acetylcholine::signalEvent(float value, const std::string& eventType) {
    if (eventType == "reward_prediction_error") {
        // ACh responds to reward prediction errors with moderate strength
        pImpl->phasicLevel = std::min(pImpl->maxAChLevel, 
                                     value * pImpl->rewardPredictionErrorWeight);
        // Boost working memory encoding
        pImpl->workingMemoryInfluence += pImpl->phasicLevel * 0.3f;
    } else if (eventType == "novelty") {
        // ACh signals novelty detection, enhancing attention
        pImpl->phasicLevel = std::min(pImpl->maxAChLevel, 
                                     value * 0.8f);
        // Increase attentional focus
        pImpl->attentionalFocus = std::max(pImpl->attentionalFocus, value * 0.5f);
    } else if (eventType == "attention_shift") {
        // ACh responds to attention shifts
        pImpl->phasicLevel = std::min(pImpl->maxAChLevel, 
                                     value * pImpl->attentionShiftWeight);
        // Stabilize working memory
        pImpl->workingMemoryInfluence += pImpl->phasicLevel * 0.2f;
    } else if (eventType == "working_memory_update") {
        // ACh stabilizes working memory representations
        pImpl->workingMemoryInfluence += value * 0.4f;
        // Enhance episodic memory consolidation
        pImpl->episodicMemoryBoost += value * 0.3f;
    }
}

std::string Acetylcholine::getNeurotransmitterType() const {
    return "Acetylcholine (ACh)";
}

void Acetylcholine::applyCrossModulation(float level, const std::string& sourceNeurotransmitter) {
    if (sourceNeurotransmitter == "Dopamine") {
        // Dopamine can modulate ACh release
        pImpl->dopamineModulation = level;
    } else if (sourceNeurotransmitter == "Norepinephrine") {
        // Norepinephrine can modulate ACh attention effects
        pImpl->norepinephrineModulation = level;
    }
}

float Acetylcholine::getActivityDependentRelease(float neuralActivity) const {
    // Activity-dependent release based on recent neural activity
    float activityFactor = neuralActivity * 0.1f;
    // Include contribution from activity history
    float historyFactor = 0.0f;
    for (int i = 0; i < 10; ++i) {
        historyFactor += pImpl->activityHistory[i];
    }
    historyFactor /= 10.0f;
    
    return std::clamp(activityFactor + historyFactor, 0.0f, 1.0f);
}

void Acetylcholine::signalAttentionShift(float newFocusStrength) {
    signalEvent(newFocusStrength, "attention_shift");
}

void Acetylcholine::signalWorkingMemoryEncoding(float memoryContent) {
    signalEvent(memoryContent, "working_memory_update");
}

void Acetylcholine::signalWorkingMemoryConsolidation(float memoryStrength) {
    // Consolidation signal enhances stability
    pImpl->workingMemoryInfluence += memoryStrength * 0.5f;
    // Also boost episodic memory consolidation
    pImpl->episodicMemoryBoost += memoryStrength * 0.4f;
}

} // namespace nlm
