#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Provides interface for neuromodulators that influence neural plasticity
// and learning through concentration-dependent effects
// Neuromodulators are chemical signals that modulate neural activity and learning
// by altering synaptic strength, neuronal excitability, and network dynamics

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    /// Get the name of this neuromodulator (e.g., "DA", "ACh", "NE", "5-HT")
    /// @return String identifier for the neuromodulator type
    virtual const char* getName() const = 0;
    
    /// Get the current concentration/level of the neuromodulator (0.0 to 1.0 normalized)
    /// @return Current neuromodulator level
    virtual float getLevel() const = 0;
    
    /// Set the current concentration/level of the neuromodulator
    /// @param level New level (will be clamped to [0.0, 1.0] range)
    virtual void setLevel(float level) = 0;
    
    /// Calculate plasticity modulation factor based on current neuromodulator level
    /// @return Plasticity factor (typically >1.0 for enhancing learning, <1.0 for suppressing)
    virtual float getPlasticityFactor() const = 0;
    
    /// Update neuromodulator state based on simulation timestep
    /// Implements biological dynamics such as decay, release, and integration with activity
    /// @param dt Time step duration in seconds
    virtual void update(TimestepDuration dt) = 0;
    
    /// Check if neuromodulator is active above its baseline level
    /// @return True if neuromodulator level exceeds baseline threshold
    virtual bool isActive() const {
        return getLevel() > getBaselineLevel();
    }
    
    /// Get baseline level for this neuromodulator (typical resting state)
    /// @return Baseline level in [0.0, 1.0] range
    virtual float getBaselineLevel() const = 0;
    
    /// Get maximum level this neuromodulator can reach
    /// @return Maximum level in [0.0, 1.0] range
    virtual float getMaxLevel() const = 0;
    
protected:
    Neuromodulator() = default;
    
    /// Validate and clamp level to valid range [0.0, 1.0]
    /// @param level Input level to validate
    /// @return Clamped level
    float clampLevel(float level) const {
        return std::clamp(level, 0.0f, 1.0f);
    }
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
// Improved implementation with realistic dynamics
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override { return 0.1f; }
    float getMaxLevel() const override { return 1.0f; }
    
    // Signal attention-related events
    void signalAttention(float intensity);
    void signalNovelty(float noveltyLevel);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implemented with realistic dynamics
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override { return 0.05f; }
    float getMaxLevel() const override { return 1.0f; }
    
    // Signal arousal-related events
    void signalArousal(float arousal);
    void signalVigilance(float vigilance);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implemented with realistic dynamics
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override { return 0.1f; }
    float getMaxLevel() const override { return 1.0f; }
    
    // Signal mood-related events
    void signalMood(float mood);
    void signalImpulsivity(float impulsivity);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
