#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance effects
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getStressLevel() const;
    
    // Set parameters
    void setBaseline(float baseline);
    void setPeak(float peak);
    void setDecayRate(float rate);
    void setArousalGain(float gain);
    void setVigilanceGain(float gain);
    void setStressModulator(float modulator);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm