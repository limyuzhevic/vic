#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <string>

namespace nlm {

// Norepinephrine: Arousal and vigilance neuromodulator
// Biological role: Increases alertness, enhances signal-to-noise ratio, modulates attention

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get modulator name
    const char* getName() const { return "NE"; }
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Arousal modulation
    void enhanceAlertness();
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getSignalToNoiseRatio() const;
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm