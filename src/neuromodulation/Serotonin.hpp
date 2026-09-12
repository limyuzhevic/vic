#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Mood and behavioral modulation
    void regulateMood(float& rewardValuation) const;
    void controlImpulsivity(float& decisionThreshold) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm