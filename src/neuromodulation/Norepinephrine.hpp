#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance response
// Real-world based: NE increases with stress/challenges, enhances signal-to-noise, promotes focused attention
// Clinical relevance: NE dysregulation in ADHD, anxiety disorders, PTSD

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference for integration
    void initialize(class Brain* brain);
    
    // Get current NE level
    float getLevel() const;
    void setLevel(float level);
    
    // Update NE dynamics based on stress/challenge level
    void update(float stressLevel, float challenge, TimestepDuration dt);
    
    // Get NE-modulated plasticity factor
    float getPlasticityFactor() const;
    
    // Advanced functionality
    void enhanceSignalToNoise(NeuronId targetNeuron);
    void increase alertness(float intensity);
    void triggerFightOrFlightResponse();
    
    // Configuration parameters
    void setStressSensitivity(float sensitivity);
    void setChallengeWeight(float weight);
    void setArousalBaseline(float baseline);
    
    float getStressSensitivity() const;
    float getChallengeWeight() const;
    float getArousalBaseline() const;
    
    // Reset for new episode
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm