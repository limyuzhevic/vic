#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Modulates mood, affect, impulsivity, and social behaviors

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
    float getMoodLevel() const;
    void setMoodLevel(float mood);
    
    // Impulse control
    float getImpulseControl() const;
    void setImpulseControl(float control);
    
    // Social behavior
    float getSocialBehavior() const;
    void setSocialBehavior(float social);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
