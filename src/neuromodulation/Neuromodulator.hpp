#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Forward declarations for new neuromodulators
class Serotonin;
class Norepinephrine;
class Acetylcholine;
class Endorphins;
class MetaplasticityManager;

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
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
    bool isAwake() const;
    
    // Update cholinergic tone based on behavioral state
    void updateCholinergicTone(bool isActive, float attentionLevel);
    
    // Reset to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
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
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, anxiety, learning rate modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood state modulation
    void setMoodState(float mood);
    float getMoodState() const;
    
    // Anxiety modulation
    void setAnxietyLevel(float anxiety);
    float getAnxietyLevel() const;
    
    // Learning rate modulation based on mood
    float getModulatedLearningRate(float baseRate) const;
    
    // Anhedonia (reduced reward response)
    void signalAnhedonia(float magnitude);
    
    // Facilitate extinction learning
    void facilitateExtinctionLearning();
    
    // Reset to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Endorphins: Pain, reward, stress response
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
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Advanced Metaplasticity Manager
class MetaplasticityManager {
public:
    MetaplasticityManager();
    ~MetaplasticityManager();
    
    void initialize(class Brain* brain);
    
    // Learning rate homeostasis - prevents runaway excitation/inhibition
    void enforceLearningRateHomeostasis(float& learningRate) const;
    
    // History-dependent plasticity - adapts based on recent learning experience
    void applyHistoryDependentPlasticity(class Synapse* synapse, float baseLearningRate, 
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
    float getBCMThetaPlus() const;
    float getBCMThetaMinus() const;
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
