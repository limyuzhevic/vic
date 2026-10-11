#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

struct Serotonin::Impl;

class Serotonin : public Neuromodulator {
    struct Impl;
    Impl* pImpl;
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    void reset() override;
    
    // Modulate mood and social behavior
    void modulateMood(float level);
    void reduceImpulsivity(float reduction);
    void enhanceSocialBehavior(float strength);
    void setSocialBonding(float bonding);
};

} // namespace nlm