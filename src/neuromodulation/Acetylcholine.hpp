#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Key neuromodulator for:
// - Memory encoding and consolidation
// - Attentional selection and focus
// - Cortical plasticity and synaptic strengthening
// - Learning in the hippocampus and cortex

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory-related functions
    float getConsolidationWeight() const;
    void setConsolidationWeight(float weight);
    void enhanceMemoryEncoding(float novelty);
    void promoteAttention(float focusLevel);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm