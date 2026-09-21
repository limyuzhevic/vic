#include "Serotonin.hpp"
#pragma once

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Social and mood-related signaling
    void signalMood(float mood);
    void signalSocialBehavior(float socialSignal);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm