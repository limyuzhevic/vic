#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning constants
namespace HebbianConstants {
    // Base Hebbian parameters
    constexpr float BASE_LEARNING_RATE = 0.01f;      // Base learning rate for Hebbian plasticity
    constexpr float BASE_MAX_WEIGHT = 1.0f;          // Maximum synaptic weight
    constexpr float BASE_MIN_WEIGHT = -1.0f;         // Minimum synaptic weight
    
    // Correlation thresholds
    constexpr float DEFAULT_CORRELATION_THRESHOLD = 0.0f;  // For covariance rule
    
    // Learning rate bounds
    constexpr float MIN_LEARNING_RATE = 0.0f;       // No learning
    constexpr float MAX_LEARNING_RATE = 1.0f;       // Maximum learning rate
    constexpr float MAX_WEIGHT_LIMIT = 10.0f;       // Safety limit for weights
    
    // Spike correlation parameters
    constexpr float SPIKE_CORRELATION_WINDOW = 100.0f;  // 100ms correlation window for Hebbian
    constexpr float MIN_SPIKE_ACTIVITY = 0.1f;         // Minimum activity to consider correlated
    
    // Stability parameters
    constexpr float WEIGHT_CHANGE_THRESHOLD = 1e-6f;   // Minimum weight change to consider significant
}

// Hebbian learning implementation
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Hebbian parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
