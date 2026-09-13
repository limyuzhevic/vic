#pragma once

#include "../core/Types/Types.hpp"
#include "../cognition/SocialLearning.hpp"
#include <string>
#include <algorithm>
#include <cmath>

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
    
    // Mood and social behavior effects
    void modulateMood(float moodLevel);
    void modulateImpulsivity(float impulsivityLevel);
    void enhanceSocialBehavior(class SocialLearning* socialLearning, float socialBoost = 1.0f);
    
    // Integration with other systems
    void integrateWithDopamine(float dopamineLevel);
    void integrateWithAcetylcholine(float achLevel);
    void integrateWithNorepinephrine(float neLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
