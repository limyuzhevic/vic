#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation and social behavior
    void regulateMood(float moodLevel);
    void reduceImpulsivity(float inhibition);
    void enhanceSocialBehavior(float socialStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
