#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Controls mood, impulse control, and social bonding

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Behavioral effects
    void enhanceMood(float moodBoost);
    void reduceImpulsivity(float controlSignal);
    void promoteSocialBehavior(float socialBond);
    
    // Accessors
    float getMood() const;
    float getImpulsivity() const;
    float getSocialBehavior() const;
    float getRewardSensitivity() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm