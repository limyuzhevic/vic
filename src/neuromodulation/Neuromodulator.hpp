#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Base class for all neuromodulators with real biological dynamics

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns modulation factor (1.0 = baseline, >1.0 = enhances, <1.0 = suppresses)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on inputs and time
    virtual void update(TimestepDuration dt) = 0;
    
    // Get adaptive response strength based on recent activity
    virtual float getAdaptiveResponse() const { return 1.0f; }
    
    // Reset neuromodulator to baseline state
    virtual void reset() {}
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward prediction error signal for reinforcement learning
// Implements real dopaminergic dynamics with burst-pause firing pattern
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling - increases DA level based on received reward
    void signalReward(float reward);
    
    // Reward prediction error signaling - positive for better-than-expected, negative for worse
    void signalRewardPredictionError(float error);
    
    // Get recent reward history
    float getRecentReward() const;
    
    // Set baseline dopamine level
    void setBaseline(float baseline);
    
    // Set learning rate modulation
    void setLearningRate(float rate);
    
    // Reset dopamine to resting state
    void reset() override;
    
    // Get prediction error magnitude
    float getPredictionError() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation modulator
// Enhances signal-to-noise ratio and supports working memory
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation - increases based on task relevance
    void signalAttention(float attentionStrength);
    
    // Memory modulation - enhances consolidation during awake state
    void signalMemoryEvent(float memoryStrength);
    
    // Get attention level
    float getAttentionLevel() const;
    
    // Get memory modulation factor
    float getMemoryModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance modulator
// Regulates arousal, attention, and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal signaling - increases with stress, novelty, and effort
    void signalArousal(float arousalLevel);
    
    // Vigilance modulation - enhances focus and alertness
    void signalVigilance(float vigilanceStrength);
    
    // Get arousal level
    float getArousalLevel() const;
    
    // Get stress level
    float getStressLevel() const;
    
    // Set stress sensitivity
    void setStressSensitivity(float sensitivity);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior modulator
// Regulates mood, impulse control, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood modulation - affects emotional state and motivation
    void signalMoodChange(float moodLevel);
    
    // Impulsivity control - reduces with higher serotonin
    void signalImpulseControl(float impulseStrength);
    
    // Social behavior modulation
    void signalSocialBehavior(float socialStrength);
    
    // Get mood level
    float getMoodLevel() const;
    
    // Get impulsivity level
    float getImpulsivityLevel() const;
    
    // Set serotonin receptor sensitivity
    void setReceptorSensitivity(float sensitivity);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
