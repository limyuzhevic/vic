#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

class Brain;

// Norepinephrine: Arousal and vigilance neuromodulator
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
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Signal arousal events
    void signalArousal(float intensity);
    
    // Signal stress responses
    void signalStressResponse(float magnitude);
    
    // Signal vigilance changes
    void signalVigilance(float focus);
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm