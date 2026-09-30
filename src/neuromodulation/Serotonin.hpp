#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <cmath>
#include <random>

namespace nlm {

struct SerotoninImpl {
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
    
    // Mood and affective state
    float moodModulation;            // Mood enhancement/suppression
    float anxietyModulation;        // Anxiety regulation
    float arousalModulation;        // Arousal level
    float socialModulation;          // Social behavior modulation
    float impulsivityModulation;     // Impulsivity control
    
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
    
    // Stress and emotional state
    float stressLevel;              // Current stress level
    float emotionalState;            // Current emotional state
    float rewardSensitivity;         // Sensitivity to reward
    
    // Random generator for probabilistic responses
    std::mt19937 rng;
    std::uniform_real_distribution<float> responseDist;
    
    SerotoninImpl() :
        level(0.0f), baseline(0.03f), peak(1.0f),
        decayRate(0.08f), releaseRate(0.3f), synthesisRate(0.04f),
        reuptakeRate(0.3f), receptorDensity(1.2f), bindingAffinity(0.9f),
        moodModulation(1.0f), anxietyModulation(1.0f), arousalModulation(0.5f),
        socialModulation(1.0f), impulsivityModulation(1.0f),
        developmentalOffset(1.0f), maturationStage(1.0f),
        experienceModulation(1.0f), memoryTrace(0.0f), eligibilityTrace(0.0f),
        stressLevel(0.0f), emotionalState(0.5f), rewardSensitivity(1.0f),
        rng(std::random_device()()), responseDist(0.0f, 1.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix for 5-HT
        // 5-HT regulates mood, reduces anxiety, increases satiety
        interactionMatrix[0][0] = 1.0f;  // 5-HT-5-HT self-interaction
        interactionMatrix[0][1] = -0.5f; // 5-HT-DA: inhibition (reduces approach)
        interactionMatrix[0][2] = -0.3f; // 5-HT-NE: inhibition (reduces arousal)
        interactionMatrix[0][3] = 0.2f;  // 5-HT-ACh: excitation
        interactionMatrix[0][4] = 0.4f;  // 5-HT-GABA: excitation (inhibits excitation)
        interactionMatrix[0][5] = -0.2f; // 5-HT-Glu: inhibition
        
        // Other neuromodulators' effects on 5-HT
        interactionMatrix[1][0] = 0.5f;  // DA-5-HT: excitation
        interactionMatrix[2][0] = 0.3f;  // NE-5-HT: excitation
        interactionMatrix[3][0] = -0.2f; // ACh-5-HT: inhibition
        interactionMatrix[4][0] = 0.2f;  // GABA-5-HT: excitation
        interactionMatrix[5][0] = -0.3f; // Glu-5-HT: inhibition
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Serotonin-specific functions
    void regulateMood(float moodState);
    void modulateAnxiety(float anxietyLevel);
    void controlArousal(float arousalTarget);
    void enhanceSocialBehavior(float socialContext);
    void integratePredictionError(float predictionError);
    void updateDevelopmentalTrajectory(float age);
    void encodeEmotionalMemory(float emotion, float strength);
    void applyStressResponse(float stressLevel, float duration);
    void resetToBaseline(float recoveryRate, float dt);
    
    // State access
    float getMoodModulation() const;
    float getAnxietyModulation() const;
    float getArousalModulation() const;
    float getSocialModulation() const;
    
    // Advanced features
    void setEmotionalState(float state);
    void updateImpulseControl(float cognitiveLoad);
    void enhanceSatietyResponse(float foodIntake);
    void modulateAggression(float provocationLevel);
    void updateNeurogenesis(float neurogenesisRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm