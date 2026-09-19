#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Modulates arousal, improves signal-to-noise ratio,
// affects stress response and memory encoding

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get norepinephrine level
    float getLevel() const;
    
    // Update norepinephrine based on arousal and stress
    void update(float arousal, float stress, TimestepDuration dt);
    
    // Get arousal modulation factor
    float getArousalModulation() const;
    
    // Get signal detection enhancement factor
    float getSignalDetectionModulation() const;
    
    // Get memory encoding enhancement factor
    float getMemoryEncodingModulation() const;
    
    // Set arousal sensitivity
    void setArousalSensitivity(float sensitivity);
    
    // Set stress sensitivity
    void setStressSensitivity(float sensitivity);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm