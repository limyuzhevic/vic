#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 2: Real neuromodulation effects implemented

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
    
    // Apply neuromodulatory effect on memory consolidation
    virtual float getMemoryConsolidationFactor() const = 0;
    
    // Apply neuromodulatory effect on attention
    virtual float getAttentionModulationFactor() const = 0;
    
    // Apply neuromodulatory effect on arousal/excitability
    virtual float getArousalModulationFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Reset neuromodulator state
    virtual void reset() {}
    
protected:
    Neuromodulator() = default;
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
    float getMemoryConsolidationFactor() const override;
    float getAttentionModulationFactor() const override;
    float getArousalModulationFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Signal attention-driven learning
    void signalAttentionDrive(float strength);
    
    // Enhance memory encoding
    void enhanceEncoding(float novelty);
    
    // Modulate attention focus
    void adjustFocus(float focusLevel);
    
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
    float getMemoryConsolidationFactor() const override;
    float getAttentionModulationFactor() const override;
    float getArousalModulationFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Signal increased arousal
    void signalArousalIncrease(float amount);
    
    // Enhance signal detection
    void enhanceSensoryGain(float gain);
    
    // Modulate vigilance
    void adjustVigilance(float level);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getMemoryConsolidationFactor() const override;
    float getAttentionModulationFactor() const override;
    float getArousalModulationFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Signal mood state
    void signalMoodState(float moodLevel);
    
    // Modulate impulsivity
    void adjustImpulsivity(float level);
    
    // Enhance social learning
    void facilitateSocialLearning(float amount);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Dopamine: Reward and reinforcement learning signal
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getMemoryConsolidationFactor() const override;
    float getAttentionModulationFactor() const override;
    float getArousalModulationFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Enhance reward-based learning
    void strengthenRewardPathways(float strength);
    
    // Modulate motivation
    void adjustMotivation(float level);
    
private:
    struct Impl;
    Impl* pImpl;
};
