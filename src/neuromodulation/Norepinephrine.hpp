#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Increases arousal, attention, and vigilance based on stress/arousal needs

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get NE level
    float getLevel() const;
    
    // Update NE based on arousal and vigilance needs
    void update(TimestepDuration dt);
    
    // Get arousal modulation factor
    float getArousalFactor() const;
    
    // Get vigilance factor (attention sharpening)
    float getVigilanceFactor() const;
    
    // Get stress adaptation factor
    float getStressAdaptationFactor() const;
    
    // Set parameters
    void setArousalWeight(float weight);
    void setVigilanceWeight(float weight);
    void setBaselineLevel(float level);
    
    // Stress signal
    void signalStress(float stressLevel);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm