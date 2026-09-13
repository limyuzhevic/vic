#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Serotonin: Mood regulation, impulsivity control, and social behavior
// Enhanced with behavioral control and decision modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Behavioral control
    void signalSocialFeedback(float socialReward);
    void signalImbalance(float behavioralBias);
    float getMoodLevel() const;
    float getImpulseControlLevel() const;
    float getSocialBehaviorLevel() const;
    
    // Configuration
    void setMoodBaseline(float baseline);
    void setImpulseThreshold(float threshold);
    void setSocialSensitivity(float sensitivity);
    void reset() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm