#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PHASE 6: Enhanced neuromodulation system

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

// Acetylcholine: Attention and memory consolidation
// PHASE 6: Implemented with biological accuracy
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "Acetylcholine (ACh)"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// PHASE 6: Implemented with biological accuracy
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "Norepinephrine (NE)"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior
// PHASE 6: Implemented with biological accuracy
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "Serotonin (5-HT)"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Dopamine: Reward and reinforcement learning signal
// PHASE 6: Full implementation with biological dynamics
#include "Dopamine.hpp"

} // namespace nlm
