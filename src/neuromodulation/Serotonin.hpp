#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Regulates mood, modulates impulsivity, and supports social behavior

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get 5-HT level
    float getLevel() const;
    
    // Update serotonin based on mood and social needs
    void update(TimestepDuration dt);
    
    // Get mood modulation factor
    float getMoodFactor() const;
    
    // Get impulsivity modulation factor
    float getImpulsivityFactor() const;
    
    // Get social behavior modulation factor
    float getSocialFactor() const;
    
    // Set parameters
    void setMoodWeight(float weight);
    void setImpulsivityWeight(float weight);
    void setSocialWeight(float weight);
    void setBaselineLevel(float level);
    
    // Social signal (positive social interaction)
    void signalSocialReward(float socialQuality);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm