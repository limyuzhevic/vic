#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced implementation with real effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Enhanced: Different modulators affect different aspects
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Enhanced: Additional neuromodulatory effects
    virtual float getAttentionModulator() const { return 1.0f; } // ACh effect on attention
    virtual float getArousalModulator() const { return 1.0f; }  // NE effect on arousal
    virtual float getLearningModulator() const { return 1.0f; } // 5-HT effect on learning
    virtual float getMemoryModulator() const { return 1.0f; }   // DA effect on memory
    
protected:
    Neuromodulator() = default;
};

// Acetylcholine: Attention and memory consolidation
// Enhanced implementation with real effects
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced: Real attention modulation
    float getAttentionModulator() const override;
    
    // Boost attention and working memory
    void boostAttention(float duration);
    void enhanceMemoryConsolidation();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Enhanced implementation with real effects
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced: Real arousal modulation
    float getArousalModulator() const override;
    
    // Enhance alertness and sensory processing
    void enhanceAlertness(float duration);
    void amplifySensoryGain();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced implementation with real effects
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced: Real learning modulation
    float getLearningModulator() const override;
    
    // Modulate mood and learning rate
    void modulateMood(float moodLevel);
    void adjustLearningRate();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Dopamine: Reward and reinforcement learning signal
// Enhanced implementation with more sophisticated effects
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced: Multiple dopamine effects
    float getLearningModulator() const override;
    float getMemoryModulator() const override;
    
    // More sophisticated reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void modulateMotivation(float motivation);
    void enhanceWorkingMemory(float boost);
    void scheduleFutureActions(float planningHorizon);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
