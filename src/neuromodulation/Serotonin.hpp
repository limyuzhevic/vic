#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

// Serotonin: Mood and social behavior
// Phase 2 implementation - modulates mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood modulation
    void signalMoodChange(float moodDelta);
    
    // Social behavior
    void signalSocialFeedback(float socialValue);
    
    // Get mood level
    float getMoodLevel() const;
    
    // Get impulse control (high 5-HT = better control)
    float getImpulseControl() const;
    
    // Get social behavior strength
    float getSocialBehaviorStrength() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
