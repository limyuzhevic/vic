#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Affects attention, memory consolidation, working memory, cognitive processes
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
    
    // Cognitive effects
    float getAttentionModulator() const;
    float getMemoryModulator() const;
    float getWorkingMemoryModulator() const;
    
    // Set parameters
    void setBaseline(float baseline);
    void setPeak(float peak);
    void setReleaseRate(float rate);
    void setAttentionGain(float gain);
    void setMemoryGain(float gain);
    void setWorkingMemoryGain(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm