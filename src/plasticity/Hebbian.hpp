#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

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
    
    // Hebbian learning parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    // Update parameters from config
    void configure(float learningRate, float timeConstant);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
