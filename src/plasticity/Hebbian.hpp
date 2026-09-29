#pragma once

#include "PlasticityRule.hpp"
#include "../../core/Types/Types.hpp"

namespace nlm {

// Hebbian learning implementation
// Implements "neurons that fire together, wire together" learning rule
// Based on covariance rule: Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
// Simplified for spike-based systems using correlated firing

class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    // Update synaptic weights based on spike timing correlations
    // Implements Hebbian learning: coincident pre and post spikes strengthen the connection
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply weight change with bounds checking
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // Learning rate parameter
    // η - learning rate (0.0 to 1.0). Higher values lead to faster learning
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Weight bounds parameters
    // Maximum absolute weight value for the synapse
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    // Weight bounds parameters
    // Minimum absolute weight value for the synapse
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    // Covariance threshold for learning
    // Minimum correlation required for significant weight changes
    void setCovarianceThreshold(float threshold);
    float getCovarianceThreshold() const;
    
    // Configure all parameters from configuration
    void configure(float learningRate, float maxWeight, float minWeight, 
                   float covarianceThreshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
