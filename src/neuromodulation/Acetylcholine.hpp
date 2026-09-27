#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Real implementation with biological fidelity

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
    
    // Attention signaling
    void signalAttention(float attentionDemand);
    void signalMemoryConsolidation(float memoryStrength);
    
    float getAttentionEffect() const;
    float getMemoryConsolidationEffect() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
