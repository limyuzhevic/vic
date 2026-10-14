#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Enhanced implementation with real attention and memory modulation

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Attention and memory signaling
    void signalAttention(float attentionStrength);
    void signalMemoryConsolidation();
    
    float getAttentionGain() const;
    float getMemoryConsolidationRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm