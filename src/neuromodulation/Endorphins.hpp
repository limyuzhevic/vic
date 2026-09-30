#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <cmath>

namespace nlm {

struct EndorphinsImpl {
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
    
    // Pain and reward parameters
    float painModulation;            // Pain perception modulation
    float pleasureModulation;        // Pleasure enhancement
    float rewardProcessing;           // Reward processing efficiency
    float hedonicResponse;            // Hedonic response level
    
    // Stress and homeostasis
    float stressReduction;           // Stress reduction level
    float homeostaticBalance;        // Homeostatic balance
    float energyRegulation;           // Energy regulation
    float recoveryRate;              // Recovery rate
    
    // Social bonding and attachment
    float socialBonding;              // Social bonding enhancement
    float attachmentFormation;        // Attachment formation
    float maternalBehavior;           // Maternal behavior modulation
    
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
    
    EndorphinsImpl() :
        level(0.0f), baseline(0.01f), peak(1.5f),
        decayRate(0.08f), releaseRate(0.4f), synthesisRate(0.02f),
        reuptakeRate(0.1f), receptorDensity(1.1f), bindingAffinity(0.85f),
        painModulation(1.0f), pleasureModulation(1.0f), rewardProcessing(1.0f),
        hedonicResponse(0.5f), stressReduction(1.0f), 
        homeostaticBalance(1.0f), energyRegulation(1.0f), recoveryRate(1.0f),
        socialBonding(1.0f), attachmentFormation(1.0f), maternalBehavior(1.0f),
        developmentalOffset(1.0f), maturationStage(1.0f),
        experienceModulation(1.0f), memoryTrace(0.0f), eligibilityTrace(0.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix for Endorphins
        // Endorphins are natural painkillers and mood enhancers
        interactionMatrix[0][0] = 1.0f;  // Endorphins-Endorphins self-interaction
        interactionMatrix[0][1] = 0.2f;  // Endorphins-5HT: excitation
        interactionMatrix[0][2] = 0.3f;  // Endorphins-NE: excitation
        interactionMatrix[0][3] = 0.3f;  // Endorphins-ACh: excitation
        interactionMatrix[0][4] = 0.2f;  // Endorphins-GABA: excitation
        interactionMatrix[0][5] = 0.1f;  // Endorphins-Glu: excitation
        
        // Other neuromodulators' effects on Endorphins
        interactionMatrix[1][0] = 0.3f;  // 5HT-Endorphins: excitation
        interactionMatrix[2][0] = 0.4f;  // NE-Endorphins: excitation
        interactionMatrix[3][0] = 0.3f;  // ACh-Endorphins: excitation
        interactionMatrix[4][0] = 0.2f;  // GABA-Endorphins: excitation
        interactionMatrix[5][0] = 0.2f;  // Glu-Endorphins: excitation
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

class Endorphins : public Neuromodulator {
public:
    Endorphins();
    ~Endorphins() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Endorphin-specific functions
    void modulatePainPerception(float painLevel, float intensity);
    void enhancePleasureProcessing(float rewardValue);
    void reduceStressAndAnxiety(float stressLevel);
    void activateRewardCircuits(float rewardPredictionError);
    void integratePredictionError(float predictionError);
    void enhanceHedonicResponse(float hedonicValence);
    void updateEndorphinDynamics(float dt);
    void encodeRewardMemory(float reward, float predictionError);
    void resetToBaseline(float recoveryRate, float dt);
    
    // State access
    float getPainModulation() const;
    float getPleasureModulation() const;
    float getStressModulation() const;
    float getRewardModulation() const;
    
    // Advanced features
    void setSocialBondingStrength(float bondingStrength);
    void updateMaternalBehavior(float maternalLevel);
    void enhanceRewardLearning(float rewardPrediction);
    void modulatePainThreshold(float thresholdLevel);
    void updateHomeostaticBalance(float balanceFactor);
    void enhanceStressResilience(float resilienceFactor);
    void modulateSexualBehavior(float sexualDrive);
    void updateAddictivePotential(float addictionPotential);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm