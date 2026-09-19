#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Modulates attentional focus and memory encoding strength

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory modulation
    void enhanceMemory(float strength);
    void regulateAttention(float focus);
    
    // Get attention level
    float getAttentionLevel() const;
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
