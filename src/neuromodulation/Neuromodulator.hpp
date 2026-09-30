#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <map>

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

// Enhanced Dopamine: Comprehensive neuromodulation effects
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward and prediction error signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Enhanced dopamine effects
    // Synaptic plasticity beyond basic scaling
    float getEnhancedPlasticity() const;
    float getMetaPlasticity() const;
    
    // Working memory stability
    float getWorkingMemoryStability() const;
    void setWorkingMemoryRecency(float recency);
    
    // Attention and selection
    float getAttentionGain() const;
    float getSelectionPriority() const;
    void setAttentionContext(const std::string& context);
    
    // Prediction error learning
    float getPredictionError() const;
    void setPredictionError(float error);
    
    // Sensory and motor gating
    float getSensoryGain() const;
    float getMotorGain() const;
    void setSensoryFilter(float filter);
    void setMotorCommandFilter(float filter);
    
    // Cross-system integration
    void integrateWithWorkingMemory(float stability, float recency);
    void integrateWithPredictionSystem(float error);
    void integrateWithPlanningSystem(float value);
    void integrateWithEpisodicMemory(float strength);
    void integrateWithConceptFormation(float novelty);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced Acetylcholine: Memory and attention control
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Working memory encoding and retrieval
    float getEncodingStrength() const;
    float getRetrievalStrength() const;
    void setEncodingTarget(const std::string& target);
    void setRetrievalTarget(const std::string& target);
    
    // Attention and feature binding
    float getAttentionFocus() const;
    float getFeatureBindingStrength() const;
    void setFeatureSet(const std::vector<std::string>& features);
    
    // Novel experience detection
    bool isNovelExperience(float noveltyValue) const;
    float getNoveltyThreshold() const;
    void setNoveltyThreshold(float threshold);
    
    // Memory consolidation
    void consolidateWorkingMemory();
    void consolidateEpisodicMemory(float strength);
    float getConsolidationStrength() const;
    
    // Synaptic plasticity
    float getAChPlasticityModulation() const;
    void setSynapticTarget(NeuronId target, float weight);
    
    // Cross-system integration
    void integrateWithWorkingMemory(float encoding, float retrieval);
    void integrateWithEpisodicMemory(float consolidation);
    void integrateWithAttentionalSelection(float focus);
    void integrateWithSensoryProcessing(float binding);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and signal-to-noise ratio
    float getArousalLevel() const;
    float getSignalToNoiseRatio() const;
    void setArousalThreshold(float threshold);
    void setNoiseLevel(float noise);
    
    // Attention and vigilance
    float getVigilanceLevel() const;
    float getAttentionSelectiveGain() const;
    void setVigilanceTarget(float target);
    void setAttentionalFocus(const std::string& focus);
    
    // Memory strengthening
    void strengthenMemoryTrace(float traceId, float strength);
    float getMemoryStrength(float traceId) const;
    void setMemoryDecayRate(float decayRate);
    
    // Stress response
    bool isStressResponse(float stressor) const;
    float getStressLevel() const;
    void triggerStressResponse(float intensity, const std::string& context);
    
    // Learning and adaptation
    float getLearningRate() const;
    float getAdaptationRate() const;
    void setLearningRate(float rate);
    void updateAdaptation(const std::vector<float>& predictionErrors);
    
    // Cross-system integration
    void integrateWithArousalSystems(float arousal);
    void integrateWithMemorySystems(float strength);
    void integrateWithAttentionalSystems(float vigilance);
    void integrateWithLearningSystems(float adaptation);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced Serotonin: Mood, decision making, and flexibility
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and motivation
    float getMoodLevel() const;
    float getMotivationLevel() const;
    void setMoodTarget(float target);
    void setMotivationTarget(float target);
    
    // Decision making
    float getDecisionConfidence() const;
    float getRiskAversion() const;
    float getExplorationExploitationBalance() const;
    void setDecisionContext(const std::string& context);
    
    // Behavioral flexibility
    bool isBehavioralFlexibilityRequired(float constraint) const;
    float getFlexibilityLevel() const;
    void setFlexibilityTarget(float target);
    void recordBehaviorChange(const std::string& oldBehavior, const std::string& newBehavior);
    
    // Learning rate modulation
    float getLearningRateModulation() const;
    float getHabituationRate() const;
    void setLearningRateModulation(float modulation);
    void updateLearningRate(float experience, float outcome);
    
    // Punishment and avoidance
    float getPunishmentSensitivity() const;
    float getAvoidanceMotivations() const;
    void triggerPunishment(float intensity, const std::string& context);
    void setAvoidancePriority(const std::string& goal, float priority);
    
    // Cross-system integration
    void integrateWithMoodSystems(float mood);
    void integrateWithDecisionSystems(float confidence);
    void integrateWithBehavioralSystems(float flexibility);
    void integrateWithLearningSystems(float modulation);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
