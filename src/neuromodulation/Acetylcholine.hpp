#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Acetylcholine: Attention and working memory consolidation
// PHASE 1: Full implementation with biological plausibility
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // ACh-specific functions for attention and working memory
    float getAttentionModulator() const;
    void signalAttention(bool focusEvent);
    float getMemoryConsolidationFactor() const;
    void promoteMemoryConsolidation(int neuralTraceId);
    
    // ACh effects on cortical networks
    void enhanceSensoryProcessing() const;
    void suppressBackgroundActivity() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm