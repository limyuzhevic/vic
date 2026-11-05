#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Phase 2: Real acetylcholine dynamics affecting working memory and attention
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate attention and working memory capacity
    void modulateAttention(float attentionStrength);
    void enhanceMemoryConsolidation();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm