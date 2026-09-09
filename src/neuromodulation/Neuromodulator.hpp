#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Real implementation with neuromodulation dynamics and plasticity effects
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
    
    // Reset for new episode
    virtual void reset() {}
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Real implementation with reward prediction error and plasticity modulation
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
    void signalPredictionError(float error);
    
    // Update with prediction error for reinforcement learning
    void updateWithPredictionError(float predictionError, TimestepDuration dt);
    
    // Get prediction error history
    const std::vector<float>& getPredictionErrorHistory() const;
    
    // Set dopamine parameters
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setPhasicGain(float gain) { phasicGain_ = gain; }
    void setBaseline(float baseline) { baseline_ = baseline; }
    
    // Reset for new episode
    void reset() override { pImpl->level = baseline_; pImpl->predictionErrorHistory.clear(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float learningRate_;
    float phasicGain_;
    float baseline_;
};

// Acetylcholine: Attention and memory consolidation
// Real implementation with attentional focus modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Update attentional focus and memory gating
    void updateAttention(float globalInhibition, float topDownBias);
    void enhanceMemoryEncoding(float strength = 1.0f);
    
    // Reset for new episode
    void reset() override { pImpl->level = 0.0f; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Real implementation with alerting and response selection
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Update arousal based on novelty and prediction error
    void updateArousal(float novelty, float predictionError, TimestepDuration dt);
    
    // Reset for new episode
    void reset() override { pImpl->level = 0.0f; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Real implementation with behavioral flexibility modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Update behavioral flexibility and exploration/exploitation balance
    void updateBehavioralFlexibility(float explorationCost, float rewardUncertainty);
    
    // Reset for new episode
    void reset() override { pImpl->level = 0.0f; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
