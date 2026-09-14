#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and behavioral effects
    float getMoodLevel() const;
    float getImpulsivityModulator() const;
    float getSocialBehaviorModulator() const;
    
    // Set parameters
    void setBaseline(float baseline);
    void setPeak(float peak);
    void setDecayRate(float rate);
    void setMoodGain(float gain);
    void setImpulsivityModulator(float modulator);
    void setSocialBehaviorGain(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm