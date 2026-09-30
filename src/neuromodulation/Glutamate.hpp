#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <cmath>

namespace nlm {

struct GlutamateImpl {
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
    
    // Excitatory and plasticity parameters
    float excitatoryStrength;       // Excitatory signal strength
    float LTPStrength;              // Long-term potentiation strength
    float LTDStrength;              // Long-term depression strength
    float synapticEfficiency;        // Synaptic efficiency
    
    // Memory and learning
    float learningRate;              // Learning rate enhancement
    float patternCompletion;         // Pattern completion capability
    float interference;              // Interference with other memories
    
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
    
    GlutamateImpl() :
        level(0.0f), baseline(0.01f), peak(2.0f),
        decayRate(0.1f), releaseRate(0.5f), synthesisRate(0.03f),
        reuptakeRate(0.15f), receptorDensity(1.4f), bindingAffinity(0.95f),
        excitatoryStrength(1.0f), LTPStrength(1.0f), LTDStrength(1.0f),
        synapticEfficiency(1.0f), learningRate(1.0f), patternCompletion(1.0f),
        interference(0.1f), developmentalOffset(1.0f),
        maturationStage(1.0f), experienceModulation(1.0f),
        memoryTrace(0.0f), eligibilityTrace(0.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix for Glu
        // Glutamate is the main excitatory neurotransmitter
        interactionMatrix[0][0] = 1.0f;  // Glu-Glu self-interaction
        interactionMatrix[0][1] = -0.3f; // Glu-5HT: inhibition
        interactionMatrix[0][2] = -0.2f; // Glu-NE: inhibition
        interactionMatrix[0][3] = -0.2f; // Glu-ACh: inhibition
        interactionMatrix[0][4] = 0.5f;  // Glu-GABA: excitation
        interactionMatrix[0][5] = 1.0f;  // Glu-Glu: self
        
        // Other neuromodulators' effects on Glu
        interactionMatrix[1][5] = -0.4f; // 5HT-Glu: inhibition
        interactionMatrix[2][5] = -0.3f; // NE-Glu: inhibition
        interactionMatrix[3][5] = -0.3f; // ACh-Glu: inhibition
        interactionMatrix[4][5] = 0.6f;  // GABA-Glu: excitation
        interactionMatrix[5][0] = 1.0f;  // Glu-Glu: self
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

class Glutamate : public Neuromodulator {
public:
    Glutamate();
    ~Glutamate() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Glutamate-specific functions
    void enhanceExcitatorySignaling(float synapticStrength);
    void facilitateLongTermPotentiation(float learningRate, float predictionError);
    void activateCorticalExcitability(float neuralActivation);
    void modulateSensoryExcitability(float stimulusIntensity);
    void integratePredictionError(float predictionError);
    void enhanceSynapticPlasticity(float eligibilityTrace);
    void updateExcitatoryDynamics(float dt);
    void encodeLTPMemory(float potentiation, float duration);
    void resetToBaseline(float recoveryRate, float dt);
    
    // State access
    float getExcitabilityModulation() const;
    float getLTPModulation() const;
    float getSynapticModulation() const;
    float getLearningModulation() const;
    
    // Advanced features
    void setSensoryGain(float gainLevel);
    void updateTemporalIntegration(float timeWindow);
    void enhancePatternSeparation(float patternDistance);
    void modulateStructuralPlasticity(float structuralChange);
    void updateHebbianLearning(float correlation);
    void enhanceMetaplasticity(float metaplasticState);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm