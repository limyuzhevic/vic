#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Norepinephrine: Arousal, attention, working memory modulation
// Phasic responses to novel or salient stimuli
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal level
    void setArousalLevel(float arousal);
    float getArousalLevel() const;
    
    // Locus coeruleus activation
    void activateLocusCoeruleus(float novelty, float salience, float predictionError);
    
    // Working memory enhancement
    float enhanceWorkingMemoryCapacity(float currentCapacity) const;
    
    // Focused attention modulation
    void applyFocusedAttention(float attentionStrength);
    
    // Behavioral flexibility enhancement
    void increaseBehavioralFlexibility();
    
    // Reset to baseline
    void reset() { pImpl->level = pImpl->baseline; pImpl->arousalLevel = 0.5f; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Memory encoding, attention gating, reinforcement learning
// Mediates between sensory input and cortical processing
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory encoding strength
    void enhanceMemoryEncoding(float encodingStrength);
    float getMemoryEncodingStrength() const;
    
    // Attention gating to sensory pathways
    void gateSensoryInput(NeuronId neuron, float gatingStrength);
    
    // Binding of distributed representations
    void facilitateFeatureBinding(const std::vector<NeuronId>& neuronIds);
    
    // Reward prediction error modulation
    void modulateRewardPredictionError(float predictionError);
    
    // Sleep-wake cycle modulation
    void enterSleepState();
    void enterWakeState();
    bool isAwake() const { return pImpl->isAwake; }
    
    // Update cholinergic tone based on behavioral state
    void updateCholinergicTone(bool isActive, float attentionLevel);
    
    // Reset to baseline
    void reset() { pImpl->level = pImpl->baseline; pImpl->isAwake = true; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Endorphins: Pain modulation, stress response, reward-reward balance
// Mediates stress adaptation and social bonding
class Endorphins : public Neuromodulator {
public:
    Endorphins();
    ~Endorphins() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Pain modulation (analgesic effect)
    void reducePain(float painLevel);
    void blockPainSignals(float blockStrength);
    
    // Stress adaptation
    void signalStress(float stressLevel);
    void facilitateStressRecovery();
    
    // Social bonding and reward balance
    void facilitateSocialBonding(float socialReward);
    void modulateRewardValue(float rewardValue);
    
    // Exercise-induced analgesia
    void registerExerciseActivity(float duration, float intensity);
    
    // Opioid receptor dynamics
    void releaseEndorphins(float magnitude);
    void bindToReceptors(float receptorAffinity);
    
    // Homeostatic balance restoration
    void restoreHomeostaticBalance();
    
    // Reset to baseline
    void reset() { pImpl->level = pImpl->baseline; pImpl->stressLevel = 0.0f; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Advanced Metaplasticity Manager
// Manages multiple metaplastic mechanisms simultaneously
class MetaplasticityManager {
public:
    MetaplasticityManager();
    ~MetaplasticityManager();
    
    void initialize(Brain* brain);
    
    // Learning rate homeostasis - prevents runaway excitation/inhibition
    void enforceLearningRateHomeostasis(float& learningRate) const;
    
    // History-dependent plasticity - adapts based on recent learning experience
    void applyHistoryDependentPlasticity(Synapse* synapse, float baseLearningRate, 
                                        const std::vector<float>& learningHistory) const;
    
    // Resource-limited plasticity - allocates plasticity budget across synapses
    bool allocatePlasticityBudget(float requiredBudget, const SynapseId& synapseId) const;
    void releasePlasticityBudget(float budgetReleased, const SynapseId& synapseId) const;
    float getRemainingPlasticityBudget() const;
    
    // Metaplastic thresholds - change how easily synapses can change
    void setMetaplasticThreshold(float threshold, const SynapseId& synapseId);
    float getMetaplasticThreshold(const SynapseId& synapseId) const;
    void updateThresholdBasedOnRecentLearning(const SynapseId& synapseId, float learningRate);
    
    // Synaptic tagging and capture
    void tagSynapseForLearning(const SynapseId& synapseId, float tagStrength);
    bool isSynapseTagged(const SynapseId& synapseId) const;
    float getTagStrength(const SynapseId& synapseId) const;
    void clearTag(const SynapseId& synapseId);
    
    // BCM-like rules - sliding threshold for LTP/LTD
    void setBCMThreshold(float thetaPlus, float thetaMinus);
    float getBCMThetaPlus() const { return pImpl->thetaPlus; }
    float getBCMThetaMinus() const { return pImpl->thetaMinus; }
    void updateBCMThreshold(float avgPostSynapticActivity);
    
    // Homeostatic plasticity - maintain stability
    void enforceHomeostaticPlasticity(float targetActivityLevel);
    void adjustSynapticWeightsToMaintainHomeostasis() const;
    
    // Integration with neuromodulators
    void integrateNeuromodulation(const Neuromodulator& modulator);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
