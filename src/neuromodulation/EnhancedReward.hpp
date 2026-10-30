#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Enhanced reward system with prediction error and temporal difference learning

class EnhancedReward : public Neuromodulator {
public:
    EnhancedReward();
    ~EnhancedReward() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override;
    void setTargetLevel(float target) override;
    bool isActive() const override;
    void reset() override;
    
    // Temporal difference learning
    void updatePrediction(float newPrediction, float reward, TimestepDuration dt);
    float getPredictionError() const;
    float getTemporalDifference() const;
    
    // Reward prediction
    void setPredictedValue(float value);
    float getPredictedValue() const;
    void setActualValue(float value);
    float getActualValue() const;
    
    // Reward history for learning
    void recordReward(float reward);
    float getRecentRewardAverage(int window) const;
    
    // Reward discounting
    void setDiscountFactor(float factor);
    float getDiscountFactor() const;
    
    // Reward sensitivity
    void setRewardSensitivity(float sensitivity);
    float getRewardSensitivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
