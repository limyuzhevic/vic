#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward: Reward neuromodulator for reinforcement learning
// Computes reward signals and prediction errors
class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    // Neuromodulator interface
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward computation
    float computeReward(const class Observation& observation) const;
    
    // Reward prediction error
    void computePredictionError(float expectedReward);
    float getRewardPredictionError() const;
    
    // Reward accumulation and storage
    float getValue() const;
    void setValue(float value);
    void add(float delta);
    void reset();
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Integration with brain systems
    void integrateWithMemory(class Brain* brain);
    void applyToAttention(class AttentionalSelection* attention) const;
    
    // Signal reward and prediction error to dopamine system
    void signalToDopamine(class Dopamine* dopamine) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
