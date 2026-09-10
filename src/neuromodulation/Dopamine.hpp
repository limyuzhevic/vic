#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Dopamine neuromodulator
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override { return "DA"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, float novelty = 0.0f, 
                float predictionError = 0.0f, float reward = 0.0f) override;
    float getExcitabilityModulator() const override;
    float getAttentionModulator() const override;
    float getMemoryModulator() const override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Prediction error tracking
    void recordPredictionError(float error);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm