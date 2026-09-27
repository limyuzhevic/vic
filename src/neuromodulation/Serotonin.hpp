#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Real implementation with biological fidelity

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
    
    // Social signaling
    void signalPositiveExperience(float experienceQuality);
    void signalSocialBonding(float socialStrength);
    
    float getMoodEffect() const;
    float getSocialBehaviorEffect() const;
    float getRapheNucleusActivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
