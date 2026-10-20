#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include <memory>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Phase 2 implementation - enhances attentional selectivity and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void signalAttentionShift(float priority);
    
    // Memory consolidation
    void signalMemoryConsolidation(float consolidationSignal);
    
    // Get attention gain
    float getAttentionGain() const;
    
    // Get memory consolidation rate
    float getMemoryConsolidationRate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

