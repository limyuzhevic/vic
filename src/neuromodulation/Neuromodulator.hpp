#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator(nullptr) = default;
    
    // Get modulator name
    virtual const char* getName(nullptr) const = 0;
    
    // Get current concentration/level
    virtual float getLevel(nullptr) const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // TODO: PHASE 2 - Implement real modulation
    virtual float getPlasticityFactor(nullptr) const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator(nullptr) = default;
};

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2
class Dopamine : public Neuromodulator {
public:
    Dopamine(nullptr);
    ~Dopamine(nullptr) override;
    
    const char* getName(nullptr) const override;
    float getLevel(nullptr) const override;
    void setLevel(float level) override;
    float getPlasticityFactor(nullptr) const override;
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
    const char* getName(nullptr) const override { return "ACh"; }
    float getLevel(nullptr) const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor(nullptr) const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    const char* getName(nullptr) const override { return "NE"; }
    float getLevel(nullptr) const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor(nullptr) const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    const char* getName(nullptr) const override { return "5-HT"; }
    float getLevel(nullptr) const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor(nullptr) const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
