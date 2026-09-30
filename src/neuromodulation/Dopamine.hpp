#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Base implementation for neuromodulatory systems with advanced features
// Provides common interfaces for all neurotransmitters

struct NeuromodulatorImpl {
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
    
    // State tracking for dynamics
    float targetLevel;              // Desired/target level
    float timeSinceLastBurst;       // Time since last phasic event
    float burstProbability;         // Probability of burst generation
    float burstMagnitude;           // Average burst magnitude
    
    // Developmental trajectory
    float developmentalOffset;      // Developmental modulation
    float maturationStage;          // Current developmental stage
    float experienceModulation;     // Learning/modulation based on experience
    
    // Memory and prediction integration
    std::vector<float> history;     // Historical activity levels
    std::vector<float> predictionErrors; // Prediction error history
    float memoryTrace;              // Short-term memory trace
    float eligibilityTrace;         // Eligibility for plasticity
    
    // Interaction with other systems
    float interactionMatrix[6][6];  // Interaction with other neuromodulators (DA, 5HT, NE, ACh, GABA, GLU)
    std::vector<float> plasticityFactors; // Plasticity effects on synapses
    std::vector<float> neuralExcitability; // Effects on neural populations
    
    // Environmental context
    float contextSensitivity;       // Sensitivity to environmental context
    float stressLevel;              // Current stress level
    float arousalLevel;             // Current arousal level
    float attentionLevel;           // Current attention level
    
    NeuromodulatorImpl() : 
        level(0.0f), baseline(0.05f), peak(1.0f), 
        decayRate(0.1f), releaseRate(0.5f), synthesisRate(0.05f),
        reuptakeRate(0.2f), receptorDensity(1.0f), bindingAffinity(0.8f),
        targetLevel(0.0f), timeSinceLastBurst(0.0f), burstProbability(0.0f),
        burstMagnitude(0.0f), developmentalOffset(1.0f), maturationStage(1.0f),
        experienceModulation(1.0f), memoryTrace(0.0f), eligibilityTrace(0.0f),
        contextSensitivity(1.0f), stressLevel(0.0f), arousalLevel(0.5f), 
        attentionLevel(0.5f) {
        
        // Initialize interaction matrix with random-like values
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
            }
        }
    }
};

// Complete Dopamine system with all requested features
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    // Basic Neuromodulator interface
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Specialized dopamine methods
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void signalPhasicBurst(float burstGain);
    void modulateAdaptiveGain(float predictionError);
    void updateDevelopmentalBaseline(float ageFactor);
    void enhancePlasticityWithModulation(float factor);
    void encodeMemory(float reward, float predictionError);
    void updateTonicDynamics(float dt);
    void updatePhasicDynamics(float dt);
    void integratePredictionError(float predictionError);
    
    // Advanced features
    void setPredictionError(float error);
    float getPredictionError() const;
    void updateBurstProbability(float predictionError, float uncertainty);
    float calculateBurstMagnitude(float predictionError, float gainControl);
    void updateBaselineAdaptation(float dt);
    float getMemoryTrace() const;
    float getEligibilityTrace() const;
    void updateInteractionMatrix(const std::vector<float>& otherLevels);
    
    // Control and configuration
    void configure(float baseline, float peak, float decayRate, float releaseRate,
                   float synthesisRate, float reuptakeRate, float receptorDensity);
    void setDevelopmentalStage(float stage);
    void setExperienceModulation(float modulation);
    void setContextSensitivity(float sensitivity);
    void resetMemoryTraces();
    
    // State tracking
    float getBaseline() const;
    float getPeak() const;
    float getTimeSinceLastBurst() const;
    float getBurstProbability() const;
    float getMemoryStrength() const;
    
    // Utility methods
    void normalizeLevel();
    void decayToBaseline(float dt);
    void integrateInternalState(float externalInput, float dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin system for mood, anxiety, and arousal modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return pImpl->level; }
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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine system for attention, arousal, and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Norepinephrine-specific functions
    void enhanceAttention(float stimulus salience);
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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine system for memory consolidation, attention, and cortical activation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return pImpl->level; }
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
    
private:
    struct Impl;
    Impl* pImpl;
};

// GABA system for inhibitory control and anxiety regulation
class GABA : public Neuromodulator {
public:
    GABA();
    ~GABA() override;
    
    const char* getName() const override { return "GABA"; }
    float getLevel() const override { return pImpl->level; }
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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Glutamate system for excitatory signaling and long-term potentiation
class Glutamate : public Neuromodulator {
public:
    Glutamate();
    ~Glutamate() override;
    
    const char* getName() const override { return "Glu"; }
    float getLevel() const override { return pImpl->level; }
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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Endorphin system for pain modulation and reward processing
class Endorphins : public Neuromodulator {
public:
    Endorphins();
    ~Endorphins() override;
    
    const char* getName() const override { return "Endorphin"; }
    float getLevel() const override { return pImpl->level; }
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
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm