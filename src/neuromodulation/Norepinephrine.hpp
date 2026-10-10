#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal, vigilance, attention, stress response
// Effects on arousal and attention

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get norepinephrine level
    float getLevel() const;
    void setLevel(float level);
    
    // Get arousal modulation factor
    float getArousalFactor() const;
    
    // Get attention enhancement factor
    float getAttentionEnhancement() const;
    
    // Get stress modulation factor
    float getStressModulation() const;
    
    // Update norepinephrine dynamics
    void update(TimestepDuration dt);
    
    // Signal alert or stress
    void signalAlert(float alertness);
    void signalStress(float stress);
    void signalRecovery();
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm