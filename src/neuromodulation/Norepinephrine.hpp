#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const { return "NE"; }
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(float dt);
    
    // Signal processing
    void signalArousal(float arousal);
    void signalVigilance(float vigilance);
    void signalThreatResponse(float threatLevel);
    void signalSleepArchitecture(float slowWaveRatio);
    void modulateAttention(float& excitationGain, float& inhibitionGain) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm