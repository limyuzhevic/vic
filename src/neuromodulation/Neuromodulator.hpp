#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 2: Real neuromodulation effects implemented

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Initialize with brain reference (for integration)
    virtual void initialize(class Brain* brain) = 0;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Higher values = stronger modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Apply neuromodulatory effects on neurons
    virtual void applyToNeurons(class Brain* brain, TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain) override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void applyToNeurons(class Brain* brain, TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get current reward prediction error
    float getPredictionError() const { return pImpl->predictionError; }
    
    // Set baseline for decay
    void setBaseline(float baseline) { pImpl->baseline = baseline; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain) override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void applyToNeurons(class Brain* brain, TimestepDuration dt) override;
    
    // Arousal signaling
    void signalAttentionShift(float strength);
    void signalMemoryConsolidation(float strength);
    
    // Configuration
    void setAttentionImpact(float impact);
    void setMemoryImpact(float impact);
    float getAttentionImpact() const;
    float getMemoryImpact() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain) override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void applyToNeurons(class Brain* brain, TimestepDuration dt) override;
    
    // Vigilance signaling
    void signalArousal(float strength);
    void signalVigilance(float strength);
    
    // Configuration
    void setArousalImpact(float impact);
    void setMemoryImpact(float impact);
    void setAttentionImpact(float impact);
    float getArousalImpact() const;
    float getMemoryImpact() const;
    float getAttentionImpact() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
