#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Dopamine neuromodulator: reward and reinforcement learning signal
// PLACEHOLDER - Phase 3 will implement real neuromodulation effects
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
    
    // Get reward history
    const std::vector<float>& getRewardHistory() const;
    void clearRewardHistory();
    
    // Configuration
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Signal processing
    void applyModulation(float globalInhibition, float globalExcitation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm