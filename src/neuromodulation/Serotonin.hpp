#pragma once

#include "../core/Types/Types.hpp"
#include <string>

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
    
    // Social signaling
    void signalSocialContext(float contextStrength);
    void reduceImpulsivity(float reduction);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
