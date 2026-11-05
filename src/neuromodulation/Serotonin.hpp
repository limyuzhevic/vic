#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Phase 2: Real serotonin dynamics affecting mood and behavioral control
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate mood and behavioral control
    void improveMood(float moodLevel);
    void reduceImpulsivity(float controlStrength);
    void facilitateSocialBehavior(float socialBonding);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm