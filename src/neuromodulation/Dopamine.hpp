#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <string>

namespace nlm {

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get current effects
    float getTotalSignal() const;
    const std::vector<float>& getRecentRewards() const;
    
    // Reset
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm