#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Phase 2 implementation

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention and memory functions
    void signalAttention(float attentionLevel);
    void enhanceMemoryConsolidation();
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm