#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, social behavior, and decision making
// Modulates mood, social reward, and impulsivity

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current level
    float getLevel() const;
    void setLevel(float level);
    
    // Get mood level
    float getMood() const;
    
    // Signal mood boost
    void signalMoodBoost(float moodLevel);
    
    // Signal social reward/rejection
    void signalSocialReward(float socialReward);
    
    // Update 5-HT state
    void update(TimestepDuration dt);
    
    // Get plasticity factor
    float getPlasticityFactor() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm