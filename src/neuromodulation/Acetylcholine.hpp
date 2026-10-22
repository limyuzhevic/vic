#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation system
// Real implementation with cholinergic modulation effects

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    // ACh enhances LTP and reduces LTD, promoting learning
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Memory consolidation
    void consolidateMemory(const std::vector<float>& memoryTrace, float strength);
    
    // Attention modulation
    float modulateAttention(float baselineAttention, float novelty) const;
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
