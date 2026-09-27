#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    Hebbian(Hebbian&&) noexcept;
    ~Hebbian() override;
    
    Hebbian& operator=(Hebbian&&) noexcept;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Configure Hebbian parameters
    void configure(float learningRate, float maxWeight, float minWeight);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
