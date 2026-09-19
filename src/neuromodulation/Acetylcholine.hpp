#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Proper implementation with P-impl pattern

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Boost attention and memory consolidation
    void boostAttention(float amount);
    void consolidateMemory();
    
    // Boost cortical plasticity
    void enhancePlasticity(float amount);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm