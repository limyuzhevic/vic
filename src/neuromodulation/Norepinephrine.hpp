#pragma once

#include "Neuromodulator.hpp"
#include <vector>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Real-time norepinephrine dynamics for alertness modulation,
// stress response, and adaptive attention allocation

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
    
    // Arousal-related functions
    void signalArousal(float intensity);
    
    // Vigilance and alertness functions
    void updateVigilance(float level);
    void enhanceAlertness(float factor);
    
    // Integration with cognitive systems
    void integrateWithAttention(float attentionLevel, float taskComplexity);
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
