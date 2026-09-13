#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// 5-HT regulates mood, aggression, impulsivity, and social behavior
// Stabilizes neural circuits for emotional regulation and social cognition

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
    
    // Biological mechanisms
    void improveMood(float positiveInput);
    void reduceImpulsivity();
    void modulateSocialBehavior(float socialContext);
    void regulateAggression(float threatLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
