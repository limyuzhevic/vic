// Serotonin: Mood, impulsivity, and social behavior
// Real implementation for Phase 2 neuromodulation

#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Mood signaling
    void signalMood(float moodLevel);
    void signalReward(float reward);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm