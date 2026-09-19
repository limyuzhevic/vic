#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Real implementation with biological dynamics
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Arousal signaling
    void signalArousal(float arousalLevel);
    void signalStress(float stressLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm