#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior neuromodulator
// Phase 2: Mood modulation, impulsivity control, social behavior effects

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and behavioral modulation
    void signalMoodChange(float mood);
    void signalSatiation(float level);
    void signalSocialContext(float context);
    
    // Get internal states
    float getMoodLevel() const;
    float getImpulsivityLevel() const;
    float getSocialBehaviorModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm