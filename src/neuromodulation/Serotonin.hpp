#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Real biological implementation for serotonergic signaling

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
    void signalPositiveOutcome(float rewardMagnitude, float socialContext);
    void signalNegativeOutcome(float punishmentSeverity, float isolationLevel);
    void modulateMood(float moodInput);
    
    // Behavioral modulation
    void reduceImpulsivity(float cognitiveLoad);
    void enhanceSocialBehavior(float socialCohesion);
    
    // Raphe nuclei dynamics
    void respondToEnvironmentalFactors(float temperature, float humidity);
    void regulateSleepCycle(float sleepPressure);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm