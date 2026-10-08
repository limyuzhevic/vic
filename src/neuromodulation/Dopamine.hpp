#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine: Enhanced with VTA neuron dynamics and RPE signaling
class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getConcentration() const;
    float getBaselineConcentration() const;
    float getHomeostaticSetPoint() const;
    bool isHomeostaticallyRegulated() const;
    void resetToBaseline();
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Reward signaling with burst/phasic dynamics
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // VTA neuron dynamics
    void updateVTAActivity(float sensoryInput, float expectation, float actualOutcome);
    void processBurstSignal(float burstIntensity, float burstDuration);
    void processPhasicSignal(float phasicAmplitude, float phasicTiming);
    
    // DA-D1 and DA-D2 receptor effects
    float getD1ReceptorActivation() const;
    float getD2ReceptorActivation() const;
    void setD1ReceptorActivation(float activation);
    void setD2ReceptorActivation(float activation);
    
    // Synaptic dynamics
    void updateReleaseProbability(float probability);
    void updateUptakeRate(float rate);
    void setUptakeRate(float rate);
    
    // Homeostatic regulation
    void setHomeostaticSetPoint(float setpoint);
    void setHomeostaticStrength(float strength);
    void setActivityDependentRelease(bool enabled);
    
    // Cross-modulation with other neuromodulators
    void modulateWithAcetylcholine(float achLevel);
    void modulateWithNorepinephrine(float neLevel);
    void modulateWithSerotonin(float fiveHtLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Enhanced with cholinergic dynamics and attention modulation
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getConcentration() const;
    float getBaselineConcentration() const;
    float getHomeostaticSetPoint() const;
    bool isHomeostaticallyRegulated() const;
    void resetToBaseline();
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Cholinergic dynamics
    void updateAChDynamics(float corticalActivity, float attentionalDemand);
    void modulateSensoryGain(float baselineGain, float cholinergicInfluence);
    
    // Memory consolidation mechanisms
    void triggerMemoryConsolidation(float noveltySignal, float consolidationStrength);
    void updateWorkingMemoryTrace(float memoryTrace, float cholinergicModulation);
    
    // Ventral tegmental area projections
    void updateVTAChProjections(float dopamineLevel, float rewardPredictionError);
    void modulatePrefrontalCortex(float pfActivity, float attentionalFocus);
    
    // Cross-modulation
    void modulateWithDopamine(float daLevel);
    void modulateWithNorepinephrine(float neLevel);
    void modulateWithSerotonin(float fiveHtLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Enhanced with locus coeruleus firing rate dynamics
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getConcentration() const;
    float getBaselineConcentration() const;
    float getHomeostaticSetPoint() const;
    bool isHomeostaticallyRegulated() const;
    void resetToBaseline();
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Locus coeruleus firing dynamics
    void updateLocusCoeruleusFiring(float arousal, float novelty, float stressLevel);
    void modulateSignalToNoiseRatio(float baselineSNR, float neInfluence);
    
    // Arousal-dependent gain modulation
    void updateArousalState(float environmentalStimuli, float internalState);
    void modulateAttentionalFocus(float attentionThreshold, float neModulation);
    
    // Stress response and homeostasis
    void triggerStressResponse(float stressorIntensity, float duration);
    void updateStressRecovery(float recoveryRate);
    void setStressSetPoint(float setpoint);
    
    // NE-dependent plasticity
    void modulateSynapticPlasticity(float baselinePlasticity, float neInfluence);
    void updateReceptorSensitivity(float sensitivity, float adaptationRate);
    
    // Cross-modulation
    void modulateWithDopamine(float daLevel);
    void modulateWithAcetylcholine(float achLevel);
    void modulateWithSerotonin(float fiveHtLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Enhanced with raphe nucleus dynamics and mood regulation
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getConcentration() const;
    float getBaselineConcentration() const;
    float getHomeostaticSetPoint() const;
    bool isHomeostaticallyRegulated() const;
    void resetToBaseline();
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Raphe nucleus serotonin dynamics
    void updateRapheDynamics(float moodState, float behavioralDemand);
    void modulateMoodRegulation(float baselineMood, float fiveHtInfluence);
    
    // Behavioral inhibition and punishment learning
    void processPunishmentSignal(float punishmentIntensity, float aversiveLearning);
    void updateAvoidanceLearning(float threatLevel, float avoidanceStrength);
    
    // Exploration vs exploitation strategy switching
    void updateStrategySwitching(float explorationDrive, float exploitationValue);
    void modulateDecisionMaking(float choiceUncertainty, float fiveHtModulation);
    
    // 5-HT receptor subtypes
    float get5HT1AReceptorActivation() const;
    float get5HT2AReceptorActivation() const;
    void set5HT1AReceptorActivation(float activation);
    void set5HT2AReceptorActivation(float activation);
    
    // Cross-modulation
    void modulateWithDopamine(float daLevel);
    void modulateWithAcetylcholine(float achLevel);
    void modulateWithNorepinephrine(float neLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm