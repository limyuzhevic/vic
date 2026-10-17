#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Biologically inspired neuromodulator that regulates mood and social interactions
// Mechanisms:
// - Enhance mood and positive affect
// - Reduce impulsivity and aggression
// - Promote social bonding
// - Regulate sleep and appetite

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get serotonin level
    float getLevel() const;
    void setLevel(float level);
    
    // Update serotonin dynamics
    void update(TimestepDuration dt);
    
    // Signal mood boost from positive experiences
    void signalMood(float moodSignal);
    
    // Modulate social behavior
    void modulateSocialBehavior(float socialLevel);
    
    // Get current well-being level
    float getWellBeing() const;
    
    // Get plasticity factor for this neuromodulator
    float getPlasticityFactor() const;
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm