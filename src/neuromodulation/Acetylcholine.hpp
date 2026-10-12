// Acetylcholine: Attention and memory consolidation
// Real implementation for Phase 2 neuromodulation

#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Attention signaling
    void signalAttention(float attentionStrength);
    void signalNovelty(float noveltyLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm