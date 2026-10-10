#pragma once

#include "Neuromodulator.hpp"
#include <memory>
#include <string>
#include <vector>

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
    
    // Getters for biological analysis
    float getPrediction() const;
    void setPrediction(float prediction);
    
    float getEligibilityTrace() const;
    
    const std::vector<float>& getLevelHistory() const;
    const std::vector<float>& getPredictionHistory() const;
    const std::vector<float>& getRewardHistory() const;
    
    float getLearningRate() const;
    void setLearningRate(float rate);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
