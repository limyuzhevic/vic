#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Real implementation with actual neuromodulation effects

class Neuromodulator {
public:
    Neuromodulator();
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Real neuromodulators affect synaptic plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Forward declarations for specific neuromodulators
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Novelty;
class Curiosity;
class Reward;
class PredictionError;

} // namespace nlm
