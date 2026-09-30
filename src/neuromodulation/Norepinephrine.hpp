#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Enhanced implementation with real stress/arousal mechanisms
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
    void setArousal(float arousalLevel);
    float getArousal() const;
    
    // Stress response
    void triggerStressResponse(float stressLevel);
    
    // Locus coeruleus firing rate
    void setLCFireRate(float rate);
    float getLCFireRate() const;
    
    // reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm