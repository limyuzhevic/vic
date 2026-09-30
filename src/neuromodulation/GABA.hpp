#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <cmath>

namespace nlm {

struct GABAImpl {
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
    
    // Inhibition and anxiety parameters
    float inhibitoryControl;         // Inhibitory control strength
    float anxietyModulation;        // Anxiety regulation
    float fearResponse;             // Fear response level
    float impulseControl;            // Impulse control enhancement
    
    // Behavioral inhibition
    float behavioralInhibition;     // Behavioral inhibition level
    float responseSuppression;      // Response suppression strength
    float riskAssessment;            // Risk assessment and avoidance
    
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
    
    GABAImpl() :
        level(0.0f), baseline(0.02f), peak(2.0f),
        decayRate(0.1f), releaseRate(0.3f), synthesisRate(0.02f),
        reuptakeRate(0.25f), receptorDensity(1.3f), bindingAffinity(0.9f),
        inhibitoryControl(1.0f), anxietyModulation(1.0f), fearResponse(0.5f),
        impulseControl(1.0f), behavioralInhibition(1.0f), 
        responseSuppression(1.0f), riskAssessment(0.5f),
        developmentalOffset(1.0f), maturationStage(1.0f),
        experienceModulation(1.0f), memoryTrace(0.0f), eligibilityTrace(0.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix for GABA
        // GABA is the main inhibitory neurotransmitter
        interactionMatrix[0][0] = 1.0f;  // GABA-GABA self-interaction
        interactionMatrix[0][1] = 0.3f;  // GABA-5HT: excitation
        interactionMatrix[0][2] = 0.2f;  // GABA-NE: excitation
        interactionMatrix[0][3] = 0.2f;  // GABA-ACh: excitation
        interactionMatrix[0][4] = 1.0f;  // GABA-GABA: self
        interactionMatrix[0][5] = -0.5f; // GABA-Glu: inhibition
        
        // Other neuromodulators' effects on GABA
        interactionMatrix[1][4] = 0.4f;  // 5HT-GABA: excitation
        interactionMatrix[2][4] = 0.3f;  // NE-GABA: excitation
        interactionMatrix[3][4] = 0.3f;  // ACh-GABA: excitation
        interactionMatrix[4][0] = 1.0f;  // GABA-GABA: self
        interactionMatrix[5][4] = -0.3f; // Glu-GABA: inhibition
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

class GABA : public Neuromodulator {
public:
    GABA();
    ~GABA() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // GABA-specific functions
    void inhibitExcitation(float excitationLevel);
    void regulateAnxiety(float anxietyState);
    void controlImpulsiveBehavior(float impulseStrength);
    void modulateFearResponse(float fearLevel, float context);
    void integratePredictionError(float predictionError);
    void enhanceInhibitoryPlasticity(float learningRate);
    void updateInhibitoryDynamics(float dt);
    void encodeInhibitionMemory(float inhibition, float duration);
    void resetToBaseline(float recoveryRate, float dt);
    
    // State access
    float getInhibitionModulation() const;
    float getAnxietyModulation() const;
    float getImpulseControlModulation() const;
    float getFearModulation() const;
    
    // Advanced features
    void setFearConditioning(float conditioningStrength);
    void updateThreatSensitivity(float threatLevel);
    void enhanceConflictResolution(float conflictLevel);
    void modulateSocialBehavior(float socialContext);
    void updateStressResilience(float resilienceFactor);
    void enhanceEmotionalRegulation(float emotionalStability);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm