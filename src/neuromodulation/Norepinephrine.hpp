#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// PHASE 2 FIXED: Real NE dynamics for arousal
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getExcitabilityModulator() const override;
    
    // Arousal modulation
    void triggerAlert(float intensity);
    void calmDown();
    float getAlertness() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm