#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Norepinephrine: Arousal, vigilance, and stress response
// Advanced implementation with adaptive dynamics
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Stress and arousal modulation
    void signalStress(float stressLevel);
    void signalNovelty(float noveltyLevel);
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getStressLevel() const;
    
    // Configuration
    void setArousalBaseline(float baseline);
    void setVigilanceThreshold(float threshold);
    void setStressSensitivity(float sensitivity);
    void reset() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm