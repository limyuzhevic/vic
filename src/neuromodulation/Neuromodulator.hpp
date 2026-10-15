#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Updated Phase 2 implementation with real neuromodulatory effects

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
    
    // Reset neuromodulator to initial state
    virtual void reset() {}
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Enhanced Phase 2 implementation with real reward signaling
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Accessors
    float getLevel() const { return getLevel(); }
    void setLevel(float level) { setLevel(level); }
    float getPlasticityFactor() const { return getPlasticityFactor(); }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Real implementation with attention modulation and memory consolidation
#include "Acetylcholine.hpp"

// Norepinephrine: Arousal and vigilance
// Real implementation with arousal regulation and vigilance control
#include "Norepinephrine.hpp"

// Serotonin: Mood, impulsivity, and social behavior
// Real implementation with mood modulation and impulse control
#include "Serotonin.hpp"

} // namespace nlm
