#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Complete implementation for Phase 6

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Complete implementation with full dynamics
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
    
    // Reward-modulated STDP
    void updateSTDPWithEligibility(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Complete implementation with attention modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void enhanceAttention(float focusLevel, Brain* brain);
    void modulateWorkingMemory(Brain* brain);
    
    // Memory encoding
    void enhanceEncoding(float novelty, Brain* brain);
    void strengthenSynapticTrace(Brain* brain, NeuronId neuronId);
    
    // Signal processing
    void signalNovelty(float novelty);
    void respondToSensoryInput(Brain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Complete implementation with stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal regulation
    void enhanceSensoryProcessing(float intensity, Brain* brain);
    void modulateARResponse(Brain* brain);
    
    // Stress response
    void signalStress(float stressMagnitude, float duration);
    void enhanceAlertness(Brain* brain);
    
    // Vigilance
    float getVigilance() const;
    float getArousal() const;
    
    // Plasticity modulation
    void modulateSTDPWindows(Brain* brain, Synapse* synapse);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Complete implementation with behavioral inhibition
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Behavioral inhibition
    void enhanceBehavioralInhibition(float contextFactor);
    void modulateDecisionMaking(Brain* brain);
    void enhanceSelfControl(Brain* brain);
    
    // Mood regulation
    float getMood() const;
    void signalReward(float reward, bool isExpected);
    void respondToSocialInteraction(float socialValue, Brain* brain);
    
    // Social behavior
    void enhanceSocialBonding(float socialValue);
    void modulateSocialLearning(Brain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
