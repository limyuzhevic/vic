#pragma once

#include "../core/Types/Types.hpp"
#include "Dopamine.hpp"

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
    
    // Update neuromodulator state with prediction error, reward, and context signals
    // TODO PHASE 2: Implement real neuromodulation effects
    virtual void update(float predictionError, float reward, float contextSignal, TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(float predictionError, float reward, float contextSignal, TimestepDuration dt) override {}
};

} // namespace nlm
