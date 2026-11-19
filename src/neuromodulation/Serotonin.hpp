#pragma once

#include "../core/Types/Types.hpp"

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
    
    // Mood regulation and social behavior
    void modulateMood(float level);
    void regulateImpulsivity(float factor);
    void enhanceSocialLearning(float factor);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
