#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

// Serotonin: Raphe nucleus dynamics and receptor subtype modulation
// Implements serotonin signaling for mood regulation, impulsivity control, and social behavior
// Includes 5-HT1A, 5-HT2A, and 5-HT3 receptor subtype dynamics

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Raphe nucleus firing dynamics
    void signalModulatory();      // Modulatory signaling (basal forebrain)
    void signalBurst();           // Burst firing (social behavior)
    void signalInhibition();      // Inhibitory signaling (anxiety suppression)
    
    // Receptor subtype dynamics
    float getFiveHT1AEffect() const;   // Anxiolytic, inhibitory
    float getFiveHT2AEffect() const;   // Excitatory, cognitive
    float getFiveHT3Effect() const;    // Fast, excitatory
    
    // Reuptake and clearance
    void setReuptakeRate(float rate);
    void triggerVesicularRelease();
    
    // Social behavior modulation
    void updateSocialContext(float socialCohesion, float stressLevel);
    
    // Impulse control mechanism
    void updateImpulseControl(float currentBehavior, float targetBehavior);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm