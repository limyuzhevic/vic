#pragma once

#include "Neuromodulator.hpp"
#include <chrono>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// PHASE 2 FIXED: Real ACh dynamics for attention
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getExcitabilityModulator() const override;
    
    // Attention modulation
    void startAttention(float duration);
    void stopAttention();
    bool isAttending() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm