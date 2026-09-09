#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
class Hebbian : public PlasticityRule {
public:
    static constexpr float COHERENCE_WINDOW_SECONDS = 1000.0f;    // Time window for rate calculation (ms)
    static constexpr float COHERENCE_WINDOW_MS = 20.0f;          // Spike correlation window (ms)
    static constexpr float MIN_POST_RATE_FOR_LEARNING = 0.01f;   // Minimum post-synaptic rate for learning
    static constexpr float LEARNING_THRESHOLD = 0.1f;            // BCM-like learning threshold
    
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    void setLearningThreshold(float threshold);
    float getLearningThreshold() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
