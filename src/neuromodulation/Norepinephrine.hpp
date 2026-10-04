#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Signal arousing events
    void signalArousalEvent();
    void signalSurpriseEvent();
    void signalPredictionErrorEvent(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm