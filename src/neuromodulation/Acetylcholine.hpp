#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Real implementation with biological dynamics
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Attention-related signaling
    void signalAttention(float attentionStrength);
    void signalMemory(float memoryStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm