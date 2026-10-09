#include "Norepinephrine.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    // Phasic vs tonic dynamics (locus coeruleus based)
    float tonicLevel;           // Background NE level (baseline arousal)
    float phasicLevel;          // Phasic NE bursts (surprise, novelty, stress)
    float currentLevel;         // Combined level
    
    // Receptor dynamics (alpha and beta adrenergic receptors)
    float receptorActivation;   // Adrenergic receptor activation
    float receptorAdaptation;   // Receptor desensitization/adaptation
    float adaptationTimeConstant; // Time constant for receptor adaptation
    
    // Cognitive function modulation
    float alertnessLevel;       // General alertness and vigilance
    float flexibilityLevel;    // Cognitive flexibility and adaptability
    float noveltySensitivity;   // Sensitivity to novel stimuli
    
    // Working memory and attention effects
    float workingMemoryStability; // Stabilizing effect on working memory
    float attentionalFocus;       // Enhancement of attentional focus
    float cognitiveControl;       // Executive control modulation
    
    // Time constants and dynamics
    float tonicTimeConstant;    // Time constant for tonic release
    float phasicTimeConstant;   // Time constant for phasic bursts
    float adaptationTimeConstant; // Receptor adaptation time constant
    float recoveryTimeConstant;   // Recovery from high activity
    
    // State tracking
    float recentNoveltyHistory[5];  // Recent novelty detection
    float spikeProbability;         // Probability of phasic bursts
    float firingThreshold;          // Threshold for phasic firing
    float firingRate;               // Current firing rate of LC neurons
    
    // Learning and plasticity
    float learningRateModulator;   // Modulation of learning rates
    float plasticityThreshold;     // Threshold for plasticity induction
    float memoryUpdating;          // Rate of memory updating
    
    // Cross-neurotransmitter interactions
    float acetylcholineModulation;   // Interaction with ACh system
    float serotoninModulation;       // Interaction with serotonin system
    float dopamineModulation;        // Interaction with dopamine system
    
    // Stress and arousal modulation
    float stressLevel;               // Current stress level
    float fatigueLevel;              // Fatigue level affecting NE release
    float environmentalComplexity;   // Complexity of environment
    
    // Configuration parameters
    float baselineNE;                // Baseline NE level
    float maxNELevel;                 // Maximum NE level
    float NEE_releaseRate;            // Rate of NE release
    float NEDecayRate;                 // Rate of NE decay
    float receptorSensitivity;        // Receptor sensitivity
    float noveltySensitivityFactor;   // Factor for novelty sensitivity
    
    Impl() : tonicLevel(0.02f),
             phasicLevel(0.0f),
             currentLevel(0.02f),
             receptorActivation(0.0f),
             receptorAdaptation(1.0f),
             adaptationTimeConstant(30.0f),
             alertnessLevel(0.5f),
             flexibilityLevel(0.5f),
             noveltySensitivity(0.5f),
             workingMemoryStability(0.0f),
             attentionalFocus(0.5f),
             cognitiveControl(0.5f),
             tonicTimeConstant(80.0f),
             phasicTimeConstant(15.0f),
             recoveryTimeConstant(40.0f),
             spikeProbability(0.1f),
             firingThreshold(0.7f),
             firingRate(0.0f),
             learningRateModulator(1.0f),
             plasticityThreshold(0.1f),
             memoryUpdating(0.5f),
             acetylcholineModulation(1.0f),
             serotoninModulation(1.0f),
             dopamineModulation(1.0f),
             stressLevel(0.0f),
             fatigueLevel(0.0f),
             environmentalComplexity(0.5f) {
        
        for (int i = 0; i < 5; ++i) {
            recentNoveltyHistory[i] = 0.0f;
        }
        
        // Default biological parameters
        baselineNE = 0.02f;           // Tonic level in nM
        maxNELevel = 1.0f;            // Maximum phasic level
        NEE_releaseRate = 0.6f;       // Release rate
        NEDecayRate = 0.08f;          // Decay rate
        receptorSensitivity = 0.85f;  // Receptor sensitivity
        noveltySensitivityFactor = 1.2f; // Factor for novelty sensitivity
    }
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine neuromodulator initialized");
}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->currentLevel;
}

void Norepinephrine::setLevel(float level) {
    pImpl->currentLevel = Neuromodulator::clampLevel(level);
    // Update derived factors based on current level
    pImpl->workingMemoryStability = pImpl->currentLevel * 0.7f;
    pImpl->attentionalFocus = 0.5f + pImpl->currentLevel * 1.5f;
    pImpl->cognitiveControl = 1.0f + pImpl->currentLevel * 1.2f;
}

float Norepinephrine::getBaselineLevel() const {
    return pImpl->tonicLevel;
}

float Norepinephrine::getReceptorActivation() const {
    return pImpl->receptorActivation;
}

float Norepinephrine::getExcitabilityModulation() const {
    return 1.0f + pImpl->alertnessLevel * 0.8f;
}

float Norepinephrine::getWorkingMemoryModulation() const {
    return 1.0f + pImpl->workingMemoryStability;
}

float Norepinephrine::getEligibilityTraceModulation() const {
    return 1.0f + pImpl->plasticityThreshold * 0.3f;
}

float Norepinephrine::getAttentionModulation() const {
    return pImpl->attentionalFocus;
}

float Norepinephrine::getLearningRateModulation() const {
    return 1.0f + pImpl->learningRateModulator * pImpl->currentLevel * 0.5f;
}

float Norepinephrine::getPlasticityFactor() const {
    return 1.0f + pImpl->plasticityThreshold * pImpl->currentLevel * 0.8f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Update firing rate based on spike probability
    if ((rand() % 1000) / 1000.0f < pImpl->spikeProbability) {
        pImpl->firingRate = std::min(pImpl->maxNELevel, pImpl->firingRate + 0.1f);
    }
    
    // Natural decay of firing rate
    pImpl->firingRate = std::max(0.0f, pImpl->firingRate - pImpl->recoveryTimeConstant * static_cast<float>(dt) * 0.01f);
    
    // Phasic release based on firing rate
    pImpl->phasicLevel = pImpl->firingRate;
    
    // Tonic dynamics (slow recovery towards baseline)
    if (pImpl->phasicLevel < pImpl->tonicLevel) {
        pImpl->tonicLevel = std::max(pImpl->tonicLevel, 
                                    pImpl->tonicLevel - pImpl->NEDecayRate * static_cast<float>(dt));
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
    
    // Update cognitive states based on NE level
    pImpl->alertnessLevel = std::min(1.0f, pImpl->alertnessLevel + 
                                     (pImpl->currentLevel - pImpl->alertnessLevel) * 0.1f);
    
    pImpl->flexibilityLevel = std::min(1.0f, pImpl->flexibilityLevel + 
                                      (pImpl->currentLevel * 0.5f - pImpl->flexibilityLevel) * 0.05f);
    
    // Update novelty sensitivity based on stress and fatigue
    float noveltyMod = 1.0f + pImpl->stressLevel * 0.5f - pImpl->fatigueLevel * 0.3f;
    pImpl->noveltySensitivity = std::clamp(noveltyMod, 0.1f, 2.0f);
    
    // Shift novelty history (rolling window)
    for (int i = 4; i > 0; --i) {
        pImpl->recentNoveltyHistory[i] = pImpl->recentNoveltyHistory[i - 1];
    }
    pImpl->recentNoveltyHistory[0] = pImpl->firingRate;
}

void Norepinephrine::signalEvent(float value, const std::string& eventType) {
    if (eventType == "novelty") {
        // NE responds to novelty with phasic bursts
        float noveltyBurst = value * pImpl->noveltySensitivityFactor * 
                            (1.0f + pImpl->stressLevel);
        pImpl->phasicLevel = std::min(pImpl->maxNELevel, noveltyBurst);
        // Update novelty history
        for (int i = 4; i > 0; --i) {
            pImpl->recentNoveltyHistory[i] = pImpl->recentNoveltyHistory[i - 1];
        }
        pImpl->recentNoveltyHistory[0] = pImpl->phasicLevel;
        // Increase flexibility
        pImpl->flexibilityLevel = std::min(1.0f, pImpl->flexibilityLevel + 0.3f * value);
    } else if (eventType == "stress") {
        // NE responds to stress
        pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + value * 0.3f);
        pImpl->phasicLevel = std::min(pImpl->maxNELevel, value * 0.8f);
        // Decrease flexibility under stress
        pImpl->flexibilityLevel = std::max(0.1f, pImpl->flexibilityLevel - 0.2f * value);
    } else if (eventType == "learning") {
        // NE enhances learning under novel conditions
        if (getNoveltyHistory() > 0.5f) {
            pImpl->phasicLevel = std::min(pImpl->maxNELevel, value * 0.7f);
            pImpl->learningRateModulator = 1.5f;
        }
    } else if (eventType == "attention") {
        // NE modulates attentional focus
        pImpl->attentionalFocus = std::min(1.0f, pImpl->attentionalFocus + value * 0.4f);
    }
}

std::string Norepinephrine::getNeurotransmitterType() const {
    return "Norepinephrine (NE)";
}

void Norepinephrine::applyCrossModulation(float level, const std::string& sourceNeurotransmitter) {
    if (sourceNeurotransmitter == "Acetylcholine") {
        // ACh can modulate NE release
        pImpl->acetylcholineModulation = level;
    } else if (sourceNeurotransmitter == "Serotonin") {
        // Serotonin can modulate NE stress responses
        pImpl->serotoninModulation = level;
    } else if (sourceNeurotransmitter == "Dopamine") {
        // Dopamine can modulate NE learning effects
        pImpl->dopamineModulation = level;
    }
}

float Norepinephrine::getActivityDependentRelease(float neuralActivity) const {
    // Activity-dependent release based on neural activity and novelty
    float activityFactor = neuralActivity * 0.08f;
    
    // Include contribution from novelty history
    float noveltyFactor = 0.0f;
    for (int i = 0; i < 5; ++i) {
        noveltyFactor += pImpl->recentNoveltyHistory[i];
    }
    noveltyFactor /= 5.0f;
    
    // Stress amplifies activity-dependent release
    float stressAmplification = 1.0f + pImpl->stressLevel * 0.5f;
    
    return std::clamp((activityFactor + noveltyFactor) * stressAmplification, 0.0f, 1.0f);
}

float Norepinephrine::getNoveltyHistory() const {
    float total = 0.0f;
    for (int i = 0; i < 5; ++i) {
        total += pImpl->recentNoveltyHistory[i];
    }
    return total / 5.0f;
}

void Norepinephrine::signalNoveltyDetection(float noveltyStrength) {
    signalEvent(noveltyStrength, "novelty");
}

void Norepinephrine::signalCognitiveFlexibility(float flexibilityLevel) {
    pImpl->flexibilityLevel = std::clamp(flexibilityLevel, 0.0f, 1.0f);
    // Modulate firing threshold based on flexibility
    pImpl->firingThreshold = 0.7f - flexibilityLevel * 0.3f;
    pImpl->spikeProbability = 0.1f + flexibilityLevel * 0.3f;
}

void Norepinephrine::signalVigilance(float alertnessLevel) {
    pImpl->alertnessLevel = std::clamp(alertnessLevel, 0.0f, 1.0f);
    pImpl->phasicLevel = std::min(pImpl->maxNELevel, alertnessLevel * 0.9f);
}

} // namespace nlm
