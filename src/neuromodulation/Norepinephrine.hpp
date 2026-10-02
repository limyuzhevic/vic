#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Enhanced neuromodulator with arousal and stress functions

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
    
    // Stress and arousal modulation
    void signalStress(float stressLevel);
    void signalReward(float reward);
    void signalPunishment(float punishment);
    void setArousal(float arousal);
    void setVigilance(float vigilance);
    
    // Reset
    void reset();
    
    // Get state
    float getArousal() const;
    float getVigilance() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm