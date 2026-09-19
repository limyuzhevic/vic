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
// Modulates attention to relevant stimuli, strengthens working memory,
// promotes memory consolidation, and modulates plasticity rules
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get attention modulation factor
    float getAttentionModulation() const;
    
    // Get working memory modulation factor
    float getWorkingMemoryModulation() const;
    
    // Set attention strength
    void setAttentionStrength(float strength);
    
    // Set memory consolidation rate
    void setConsolidationRate(float rate);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Modulates arousal, improves signal-to-noise ratio,
// affects stress response and memory encoding
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get arousal modulation factor
    float getArousalModulation() const;
    
    // Get signal detection enhancement factor
    float getSignalDetectionModulation() const;
    
    // Get memory encoding enhancement factor
    float getMemoryEncodingModulation() const;
    
    // Set arousal sensitivity
    void setArousalSensitivity(float sensitivity);
    
    // Set stress sensitivity
    void setStressSensitivity(float sensitivity);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Modulates mood, allows behavioral flexibility,
// regulates sleep/wake cycle, and promotes forgetting of irrelevant info
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get mood modulation factor
    float getMoodModulation() const;
    
    // Get behavioral flexibility modulation factor
    float getBehavioralFlexibilityModulation() const;
    
    // Get memory consolidation (forgetting) factor
    float getMemoryConsolidationModulation() const;
    
    // Set mood sensitivity
    void setMoodSensitivity(float sensitivity);
    
    // Set flexibility sensitivity
    void setFlexibilitySensitivity(float sensitivity);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
