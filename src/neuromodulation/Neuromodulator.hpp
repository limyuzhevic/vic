#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Manages neuromodulatory signals that influence plasticity, attention, and learning
// These signals (dopamine, acetylcholine, etc.) modulate neural processing based on
// behavioral relevance, rewards, and environmental demands

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns scaling factor (1.0 = normal, >1 = enhanced, <1 = suppressed)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on neural activity
    virtual void update(TimestepDuration dt) = 0;
    
    // Receive neuromodulatory signals
    virtual void receiveSignal(float signal) = 0;
    
    // Reset to baseline
    virtual void reset() = 0;
    
    // Get baseline level for the neuromodulator
    virtual float getBaseline() const { return 0.0f; }
    
    // Get target level for homeostasis (optimal concentration)
    virtual float getTarget() const { return 0.5f; }
    
protected:
    Neuromodulator() = default;
};

// Neuromodulator implementations with encapsulated state and constants

// Dopamine: Reward and reinforcement learning signal
// Tracks reward prediction errors and reinforces adaptive behaviors
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
    
    // Get/Set dopamine concentration with validation
    void setTargetLevel(float target);
    float getTargetLevel() const;
    
    // Get/Set time constant for dopamine dynamics
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Enhances signal-to-noise ratio and promotes learning during novel stimuli
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Get/Set attention modulation strength
    void setAttentionGain(float gain);
    float getAttentionGain() const { return attentionGain_; }
    
    // Get/Set memory consolidation rate
    void setConsolidationRate(float rate);
    float getConsolidationRate() const { return consolidationRate_; }
    
private:
    float attentionGain_ = 1.0f;
        float consolidationRate_ = 0.1f;
};

// Norepinephrine: Arousal and vigilance
// Increases alertness and enhances encoding of salient information
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Get/Set arousal level
    void setArousalLevel(float level);
    float getArousalLevel() const { return arousalLevel_; }
    
    // Get/Set vigilance parameter
    void setVigilance(float vigilance);
    float getVigilance() const { return vigilance_; }
    
private:
    float arousalLevel_ = 1.0f;
    float vigilance_ = 0.5f;
};

// Serotonin: Mood, impulsivity, and social behavior
// Regulates mood states and social decision-making
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Get/Set mood baseline
    void setMoodBaseline(float baseline);
    float getMoodBaseline() const { return moodBaseline_; }
    
    // Get/Set impulsivity threshold
    void setImpulsivityThreshold(float threshold);
    float getImpulsivityThreshold() const { return impulsivityThreshold_; }
    
private:
    float moodBaseline_ = 0.5f;
    float impulsivityThreshold_ = 0.5f;
};

} // namespace nlm
