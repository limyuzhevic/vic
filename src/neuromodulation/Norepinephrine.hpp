#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Modulates arousal, vigilance, and attention breadth

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current level
    float getLevel() const;
    void setLevel(float level);
    
    // Get arousal level
    float getArousal() const;
    
    // Signal increased arousal
    void signalArousal(float arousalLevel);
    
    // Signal increased vigilance
    void signalVigilance(float vigilanceLevel);
    
    // Update NE state
    void update(TimestepDuration dt);
    
    // Get plasticity factor
    float getPlasticityFactor() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm