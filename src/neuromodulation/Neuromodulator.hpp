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
    
    // TD learning and eligibility traces
    void recordPostSynapticActivity(NeuronId neuronId, float activity);
    void updateEligibilityTrace(NeuronId neuronId, TimestepDuration dt);
    void computeTDLearningError(float expectedReward, float actualReward);
    
    // Memory consolidation
    void consolidateToMemory(float importanceThreshold);
    
    // Phasic dynamics
    void triggerBurst(float intensity, TimestepDuration duration);
    bool isBursting() const;
    
    // Recovery dynamics
    void startRecoveryPhase(float recoveryRate);
    void resetToBaseline();
    
    // Getters for advanced dynamics
    float getTDLearningError() const;
    float getEligibilityTraceSum() const;
    float getBaselineLevel() const;
    float getBurstStrength() const;
    
    // Configuration
    void setTDAlpha(float alpha);
    void setElearnCoefficient(float coeff);
    void setBurstThreshold(float threshold);
    void setRecoveryTime(float time);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void increaseAttention(float attentionGain);
    void decreaseAttention(float attentionDecay);
    void switchAttention(NeuronId sourceNeuron, NeuronId targetNeuron);
    
    // Memory consolidation
    void consolidateToLongTermMemory(const std::vector<float>& pattern);
    void updateMemoryTrace(NeuronId neuronId, float activity);
    float getMemoryConsolidationStrength() const;
    
    // Attentional selection
    void processCompetition(const std::vector<NeuronId>& competitors);
    NeuronId getCurrentFocus() const;
    float getAttentionLevel() const;
    
    // Cholinergic dynamics
    void setBaselineLevel(float baseline);
    void setNoiseLevel(float noise);
    void setReverberationTime(float time);
    
    // Getters
    float getNoiseLevel() const;
    float getReverberationTime() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance
    void increaseArousal(float arousalLevel);
    void decreaseArousal(float decayRate);
    void detectNovelty(float noveltyScore);
    void updateLocusCoeruleanFiring(float externalNovelty, float internalPredictability);
    
    // Attention switching
    void enableAttentionSwitching(float switchProbability);
    NeuronId selectNextFocus(const std::vector<NeuronId>& availableNeurons);
    
    // Exploration bias
    void promoteExploration(float explorationBias);
    void promoteExploitation(float exploitationBias);
    
    // NE dynamics
    void setBaselineFiring(float baseline);
    void setFiringVariability(float variability);
    void setAdaptationRate(float rate);
    
    // Getters
    float getBaselineFiring() const;
    float getFiringVariability() const;
    float getAdaptationRate() const;
    bool isArousalHigh() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation
    void modulateMood(float moodLevel);
    void respondToNegativeOutcome(float outcomeSeverity);
    void recoverFromNegativeState(float recoveryRate);
    
    // Behavioral inhibition
    void inhibitImpulsiveActions(float inhibitionStrength);
    void enableGoalDirectedBehavior(float goalValue);
    bool isActionInhibited() const;
    
    // Social behavior
    void modulateSocialBonding(float bondingLevel);
    void respondToSocialStatus(float statusChange);
    float getSocialCohesion() const;
    
    // 5-HT dynamics
    void setBaselineMood(float baseline);
    void setRecoveryRate(float rate);
    void setInhibitionThreshold(float threshold);
    
    // Getters
    float getBaselineMood() const;
    float getRecoveryRate() const;
    float getInhibitionThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
