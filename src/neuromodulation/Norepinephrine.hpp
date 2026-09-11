#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Real biological implementation for noradrenergic signaling

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal mechanisms
    void signalAlert(float threatLevel, float noveltyScore);
    void enhanceVigilance(float vigilanceDemand);
    void modulateSleepWakeCycle(bool isAwake);
    
    // Locus coeruleus dynamics
    void respondToStress(float stressLevel);
    void enhanceFocus(float taskDifficulty);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm