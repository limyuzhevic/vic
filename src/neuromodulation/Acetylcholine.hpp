#pragma once

#include "../Neuromodulator.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention signaling
    void signalAttention(float attentionStrength);
    
    // Memory consolidation signaling
    void signalMemoryPriority();
    
    // Reset to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm