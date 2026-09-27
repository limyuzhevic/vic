#pragma once

#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Real implementation with Pimpl pattern

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Mood and social behavior modulation
    void signalReward(float reward);
    void signalPunishment(float punishment);
    void modulateSocialBehavior(float socialStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm