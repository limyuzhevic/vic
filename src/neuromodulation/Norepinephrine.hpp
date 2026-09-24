#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Phase 6 Implementation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // NE effects on arousal and stress
    void signalStress(float stressLevel);
    void signalAttention(float attentionLevel);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
