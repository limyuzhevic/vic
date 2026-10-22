#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <string>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior neuromodulator
// Biological role: Regulates mood, inhibits behavioral inhibition, affects social behavior

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get modulator name
    const char* getName() const { return "5-HT"; }
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Mood modulation
    void enhanceMood(float positiveStimulus);
    void triggerReward(float rewardValue);
    float getMoodLevel() const;
    
    // Behavioral control
    float getInhibitionLevel() const;
    float getRiskTakingLevel() const;
    
    // Social behavior
    float getSocialCohesionLevel() const;
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm