#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

class Brain;

// Serotonin: Mood and behavioral flexibility neuromodulator
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
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Signal positive mood events
    void signalPositiveMood(float intensity);
    
    // Signal social rewards
    void signalSocialReward(float value);
    
    // Signal stress responses
    void signalStress(float magnitude);
    
    // Set impulsivity reduction
    void setImpulsivityReduction(float reduction);
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm