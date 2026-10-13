#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// 5-HT regulates mood, impulsivity, and social behavior, modulates plasticity

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const { return "5-HT"; }
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Mood regulation
    void regulateMood(float moodLevel);
    
    // Impulse control
    void controlImpulses(float controlStrength);
    
    // Social behavior modulation
    void modulateSocialBehavior(float socialStrength);
    
    // Set social learning rate
    void setSocialLearningRate(float rate);
    
    // Initialize with brain reference (like other neuromodulators)
    void initialize(class Brain* brain);
    
    // Reset function
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm