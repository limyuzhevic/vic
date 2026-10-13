#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Implemented with biological plausibility for mood regulation

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Reset to baseline state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
