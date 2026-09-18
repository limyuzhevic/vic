#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Base class for all neuromodulatory signals affecting brain plasticity and dynamics

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0 normalized)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns a factor that multiplies plasticity rates
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on time
    virtual void update(TimestepDuration dt) = 0;
    
    // Signal an event that should affect this neuromodulator
    virtual void signalEvent(float magnitude = 1.0f) = 0;
    
    // Reset neuromodulator to baseline level
    virtual void reset() = 0;
    
    // Get baseline level (resting concentration)
    virtual float getBaselineLevel() const = 0;
    
    // Get peak level (maximum concentration)
    virtual float getPeakLevel() const = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// DA encodes reward prediction error and salience
// Drives motivation, reward learning, and action selection
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(float magnitude = 1.0f) override;
    void reset() override;
    float getBaselineLevel() const override;
    float getPeakLevel() const override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void signalSalience(float salience);
    
    // Get reward-related statistics
    float getTotalReward() const;
    float getPredictionError() const;
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// ACh regulates attention, arousal, and working memory encoding
// Mediates attention switching and memory consolidation during sleep
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(float magnitude = 1.0f) override;
    void reset() override;
    float getBaselineLevel() const override;
    float getPeakLevel() const override;
    
    // Attention modulation
    void signalAttention(float focusLevel);
    void signalTaskSwitching(float switchCost);
    
    // Memory consolidation
    void triggerMemoryConsolidation();
    
    // Get attention-related statistics
    float getAttentionLevel() const;
    float getWMEncodingEfficiency() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// NE regulates arousal, vigilance, and stress response
// Modulates signal-to-noise ratio and learning rate
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(float magnitude = 1.0f) override;
    void reset() override;
    float getBaselineLevel() const override;
    float getPeakLevel() const override;
    
    // Arousal modulation
    void signalStress(float stressLevel);
    void signalNovelty(float noveltyLevel);
    
    // Cognitive enhancement
    void enhanceSignalToNoiseRatio(float enhancement);
    void modulateLearningRate(float rateChange);
    
    // Get arousal-related statistics
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getStressLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// 5-HT regulates mood, impulse control, and social behavior
// Affects decision-making and social learning
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(float magnitude = 1.0f) override;
    void reset() override;
    float getBaselineLevel() const override;
    float getPeakLevel() const override;
    
    // Mood and social modulation
    void signalSocialCohesion(float cohesionLevel);
    void signalRewardPunishment(float reward, float punishment);
    
    // Behavioral control
    void modulateImpulsivity(float controlLevel);
    void modulateAnhedonia(float anhedoniaLevel);
    
    // Get mood-related statistics
    float getMoodLevel() const;
    float getSocialCohesionLevel() const;
    float getImpulseControlLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
