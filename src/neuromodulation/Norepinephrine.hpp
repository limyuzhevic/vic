#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// NOT consciousness - noradrenergic signaling for alertness and attention
//
// Key mechanisms:
// - Regulates arousal and alertness levels
// - Enhances signal-to-noise ratio in neural processing
// - Modulates attention and working memory capacity
// - Influences stress response and motivation

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
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
    
    // Increase arousal based on stimulus intensity
    void increaseArousal(float stimulusIntensity);
    
    // Enhance vigilance based on prediction confidence
    void enhanceVigilance(float predictionConfidence);
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Calculate current arousal level from brain systems
    float calculateArousalLevel() const;
};

} // namespace nlm