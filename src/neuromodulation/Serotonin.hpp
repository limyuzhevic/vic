#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Regulates mood, impulse control, and social behavior modulation

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current 5-HT level
    float getLevel() const;
    void setLevel(float level);
    
    // Get mood regulation factor
    float getMoodRegulation() const;
    
    // Get impulse control factor
    float getImpulseControl() const;
    
    // Get social behavior modulation factor
    float getSocialBehaviorModulation() const;
    
    // Update serotonin based on reward prediction, punishments, and social interactions
    void update(TimestepDuration dt, float rewardHistory, float punishmentHistory, float socialInteraction);
    
    // Signal positive outcome/punishment
    void signalOutcome(float outcomeValue);
    
    // Signal social interaction
    void signalSocial(float interactionValue);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm