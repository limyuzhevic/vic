#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory consolidation effects
    float getMemoryConsolidationRate() const;
    float getAttentionGain() const;
    float getLearningRate() const;
    
    // Set parameters
    void setBaseline(float baseline);
    void setPeak(float peak);
    void setDecayRate(float rate);
    void setMemoryGain(float gain);
    void setAttentionStrength(float strength);
    void setLearningModulator(float modulator);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm