#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Phase 2 implementation - modulates arousal, alertness, and focus
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal modulation
    void signalArousal(float arousal);
    
    // Stress response
    void signalStress(float stressLevel);
    
    // Get arousal level
    float getArousalLevel() const;
    
    // Get focus width (high NE = narrow focus)
    float getFocusWidth() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
