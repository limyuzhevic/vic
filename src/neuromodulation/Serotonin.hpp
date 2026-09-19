#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Modulates mood, allows behavioral flexibility,
// regulates sleep/wake cycle, and promotes forgetting of irrelevant info

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get serotonin level
    float getLevel() const;
    
    // Update serotonin based on mood and behavioral state
    void update(float mood, float behavioralFlexibility, TimestepDuration dt);
    
    // Get mood modulation factor
    float getMoodModulation() const;
    
    // Get behavioral flexibility modulation factor
    float getBehavioralFlexibilityModulation() const;
    
    // Get memory consolidation (forgetting) factor
    float getMemoryConsolidationModulation() const;
    
    // Set mood sensitivity
    void setMoodSensitivity(float sensitivity);
    
    // Set flexibility sensitivity
    void setFlexibilitySensitivity(float sensitivity);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm