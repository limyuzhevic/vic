#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// PHASE 2 FIXED: Real 5-HT dynamics for mood regulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getExcitabilityModulator() const override;
    
    // Mood modulation
    void setMood(float mood);
    float getMood() const;
    void modulateImpulsivity(float factor);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm