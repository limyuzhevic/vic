#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Controls alertness, attention, and signal detection

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
    
    // Behavioral effects
    void signalAlert(float alertStrength);
    void increaseArousal(float arousalSignal);
    
    // Accessors
    float getArousalLevel() const;
    float getVigilance() const;
    float getSignalDetection() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm