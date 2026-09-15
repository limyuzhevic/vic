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
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Reward-modulated learning with eligibility traces
    virtual float calculateRewardModulation(float eligibilityTrace) const = 0;
    
    // Prediction error modulation
    virtual float calculatePredictionErrorModulation(float predictionError) const = 0;
    
    // Metaplasticity based on recent learning history
    virtual float calculateMetaplasticFactor() const = 0;
    
    // Synaptic tagging for plasticity events
    virtual bool hasPlasticityTag() const = 0;
    virtual void setPlasticityTag(bool hasTag) = 0;
    virtual float getTagAge() const = 0;
    
protected:
    Neuromodulator() = default;
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
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Reward-modulated learning implementation
    float calculateRewardModulation(float eligibilityTrace) const override;
    float calculatePredictionErrorModulation(float predictionError) const override;
    float calculateMetaplasticFactor() const override;
    bool hasPlasticityTag() const override;
    void setPlasticityTag(bool hasTag) override;
    float getTagAge() const override;
    
    // Learning rate adaptation
    void adaptLearningRate(float error, float learningRate);
    
    // Tag management for synaptic plasticity
    void createPlasticityTag(float tagDuration);
    void updatePlasticityTag(TimestepDuration dt);
    
    // Eligibility trace management
    void addToEligibilityTrace(float traceChange, int synapseId);
    void decayEligibilityTrace(float decayRate, int synapseId);
    
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
    
    // Attention modulation of learning
    float calculateAttentionModulation(float attentionLevel) const;
    
    // Memory consolidation effects
    float calculateMemoryConsolidation() const;
    
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
    
    // Arousal modulation of attention and learning
    float calculateArousalModulation(float arousalLevel) const;
    
    // Vigilance control of learning
    float calculateVigilanceControl() const;
    
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
    void update(TimestepDuration dt) override;
    
    // Mood-dependent learning modulation
    float calculateMoodModulation() const;
    
    // Impulse control effects on plasticity
    float calculateImpulseControl() const;
    
    // Social behavior modulation
    float calculateSocialModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
