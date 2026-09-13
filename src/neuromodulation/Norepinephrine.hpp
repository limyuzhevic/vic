#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// NE regulates arousal, alertness, and stress response
// Increases signal-to-noise ratio for enhanced focus and attention

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Biological mechanisms
    void increaseArousal(float stimulusIntensity);
    void maintainVigilance(float duration);
    void respondToStress(float stressLevel);
    void enhanceSensoryProcessing();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
