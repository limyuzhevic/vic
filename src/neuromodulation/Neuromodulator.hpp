#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Interface for all neuromodulators implementing biological mechanisms

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Modulates synaptic plasticity for learning
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state (called each simulation step)
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Implements reward prediction error and reinforcement learning
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get error magnitude
    float getError() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

// Acetylcholine: Attention and memory consolidation
// Enhances attention, working memory, and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Biological mechanisms
    void enhanceAttention(float stimulusStrength);
    void initiateMemoryConsolidation();
    void modulateWorkingMemory();
    void signalAttentionShift();
    void decayMemoryTrace();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Regulates arousal, alertness, and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Biological mechanisms
    void increaseArousal(float stimulusIntensity);
    void maintainVigilance(float duration);
    void respondToStress(float stressLevel);
    void enhanceSensoryProcessing();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Regulates mood, aggression, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Biological mechanisms
    void improveMood(float positiveInput);
    void reduceImpulsivity();
    void modulateSocialBehavior(float socialContext);
    void regulateAggression(float threatLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
