#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Real implementation with biological dynamics
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Mood and social signaling
    void signalMood(float mood);
    void signalSocialBehavior(float sociality);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm