#pragma once

#include "Neuromodulator.hpp"
#include "../environment/Environment.hpp"

namespace nlm {

class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    // Neuromodulator interface
    const char* getName() const override { return "Reward"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward-specific functionality
    void computeReward(const Environment& environment, 
                      const nlm::Observation& observation);
    float getTotalReward() const { return totalReward; }
    int getStepsSinceLastReward() const { return stepsSinceLastReward; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float totalReward;
    int stepsSinceLastReward;
};

} // namespace nlm