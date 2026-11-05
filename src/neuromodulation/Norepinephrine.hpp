#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Phase 2: Real norepinephrine dynamics affecting arousal and signal detection
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate arousal and vigilance
    void increaseArousal(float levelIncrease);
    void enhanceSignalDetection(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm