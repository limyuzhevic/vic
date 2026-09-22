#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>

namespace nlm {

class Neuromodulator;

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Real dopamine dynamics
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void processRewardContext(const std::string& context);
    
    // Get dopamine history for prediction error computation
    const std::vector<float>& getRecentActivity() const;
    
    // Get phasic response magnitude
    float getPhasicResponse() const;
    
    // Get tonic level
    float getTonicLevel() const;
    
    // Configure dopamine parameters
    void setBaseline(float baseline);
    void setPhasicGain(float gain);
    void setTemporalFilter(float tau);
    void setAttentionCoupling(float coupling);
    
    // Connection to attention and memory
    void updateAttentionWeight(float weight);
    void updateMemoryIntegration(float integration);
    
    // Neuromodulator interface implementations
    void updateAttention(float currentLevel, float &attentionWeight) override;
    void updateWorkingMemory(float currentLevel, float &memoryStability) override;
    void updateDevelopmentalPlasticity(float currentLevel, float &developmentalFactor) override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm