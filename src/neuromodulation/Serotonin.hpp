#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced implementation with real mood regulation mechanisms
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation
    void setMood(float mood);
    float getMood() const;
    
    // Social behavior modulation
    void modulateSocialBehavior(float socialCohesion);
    
    // Raphe nucleus activity
    void setRapheActivity(float activity);
    float getRapheActivity() const;
    
    // reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm