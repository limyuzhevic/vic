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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {};
    
    // Development support
    void setDevelopmentMultiplier(float multiplier) { developmentMultiplier_ = multiplier; }
    
protected:
    Serotonin() : level_(0.0f), developmentMultiplier_(1.0f) {}
    
private:
    float level_;
    float developmentMultiplier_;
};

// Curiosity: Exploration motivation
// PLACEHOLDER - Phase 2
class Curiosity : public Neuromodulator {
public:
    const char* getName() const override { return "Curiosity"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {};
    
    // Development support
    void setDevelopmentMultiplier(float multiplier) { developmentMultiplier_ = multiplier; }
    
protected:
    Curiosity() : level_(0.0f), developmentMultiplier_(1.0f) {}
    
private:
    float level_;
    float developmentMultiplier_;
};

// Novelty: Novelty detection
// PLACEHOLDER - Phase 2
class Novelty : public Neuromodulator {
public:
    const char* getName() const override { return "Novelty"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {};
    
    // Development support
    void setDevelopmentMultiplier(float multiplier) { developmentMultiplier_ = multiplier; }
    
protected:
    Novelty() : level_(0.0f), developmentMultiplier_(1.0f) {}
    
private:
    float level_;
    float developmentMultiplier_;
};

// PredictionError: Prediction error signaling
// PLACEHOLDER - Phase 2
class PredictionError : public Neuromodulator {
public:
    const char* getName() const override { return "PredictionError"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {};
    
    // Development support
    void setDevelopmentMultiplier(float multiplier) { developmentMultiplier_ = multiplier; }
    
protected:
    PredictionError() : level_(0.0f), developmentMultiplier_(1.0f) {}
    
private:
    float level_;
    float developmentMultiplier_;
};

// Reward: Reward and satiety signaling
// PLACEHOLDER - Phase 2
class Reward : public Neuromodulator {
public:
    const char* getName() const override { return "Reward"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {};
    
    // Development support
    void setDevelopmentMultiplier(float multiplier) { developmentMultiplier_ = multiplier; }
    
protected:
    Reward() : level_(0.0f), developmentMultiplier_(1.0f) {}
    
private:
    float level_;
    float developmentMultiplier_;
};
