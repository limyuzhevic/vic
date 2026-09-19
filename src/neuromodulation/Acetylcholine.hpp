#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const { return "ACh"; }
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Attention effects on working memory and encoding
    void signalAttention(float attentionStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
