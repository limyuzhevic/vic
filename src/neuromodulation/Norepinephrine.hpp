// Norepinephrine.hpp - Neuromodulator for arousal and vigilance

#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine modulates arousal, alertness, and stress response
// Increases neural responsiveness and reduces noise
// Important for encoding and memory consolidation

class Brain;

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get norepinephrine level
    float getLevel() const;
    
    // Get arousal factor
    float getArousalFactor() const;
    
    // Update norepinephrine based on stress and alertness
    void update(TimestepDuration dt, float stressLevel, float alertness);
    
    // Signal stress or surprise
    void signalStress(float stressIntensity, float surprise);
    
    // Enhance sensory processing
    void enhanceSensoryProcessing(class NeuralPopulation* sensoryPopulation);
    
    // Promote memory consolidation
    void promoteMemoryConsolidation();
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm