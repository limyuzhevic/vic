#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Implemented neuromodulation system for Phase 6 with biologically inspired signaling

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
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Implements dopamine-based reward prediction error signaling
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
    
    // Dopamine parameters
    void setSignalStrength(float strength);
    float getSignalStrength() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Implements ACh signaling for attention and working memory
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // ACh parameters
    void setBurstFrequency(float freq);
    float getBurstFrequency() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implements NE signaling for arousal states
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // NE parameters
    void setReleaseRate(float rate);
    float getReleaseRate() const;
    void setReuptakeRate(float rate);
    float getReuptakeRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implements serotonin signaling for mood regulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Serotonin parameters
    void setSynthesisRate(float rate);
    float getSynthesisRate() const;
    void setDegradationRate(float rate);
    float getDegradationRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
