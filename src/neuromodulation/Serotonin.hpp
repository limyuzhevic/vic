#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood modulation
    float getMoodLevel() const;
    void signalPleasure();
    void signalAnhedonia();
    
    // Social behavior
    float getSocialApproach() const;
    void signalSocialReward();
    
    // Impulse control
    float getImpulseControl() const;
    void signalImpulse();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm