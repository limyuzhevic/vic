#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Enhanced neuromodulator with attention and memory functions

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
    
    // Attention and memory modulation
    void signalAttention(float attentionStrength);
    void signalMemoryConsolidation(float memoryStrength);
    void updateAttention(float newAttention, TimestepDuration dt);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm