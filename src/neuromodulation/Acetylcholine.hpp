#pragma once

#include "Neuromodulator.hpp"

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
    
    // Modulate attention and memory
    void applyToAttention() override;
    void applyToMemoryConsolidation() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm