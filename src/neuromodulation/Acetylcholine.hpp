#pragma once

#include "../Neuromodulator.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal attention through learning signals
    void signalAttention(float attentionSignal);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
