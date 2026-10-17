#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Enhanced implementation with arousal mechanisms
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Arousal mechanisms
    void signalArousal(float level, float stress);
    void setArousalThreshold(float threshold);
    
    // State access
    float getArousalLevel() const { return pImpl->arousalLevel; }
    float getStressLevel() const { return pImpl->stressLevel; }
    float getVigilance() const { return pImpl->vigilance; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
