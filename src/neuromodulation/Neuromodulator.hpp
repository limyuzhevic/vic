#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals

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

// Simple placeholder neuromodulators for Phase 1 systems
// These provide default implementations for neuromodulators that
// are not yet fully implemented but maintain the interface
class SimpleNeuromodulator : public Neuromodulator {
public:
    SimpleNeuromodulator(const char* name);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
protected:
    const char* name_;
    float level_;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public SimpleNeuromodulator {
public:
    Acetylcholine();
    
    float getPlasticityFactor() const override;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public SimpleNeuromodulator {
public:
    Norepinephrine();
    
    float getPlasticityFactor() const override;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public SimpleNeuromodulator {
public:
    Serotonin();
    
    float getPlasticityFactor() const override;
};

} // namespace nlm
