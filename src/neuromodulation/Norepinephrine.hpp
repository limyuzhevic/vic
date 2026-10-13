#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// NE increases arousal, enhances signal detection, modulates memory encoding

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const { return "NE"; }
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Arousal effect
    void increaseArousal(float arousalLevel);
    
    // Signal detection enhancement
    void enhanceSignalDetection(float gain);
    
    // Memory encoding modulation
    void modulateEncoding(bool enhance);
    
    // Initialize with brain reference (like other neuromodulators)
    void initialize(class Brain* brain);
    
    // Reset function
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm