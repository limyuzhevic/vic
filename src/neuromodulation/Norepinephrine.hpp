#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Enhanced implementation with real arousal and stress modulation

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Arousal and stress signaling
    void signalArousal(float arousalStrength);
    void signalStress(float stressStrength);
    void signalRecovery(float recoveryStrength);
    
    float getLearningRateModulation() const;
    float getInhibitionLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm