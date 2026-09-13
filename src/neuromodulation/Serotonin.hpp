#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Mood and social signaling
    void signalMood(float rewardHistory, float socialContext);
    void regulateSocialBehavior(float groupCohesion, float dominance);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
