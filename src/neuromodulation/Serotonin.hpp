#pragma once

#include "Neuromodulator.hpp"
#include <vector>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

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
    void signalPositiveSocial(float socialReward);
    void reduceImpulsivity(float currentImpulsivity);
    void enhanceSocialLearning(const std::vector<float>& socialPattern);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm