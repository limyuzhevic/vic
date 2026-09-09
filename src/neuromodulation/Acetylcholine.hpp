#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Real ACh dynamics with attention modulation and memory consolidation
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Get current neuromodulator levels
    float getLevel() const;
    void setLevel(float level);
    
    // Plasticity factor computation
    float getPlasticityFactor() const;
    float getPlasticityModulation() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Attention modulation
    void signalAttention(float attentionSignal);
    void consolidateMemory(const std::vector<float>& memoryTrace);
    
    // Get attention/memory values
    float getAttentionValue() const;
    float getMemoryConsolidationValue() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
