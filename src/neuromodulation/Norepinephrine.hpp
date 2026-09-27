#pragma once

#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Real implementation with Pimpl pattern

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
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
    
    // Arousal and stress response
    void signalStress(float stressLevel);
    void increaseVigilance(float vigilanceLevel);
    void enhanceAlertness(float alertnessStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm