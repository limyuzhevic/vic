#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Adenosine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float accumulationRate;
    float clearanceRate;
    float fatigueThreshold;
    float recoveryThreshold;
    float buildupRate;
    float energyCost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.01f), accumulationRate(0.001f)
            , clearanceRate(0.02f), fatigueThreshold(0.5f), recoveryThreshold(0.3f)
            , buildupRate(0.005f), energyCost(0.1f) {}
};

class Adenosine : public Neuromodulator {
public:
    Adenosine();
    ~Adenosine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Rest-related functions
    void signalRest(float energySaved) override;
    void signalWakeUp() override;
    void signalMetabolicDemand(float demand) override;
    bool shouldTriggerRest() const override;
    bool shouldEndRest() const override;
    float getRestDrive() const override { return getFatigueLevel(); }
    
    // Additional adenosine-specific functions
    void setPeak(float peak) { pImpl->peak = peak; }
    float getPeak() const { return pImpl->peak; }
    void setClearanceRate(float rate) { pImpl->clearanceRate = rate; }
    float getClearanceRate() const { return pImpl->clearanceRate; }
    void setFatigueThreshold(float threshold) { pImpl->fatigueThreshold = threshold; }
    float getFatigueThreshold() const { return pImpl->fatigueThreshold; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm