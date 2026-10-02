#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced neuromodulator with mood and social functions

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
    
    // Mood and social modulation
    void signalSocialBonding(float bondingStrength);
    void signalReward(float reward);
    void setMood(float mood);
    void setImpulsivity(float impulsivity);
    void signalSocialReward(float socialReward);
    
    // Reset
    void reset();
    
    // Get state
    float getImpulsivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm