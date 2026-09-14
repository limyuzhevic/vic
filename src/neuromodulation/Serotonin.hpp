#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2 implementation

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation
    void signalSatiation(float rewardValue);
    
    // Impulsivity reduction
    void reduceImpulsivity(float duration);
    
    // Get current reward sensitivity and impulsivity
    float getRewardSensitivity() const;
    float getImpulsivity() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm