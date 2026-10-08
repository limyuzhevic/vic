#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Enhanced with comprehensive arousal and signal detection

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced norepinephrine effects
    
    // Modulation for arousal and vigilance
    float getArousalModulation() const;
    
    // Modulation for signal detection in noisy environments
    float getSignalDetectionFactor() const;
    
    // Modulation for locus coeruleus firing rate
    float getLocusCoeruleusModulation() const;
    
    // Modulation for attentional focus
    float getAttentionalFocusModulation() const;
    
    // Modulation for memory encoding under stress
    float getStressMemoryModulation() const;
    
    // Modulation for fight-or-flight response
    float getFightOrFlightModulation() const;
    
    // Set norepinephrine parameters
    void setBaseline(float baseline);
    void setArousalLevel(float arousal);
    void setVigilanceLevel(float vigilance);
    void setLocusCoeruleusGain(float gain);
    
    // Get norepinephrine state information
    float getBaseline() const;
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getLocusCoeruleusGain() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm