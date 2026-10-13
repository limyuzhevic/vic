#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Implemented with biological plausibility for stress and alertness responses

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Signal alertness or stress
    void signalAlert(float intensity);
    void signalStress(float intensity);
    
    // Get arousal and vigilance levels
    void setArousalLevel(float arousal);
    float getArousalLevel() const;
    
    void setVigilance(float vigilance);
    float getVigilance() const;
    
    // Get attentional focus
    float getAttentionalFocus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
