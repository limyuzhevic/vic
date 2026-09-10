#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <string>

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Phase 6: Real implementation for reward-based learning and motivation
// Implements dopaminergic signaling for reward prediction error and value learning

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    // Neuromodulator interface implementation
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Value learning
    void computeValue(float reward, TimestepDuration dt);
    float getValue() const;
    
    // Learning modulation based on reward prediction error
    float getLearningModulationFactor() const;
    
    // Reward history for temporal difference learning
    void recordRewardHistory(float reward, Timestamp time);
    float getAverageReward() const;
    
    // Dopamine tone regulation (baseline level)
    void setBaselineTone(float tone);
    float getBaselineTone() const;
    
    // Reward anticipation signals
    void signalExpectedReward(float reward);
    float getExpectedReward() const;
    
    // Reset for new learning episode
    void reset() override;
    
    // Phase 6: Integration with plasticity systems
    void integrateWithSTDP(STDP* stdp, float learningRate = 0.01f);
    void integrateWithHebbian(Hebbian* hebbian, float learningRate = 0.005f);
    
    // Phase 6: Behavioral reinforcement
    ActionType selectActionWithReinforcement(
        const std::vector<ActionType>& actions,
        const std::vector<float>& actionValues
    );
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace nlm
