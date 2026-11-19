#pragma once

#include "PlasticityRule.hpp"
#include "../core/Random/Random.hpp"

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
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    // Access to random number generator for exploration
    RandomGenerator& getRandomGenerator() { return *pImpl->rng; }
    const RandomGenerator& getRandomGenerator() const { return *pImpl->rng; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
