#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced Phase 6 implementation with advanced features

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Real neuromodulator-enhanced plasticity factor calculations
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state with advanced dynamics
    virtual void update(TimestepDuration dt) = 0;
    
    // Integration with neural excitability
    virtual void applyToNeuralExcitability(float& excitability) const = 0;
    
    // Integration with plasticity rules
    virtual void applyToPlasticityRule(float& plasticity) const = 0;
    
protected:
    Neuromodulator() = default;
};

// Complete Dopamine system with all requested features
// Advanced reward and reinforcement learning with prediction errors,
// adaptive gain control, developmental modulation, and enhanced plasticity
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Phasic burst responses to prediction errors
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void signalPhasicBurst(float burstGain);
    
    // Adaptive gain control and burst probability calculations
    void modulateAdaptiveGain(float predictionError);
    void updateBurstProbability(float predictionError, float uncertainty);
    float calculateBurstMagnitude(float predictionError, float gainControl);
    
    // Developmental modulation with baseline adaptation
    void updateDevelopmentalBaseline(float ageFactor);
    void setDevelopmentalStage(float stage);
    
    // Enhanced plasticity factor calculations with neuromodulatory influence
    void enhancePlasticityWithModulation(float factor);
    
    // Realistic reward signaling with memory encoding
    void encodeMemory(float reward, float predictionError);
    
    // Dopamine dynamics (phasic and tonic components)
    void updateTonicDynamics(float dt);
    void updatePhasicDynamics(float dt);
    
    // Prediction error integration
    void integratePredictionError(float predictionError);
    void setPredictionError(float error);
    float getPredictionError() const;
    
    // Memory and eligibility traces
    float getMemoryTrace() const;
    float getEligibilityTrace() const;
    void updateBaselineAdaptation(float dt);
    
    // Neuromodulator-receptor interactions and synthesis
    void updateInteractionMatrix(const std::vector<float>& otherLevels);
    void configure(float baseline, float peak, float decayRate, float releaseRate,
                   float synthesisRate, float reuptakeRate, float receptorDensity);
    
    // State access
    float getBaseline() const;
    float getPeak() const;
    float getTimeSinceLastBurst() const;
    float getBurstProbability() const;
    float getMemoryStrength() const;
    
    // Utility methods
    void normalizeLevel();
    void decayToBaseline(float dt);
    void integrateInternalState(float externalInput, float dt);
    
    void applyToNeuralExcitability(float& excitability) const override;
    void applyToPlasticityRule(float& plasticity) const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood regulation, anxiety, and arousal modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation and affective processing
    void regulateMood(float moodState);
    void modulateAnxiety(float anxietyLevel);
    void controlArousal(float arousalTarget);
    void enhanceSocialBehavior(float socialContext);
    
    // Prediction error integration and emotional memory
    void integratePredictionError(float predictionError);
    void updateDevelopmentalTrajectory(float age);
    void encodeEmotionalMemory(float emotion, float strength);
    void applyStressResponse(float stressLevel, float duration);
    
    // Advanced features
    void setEmotionalState(float state);
    void updateImpulseControl(float cognitiveLoad);
    void enhanceSatietyResponse(float foodIntake);
    void modulateAggression(float provocationLevel);
    void updateNeurogenesis(float neurogenesisRate);
    
    // State access
    float getMoodModulation() const;
    float getAnxietyModulation() const;
    float getArousalModulation() const;
    float getSocialModulation() const;
    
    void applyToNeuralExcitability(float& excitability) const override;
    void applyToPlasticityRule(float& plasticity) const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Attention, arousal, and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention and arousal modulation
    void enhanceAttention(float stimulusSalience);
    void increaseArousal(float level, float rate);
    void activateFightOrFlightResponse(float threatLevel, float urgency);
    void modulateStressResponse(float stressInput, float adaptation);
    
    // Cognitive processing and prediction integration
    void integratePredictionError(float predictionError);
    void enhanceCognitiveProcessing(float taskDemand);
    void updateLocusCoeruleusDynamics(float dt);
    void encodeAttentionMemory(float focus, float duration);
    
    // Advanced features
    void setVigilanceLevel(float level);
    void updateThreateningInput(float threatLevel);
    void enhanceMemoryEncoding(float learningRate);
    void modulateResponseSelection(float competition);
    void updateAdaptiveAllocation(float resourceAvailability);
    
    // State access
    float getAttentionModulation() const;
    float getArousalModulation() const;
    float getStressModulation() const;
    float getCognitiveModulation() const;
    
    void applyToNeuralExcitability(float& excitability) const override;
    void applyToPlasticityRule(float& plasticity) const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Memory consolidation, attention, and cortical activation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory and attention modulation
    void consolidateMemory(float memoryStrength, float age);
    void enhanceAttentionSpan(float taskComplexity);
    void activateCorticalAreas(float targetActivation);
    void modulateSensoryProcessing(float stimulusIntensity);
    
    // Learning rate enhancement and prediction integration
    void integratePredictionError(float predictionError);
    void enhanceLearningRate(float novelty, float uncertainty);
    void updateCholinergicDynamics(float dt);
    void encodeWorkingMemory(float content, float relevance);
    
    // Advanced features
    void setNoveltyDetection(float detectionThreshold);
    void updatePatternSeparation(float patternSimilarity);
    void enhanceTemporalProcessing(float temporalPrecision);
    void modulateInhibitoryControl(float inhibitionLevel);
    void updateReceptiveFieldExpansion(float expansionRate);
    
    // State access
    float getMemoryModulation() const;
    float getAttentionModulation() const;
    float getCorticalModulation() const;
    float getLearningModulation() const;
    
    void applyToNeuralExcitability(float& excitability) const override;
    void applyToPlasticityRule(float& plasticity) const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// GABA: Inhibitory control and anxiety regulation
class GABA : public Neuromodulator {
public:
    GABA();
    ~GABA() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Inhibitory control and anxiety regulation
    void inhibitExcitation(float excitationLevel);
    void regulateAnxiety(float anxietyState);
    void controlImpulsiveBehavior(float impulseStrength);
    void modulateFearResponse(float fearLevel, float context);
    
    // Prediction integration and plasticity enhancement
    void integratePredictionError(float predictionError);
    void enhanceInhibitoryPlasticity(float learningRate);
    void updateInhibitoryDynamics(float dt);
    void encodeInhibitionMemory(float inhibition, float duration);
    
    // Advanced features
    void setFearConditioning(float conditioningStrength);
    void updateThreateningInput(float threatLevel);
    void enhanceConflictResolution(float conflictLevel);
    void modulateSocialBehavior(float socialContext);
    void updateStressResilience(float resilienceFactor);
    void enhanceEmotionalRegulation(float emotionalStability);
    
    // State access
    float getInhibitionModulation() const;
    float getAnxietyModulation() const;
    float getImpulseControlModulation() const;
    float getFearModulation() const;
    
    void applyToNeuralExcitability(float& excitability) const override;
    void applyToPlasticityRule(float& plasticity) const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Glutamate: Excitatory signaling and long-term potentiation
class Glutamate : public Neuromodulator {
public:
    Glutamate();
    ~Glutamate() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Excitatory signaling and LTP/LTD
    void enhanceExcitatorySignaling(float synapticStrength);
    void facilitateLongTermPotentiation(float learningRate, float predictionError);
    void activateCorticalExcitability(float neuralActivation);
    void modulateSensoryExcitability(float stimulusIntensity);
    
    // Plasticity enhancement and prediction integration
    void integratePredictionError(float predictionError);
    void enhanceSynapticPlasticity(float eligibilityTrace);
    void updateExcitatoryDynamics(float dt);
    void encodeLTPMemory(float potentiation, float duration);
    
    // Advanced features
    void setSensoryGain(float gainLevel);
    void updateTemporalIntegration(float timeWindow);
    void enhancePatternSeparation(float patternDistance);
    void modulateStructuralPlasticity(float structuralChange);
    void updateHebbianLearning(float correlation);
    void enhanceMetaplasticity(float metaplasticState);
    
    // State access
    float getExcitabilityModulation() const;
    float getLTPModulation() const;
    float getSynapticModulation() const;
    float getLearningModulation() const;
    
    void applyToNeuralExcitability(float& excitability) const override;
    void applyToPlasticityRule(float& plasticity) const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Endorphins: Pain modulation and reward processing
class Endorphins : public Neuromodulator {
public:
    Endorphins();
    ~Endorphins() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Pain and reward modulation
    void modulatePainPerception(float painLevel, float intensity);
    void enhancePleasureProcessing(float rewardValue);
    void reduceStressAndAnxiety(float stressLevel);
    void activateRewardCircuits(float rewardPredictionError);
    
    // Prediction integration and hedonic enhancement
    void integratePredictionError(float predictionError);
    void enhanceHedonicResponse(float hedonicValence);
    void updateEndorphinDynamics(float dt);
    void encodeRewardMemory(float reward, float predictionError);
    
    // Advanced features
    void setSocialBondingStrength(float bondingStrength);
    void updateMaternalBehavior(float maternalLevel);
    void enhanceRewardLearning(float rewardPrediction);
    void modulatePainThreshold(float thresholdLevel);
    void updateHomeostaticBalance(float balanceFactor);
    void enhanceStressResilience(float resilienceFactor);
    void modulateSexualBehavior(float sexualDrive);
    void updateAddictivePotential(float addictionPotential);
    
    // State access
    float getPainModulation() const;
    float getPleasureModulation() const;
    float getStressModulation() const;
    float getRewardModulation() const;
    
    void applyToNeuralExcitability(float& excitability) const override;
    void applyToPlasticityRule(float& plasticity) const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm