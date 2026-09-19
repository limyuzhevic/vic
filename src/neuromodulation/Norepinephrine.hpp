#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Controls alertness and behavioral activation

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal modulation
    void increaseAlertness(float amount);
    void decreaseStress(float amount);
    
    // Get arousal level
    float getArousalLevel() const;
    
    // Get stress level
    float getStressLevel() const;
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
