#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Regulates alertness, attention, and prioritizes important information

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current NE level
    float getLevel() const;
    void setLevel(float level);
    
    // Get alertness level
    float getAlertness() const;
    
    // Get attention priority factor
    float getAttentionPriority() const;
    
    // Get arousal level
    float getArousalLevel() const;
    
    // Update NE based on stress, novelty, and importance
    void update(TimestepDuration dt, float stress, float novelty, float importance);
    
    // Signal stress or challenge
    void signalChallenge(float intensity);
    
    // Signal reward or positive outcome
    void signalReward(float value);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm