#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Real implementation with mood and social behavior modulation

class Serotonin {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Signal mood for positive experiences
    void signalMood(float happiness);
    
    // Modulate social behavior
    void modulateSocialBehavior(float sociality);
    
    // Get impulsivity level
    float getImpulsivity() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
