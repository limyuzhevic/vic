#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// TODO PHASE 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and social signaling
    void signalMood(float mood);
    void modifyImpulsivity(float impulseChange);
    void enhanceSocialBehavior(float socialBoost);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm