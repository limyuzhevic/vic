#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

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
    
    // Arousal signaling
    void signalArousal(float stimulusIntensity, float novelty);
    void enhanceVigilance(const std::vector<float>& inputPattern,
                         const std::vector<float>& threshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
