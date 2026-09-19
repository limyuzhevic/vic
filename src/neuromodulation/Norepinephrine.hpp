#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Proper implementation with P-impl pattern

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Increase arousal with novel stimuli
    void increaseArousal(float amount);
    void enhanceVigilance(float amount);
    
    // Detect and respond to novelty
    void processNovelty(float novelty, TimestepDuration dt);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm