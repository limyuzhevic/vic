#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation (Phase 6)
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention-related functions
    void enhanceAttention(float focusStrength);
    void enhanceMemoryEncoding(float encodingStrength);
    void reduceInterference(float reductionFactor);
    
    // Memory consolidation functions
    void promoteWorkingMemoryConsolidation(float factor);
    void promoteEpisodicMemoryConsolidation(float factor);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm