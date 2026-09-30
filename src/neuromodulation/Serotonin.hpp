#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Serotonin: Mood regulation, impulsivity control, social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Social behavior modulation
    void signalSocialReward(float socialValue);
    void signalSocialPunishment(float socialCost);
    
    // Impulsivity control
    float getImpulsivityLevel() const;
    void setImpulsivityLevel(float level);
    
    // Mood modulation
    void setMood(float mood) { setLevel(mood); }
    float getMood() const { return getLevel(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
