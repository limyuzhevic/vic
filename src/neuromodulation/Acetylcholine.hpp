#pragma once

#include "Neuromodulator.hpp"
#include <vector>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Real-time acetylcholine dynamics for attention modulation, 
// working memory enhancement, and memory consolidation

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
    
    // Attention-related functions
    void enhanceAttention(float strength);
    
    // Memory-related functions
    void signalMemoryRequest(float priority);
    void integrateWithLearning(float learningSignal, float confidence);
    
    // Focus and concentration modulation
    void increaseFocus(float factor);
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
