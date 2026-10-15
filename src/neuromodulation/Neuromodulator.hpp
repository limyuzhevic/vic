#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Manages neuromodulator concentrations and integrates with plasticity rules
// Supports spike-based neuromodulatory signaling

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Get plasticity factor (neuromodulator modulates synaptic plasticity)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state dynamics
    virtual void update(TimestepDuration dt) = 0;
    
    // Spike-based neuromodulatory signaling
    virtual void signalSpike() = 0;
    
    // Reset to initial state
    virtual void reset() = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward prediction error computation
// Models phasic dopamine responses to rewards and prediction errors

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalSpike() override;
    void reset() override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Integrate with prediction error system
    void setPredictionError(float error);
    
    // Set baseline dopamine level (tonic component)
    void setBaseline(float baseline);
    float getBaseline() const;
    
    // Configure dopamine dynamics parameters
    void setDecayRate(float rate);
    void setReleaseRate(float rate);
    void setPeakLevel(float level);
    void setBurstFactor(float factor);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2 will implement real acetylcholine effects

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalSpike() override;
    void reset() override;
    
    // Set attention modulation parameters
    void setAttentionGain(float gain);
    void setMemoryConsolidationRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2 will implement real norepinephrine effects

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalSpike() override;
    void reset() override;
    
    // Set arousal modulation parameters
    void setArousalLevel(float level);
    void setVigilanceGain(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2 will implement real serotonin effects

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalSpike() override;
    void reset() override;
    
    // Set mood modulation parameters
    void setMoodLevel(float level);
    void setImpulsivityFactor(float factor);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
