#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <cmath>

namespace nlm {

struct NorepinephrineImpl {
    // Core neuromodulator parameters
    float level;                    // Current concentration/activity
    float baseline;                 // Baseline level
    float peak;                     // Maximum achievable level
    float decayRate;                // Natural decay rate
    float releaseRate;              // Release rate for phasic signals
    float synthesisRate;            // Synthesis/production rate
    float reuptakeRate;             // Reuptake/metabolism rate
    float receptorDensity;          // Receptor density for sensitivity
    float bindingAffinity;          // Binding affinity to receptors
    
    // Attention and arousal parameters
    float attentionModulation;       // Attention enhancement
    float arousalModulation;         // Arousal level
    float vigilanceModulation;       // Vigilance/arousal level
    float locusCoeruleusActivity;    // LC neuron firing rate
    
    // Stress and threat response
    float stressResponse;            // Fight-or-flight response
    float threatSensitivity;         // Sensitivity to threats
    float emergencyActivation;       // Emergency activation level
    
    // Cognitive processing enhancement
    float workingMemoryModulation;   // Working memory enhancement
    float cognitiveFlexibility;      // Cognitive flexibility
    float executiveFunction;         // Executive function modulation
    
    // Developmental trajectory
    float developmentalOffset;      // Developmental modulation
    float maturationStage;           // Current developmental stage
    float experienceModulation;     // Learning/modulation based on experience
    
    // Memory and prediction integration
    std::vector<float> history;     // Historical activity levels
    std::vector<float> predictionErrors; // Prediction error history
    float memoryTrace;              // Short-term memory trace
    float eligibilityTrace;         // Eligibility for plasticity
    
    // Interaction with other systems
    float interactionMatrix[6][6];  // Interaction with other neuromodulators
    std::vector<float> plasticityFactors; // Plasticity effects on synapses
    std::vector<float> neuralExcitability; // Effects on neural populations
    
    NorepinephrineImpl() :
        level(0.0f), baseline(0.01f), peak(2.0f),
        decayRate(0.2f), releaseRate(0.4f), synthesisRate(0.03f),
        reuptakeRate(0.25f), receptorDensity(1.5f), bindingAffinity(0.95f),
        attentionModulation(1.0f), arousalModulation(0.5f), 
        vigilanceModulation(0.5f), locusCoeruleusActivity(0.1f),
        stressResponse(0.0f), threatSensitivity(0.5f), emergencyActivation(0.0f),
        workingMemoryModulation(1.0f), cognitiveFlexibility(1.0f),
        executiveFunction(1.0f), developmentalOffset(1.0f),
        maturationStage(1.0f), experienceModulation(1.0f),
        memoryTrace(0.0f), eligibilityTrace(0.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix for NE
        // NE increases arousal, vigilance, and attention
        interactionMatrix[0][0] = 1.0f;  // NE-NE self-interaction
        interactionMatrix[0][1] = 0.3f;  // NE-5HT: excitation (modulated)
        interactionMatrix[0][2] = 1.0f;  // NE-NE: strong self
        interactionMatrix[0][3] = 0.2f;  // NE-ACh: excitation
        interactionMatrix[0][4] = -0.5f; // NE-GABA: strong inhibition
        interactionMatrix[0][5] = 0.6f;  // NE-Glu: excitation
        
        // Other neuromodulators' effects on NE
        interactionMatrix[1][2] = 0.5f;  // 5HT-NE: excitation
        interactionMatrix[2][0] = 1.2f;  // NE-NE: stronger
        interactionMatrix[3][2] = 0.3f;  // ACh-NE: excitation
        interactionMatrix[4][2] = -0.4f; // GABA-NE: inhibition
        interactionMatrix[5][2] = 0.5f;  // Glu-NE: excitation
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Norepinephrine-specific functions
    void enhanceAttention(float stimulusSalience);
    void increaseArousal(float level, float rate);
    void activateFightOrFlightResponse(float threatLevel, float urgency);
    void modulateStressResponse(float stressInput, float adaptation);
    void integratePredictionError(float predictionError);
    void enhanceCognitiveProcessing(float taskDemand);
    void updateLocusCoeruleusDynamics(float dt);
    void encodeAttentionMemory(float focus, float duration);
    void resetToBaseline(float recoveryRate, float dt);
    
    // State access
    float getAttentionModulation() const;
    float getArousalModulation() const;
    float getStressModulation() const;
    float getCognitiveModulation() const;
    
    // Advanced features
    void setVigilanceLevel(float level);
    void updateThreateningInput(float threatLevel);
    void enhanceMemoryEncoding(float learningRate);
    void modulateResponseSelection(float competition);
    void updateAdaptiveAllocation(float resourceAvailability);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm