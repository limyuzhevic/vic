#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Real implementation with biologically inspired dynamics

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
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Signal attention for salient stimuli
    void signalAttention(float salience);
    
    // Enhance memory consolidation
    void enhanceMemory();
    
    // Get attention gain factor
    const float& getAttentionGain() const;
    void setAttentionGain(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
