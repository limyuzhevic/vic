#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Dopamine: Real dopamine dynamics with reward prediction error and reinforcement learning
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward prediction error signaling (TD learning)
    void signalReward(float reward, float predictedValue);
    void signalPredictionError(float error);
    
    // Reward accumulation and history
    void addReward(float delta);
    float getAccumulatedReward() const;
    const std::vector<float>& getRewardHistory() const;
    
    // State-dependent modulation
    void setBaseline(float baseline);
    void setLearningRate(float rate);
    void setPhasicModulation(float gain);
    
    // Temporal dynamics parameters
    void setTimeConstant(float tcs, float tcd);  // tonic, phasic decay
    void setReleaseRate(float rate);
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm