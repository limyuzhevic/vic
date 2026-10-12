// Norepinephrine: Arousal and vigilance
// Real implementation for Phase 2 neuromodulation

#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

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
    void signalStress(float stressLevel);
    void signalAttention(float attentionStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm