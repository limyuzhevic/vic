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
    
    // Memory consolidation
    void signalMemoryEncoding(float strength);
    void signalMemoryConsolidation(float consolidation);
    void signalAttention(float attention);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm