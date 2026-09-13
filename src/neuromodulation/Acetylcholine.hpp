#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Enhanced with temporal dynamics and learning modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory consolidation
    void consolidateMemory(const std::vector<float>& memoryTrace);
    void updateAttention(float sensoryInput);
    float getAttentionLevel() const;
    float getMemoryConsolidationLevel() const;
    
    // Configuration
    void setAttentionGain(float gain);
    void setMemoryDecayRate(float decayRate);
    void reset() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm