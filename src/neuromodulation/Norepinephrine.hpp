#pragma once

#include "../Neuromodulator.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal signaling
    void signalArousal(float arousalStrength);
    
    // Vigilance signaling
    void signalVigilance(float vigilanceStrength);
    
    // Reset to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm