#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Noradrenaline: Arousal and vigilance
// Increases arousal, attention, vigilance

class Noradrenaline {
public:
    Noradrenaline();
    ~Noradrenaline();
    
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
    
    // Signal arousal and threat detection
    void signalArousal(float intensity, float threat);
    
    // Set vigilance level
    void setVigilance(float vigilance);
    
    // Get vigilance level
    float getVigilance() const;
    
    // Get stress response level
    float getStressResponse() const;
    
    // Enhance sensory processing
    void enhanceSensoryProcessing();
    
    // Prepare for fight-or-flight response
    void prepareFightOrFlight();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
