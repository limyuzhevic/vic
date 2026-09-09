#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// PHASE 1: Full implementation with biological plausibility
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // NE-specific functions for arousal and vigilance
    float getArousalLevel() const;
    void signalArousal(float intensity, bool novelEvent);
    float getVigilanceModulator() const;
    void maintainVigilance(bool isAlertState);
    float getLocusCoeruleusGain() const;
    
    // NE effects on global brain state
    void increaseAlertness() const;
    void enhanceSignalToNoiseRatio() const;
    void prepareForAction() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm