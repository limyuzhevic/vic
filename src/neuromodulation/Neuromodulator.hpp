#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <algorithm>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Neuromodulators regulate neural excitability, plasticity, and information processing

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
    
    // Apply neuromodulator to multiple targets (convenience)
    virtual void applyToAll(float& excitability, float& learningRate) const {}
    
protected:
    Neuromodulator() = default;
};

} // namespace nlm