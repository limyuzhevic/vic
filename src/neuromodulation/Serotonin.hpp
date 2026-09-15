#pragma once

#include "Neuromodulator.hpp"

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
    
    // Social behavior modulation
    void signalSocialReward(float reward);
    void signalSocialBonding(float bonding);
    void updateSocialContext(float context);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm