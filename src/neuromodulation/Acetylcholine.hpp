#include "Acetylcholine.hpp"
#pragma once

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention-related signaling
    void signalAttention(float attentionSignal);
    void signalMemoryConsolidation(float consolidationStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm