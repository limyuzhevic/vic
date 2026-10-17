#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    float getAttentionGain() const;
    void signalSalientEvent(float saliency);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
