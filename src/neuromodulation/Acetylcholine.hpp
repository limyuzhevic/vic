#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Phase 6 Implementation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // ACh effects on attention and memory
    void signalAttentionBoost(float boost);
    void signalMemoryConsolidation(float strength);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
