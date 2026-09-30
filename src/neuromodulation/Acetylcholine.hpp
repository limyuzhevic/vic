#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <cmath>

namespace nlm {

struct AcetylcholineImpl {
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
    
    // Memory and attention parameters
    float memoryConsolidation;      // Memory consolidation strength
    float attentionSpan;            // Attention span enhancement
    float corticalActivation;        // Cortical activation level
    float sensoryProcessing;        // Sensory processing modulation
    
    // Learning rate modulation
    float learningRate;              // Learning rate enhancement
    float noveltySensitivity;        // Sensitivity to novel information
    float explorationBias;           // Bias toward exploration vs exploitation
    
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
    
    AcetylcholineImpl() :
        level(0.0f), baseline(0.02f), peak(1.5f),
        decayRate(0.15f), releaseRate(0.35f), synthesisRate(0.025f),
        reuptakeRate(0.2f), receptorDensity(1.8f), bindingAffinity(0.85f),
        memoryConsolidation(1.0f), attentionSpan(1.0f), corticalActivation(0.5f),
        sensoryProcessing(1.0f), learningRate(1.0f), noveltySensitivity(1.0f),
        explorationBias(0.5f), developmentalOffset(1.0f),
        maturationStage(1.0f), experienceModulation(1.0f),
        memoryTrace(0.0f), eligibilityTrace(0.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix for ACh
        // ACh enhances memory, attention, and cortical activation
        interactionMatrix[0][0] = 1.0f;  // ACh-ACh self-interaction
        interactionMatrix[0][1] = 0.4f;  // ACh-5HT: excitation
        interactionMatrix[0][2] = 0.5f;  // ACh-NE: excitation
        interactionMatrix[0][3] = 1.0f;  // ACh-ACh: self
        interactionMatrix[0][4] = 0.3f;  // ACh-GABA: excitation
        interactionMatrix[0][5] = 0.6f;  // ACh-Glu: excitation
        
        // Other neuromodulators' effects on ACh
        interactionMatrix[1][3] = 0.5f;  // 5HT-ACh: excitation
        interactionMatrix[2][3] = 0.6f;  // NE-ACh: excitation
        interactionMatrix[3][0] = 1.0f;  // ACh-ACh: self
        interactionMatrix[4][3] = 0.4f;  // GABA-ACh: excitation
        interactionMatrix[5][3] = 0.5f;  // Glu-ACh: excitation
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Acetylcholine-specific functions
    void consolidateMemory(float memoryStrength, float age);
    void enhanceAttentionSpan(float taskComplexity);
    void activateCorticalAreas(float targetActivation);
    void modulateSensoryProcessing(float stimulusIntensity);
    void integratePredictionError(float predictionError);
    void enhanceLearningRate(float novelty, float uncertainty);
    void updateCholinergicDynamics(float dt);
    void encodeWorkingMemory(float content, float relevance);
    void resetToBaseline(float recoveryRate, float dt);
    
    // State access
    float getMemoryModulation() const;
    float getAttentionModulation() const;
    float getCorticalModulation() const;
    float getLearningModulation() const;
    
    // Advanced features
    void setNoveltyDetection(float detectionThreshold);
    void updatePatternSeparation(float patternSimilarity);
    void enhanceTemporalProcessing(float temporalPrecision);
    void modulateInhibitoryControl(float inhibitionLevel);
    void updateReceptiveFieldExpansion(float expansionRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm