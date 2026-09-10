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
    void initialize(class Brain* brain) override;
    
    // Mood regulation
    void modulateMood(float environmentalRiches);
    
    // Behavioral inhibition
    void modulateImpulsivity(float urgency);
    
    // Social behavior
    void modulateSocialBehavior(float socialContext);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
