#pragma once

#include "../Neuromodulator.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood signaling
    void signalMood(float moodStrength);
    
    // Social behavior signaling
    void signalSocialBehavior(float socialStrength);
    
    // Impulsivity control
    void reduceImpulsivity(float reductionStrength);
    
    // Reset to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm