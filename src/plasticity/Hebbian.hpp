#pragma once

#include "PlasticityRule.hpp"
#include <vector>

namespace nlm {

// Hebbian learning implementation
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    // Disable copying, enable moving
    Hebbian(const Hebbian&) = delete;
    Hebbian& operator=(const Hebbian&) = delete;
    Hebbian(Hebbian&&) noexcept;
    Hebbian& operator=(Hebbian&&) noexcept;
    
    // Update synaptic weight based on spike history
    // Uses covariance rule: Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Configuration methods
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    void setCovarianceThreshold(float threshold);
    float getCovarianceThreshold() const;
    
    // Adaptive learning rate control
    void adaptLearningRate(float preSpikeRate, float postSpikeRate, TimestepDuration dt);
    
    // Get current statistics
    float getAverageCorrelation() const;
    float getLearningEfficiency() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Hebbian learning statistics structure
struct HebbianLearningStats {
    float averageCorrelation;        // Average spike correlation (0-1)
    float learningEfficiency;        // Effectiveness of learning (0-1)
    float totalWeightChange;         // Cumulative weight changes
    float baselineActivity;          // Average baseline activity
    uint32_t updateCount;            // Number of successful updates
    uint32_t failedUpdates;          // Number of failed updates
    
    HebbianLearningStats()
        : averageCorrelation(0.0f), learningEfficiency(0.0f), totalWeightChange(0.0f),
          baselineActivity(0.0f), updateCount(0), failedUpdates(0) {}
};

} // namespace nlm
