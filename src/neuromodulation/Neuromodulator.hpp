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
    Acetylcholine() : level_(0.0f), baseline_(0.0f), peak_(1.0f), decayRate_(0.1f), releaseRate_(1.0f) {}
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override { level_ = std::max(baseline_, level_ - decayRate_ * static_cast<float>(dt)); }
    
    void signalAttention(float signal) { level_ = std::min(peak_, level_ + signal * releaseRate_); }
    
private:
    float level_;
    float baseline_;
    float peak_;
    float decayRate_;
    float releaseRate_;
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine() : level_(0.0f), baseline_(0.1f), peak_(1.0f), decayRate_(0.05f), releaseRate_(1.2f) {}
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 0.8f + 0.4f * level_; }
    void update(TimestepDuration dt) override { level_ = std::max(baseline_, level_ - decayRate_ * static_cast<float>(dt)); }
    
    void signalArousal(float signal) { level_ = std::min(peak_, level_ + signal * releaseRate_); }
    
private:
    float level_;
    float baseline_;
    float peak_;
    float decayRate_;
    float releaseRate_;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin() : level_(0.0f), baseline_(0.0f), peak_(1.0f), decayRate_(0.08f), releaseRate_(0.8f) {}
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override { level_ = std::max(baseline_, level_ - decayRate_ * static_cast<float>(dt)); }
    
    void signalMood(float signal) { level_ = std::min(peak_, level_ + signal * releaseRate_); }
    
private:
    float level_;
    float baseline_;
    float peak_;
    float decayRate_;
    float releaseRate_;
};

} // namespace nlm
