#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Signal social and reward events
    void signalRewardEvent(float reward);
    void signalSocialEvent();
    void signalPredictionErrorEvent(float error);
    
    // Reduce impulsivity
    void reduceImpulsivity();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm