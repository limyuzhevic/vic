#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <random>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// NOT consciousness - serotonergic signaling for mood regulation
//
// Key mechanisms:
// - Regulates mood and emotional state
// - Modulates impulsivity and risk-taking behavior
// - Influences social learning and bonding
// - Affects sleep and appetite
// - Impacts learning and memory consolidation

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Adjust mood based on reward prediction
    void adjustMood(float rewardPrediction);
    
    // Modify impulsivity based on urgency
    void modifyImpulsivity(float urgency);
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Get random number generator for stochastic fluctuations
    std::mt19937& getRng() { return rng_; }
    
    // Calculate current mood level from brain systems
    float calculateMoodLevel() const;
    
    // Random number generator for physiological fluctuations
    std::mt19937 rng_;
};

} // namespace nlm