#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <vector>

namespace nlm {

// Abstract base class for plasticity rules
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // Pure virtual function that must be implemented by derived classes
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change to synapse
    // Pure virtual function that must be implemented by derived classes
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get name of the plasticity rule
    // Pure virtual function that must be implemented by derived classes
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
class AntiHebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "AntiHebbian"; }
};

// Bienenstock-Cooper-Munro (BCM) rule
class BCMRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "BCM"; }
private:
    float theta_;  // Sliding threshold
};

// Reward-modulated STDP (R-STDP)
class RewardModulatedSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "R-STDP"; }
    
    // Configuration parameters
    void configure(float ltpWeight, float ltdWeight, float tau, float rewardWeight, float learningRate);
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
private:
    float ltpWeight_;
    float ltdWeight_;
    float timeConstant_;
    float rewardWeight_;
    float learningRate_;
    float theta_;
    float storedStdpDelta_;
};

// Spike-timing dependent plasticity with triplet interactions
class TripletSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "TripletSTDP"; }
    
    // Configuration parameters
    void configure(float weightPlus, float weightMinus, float tauPlus, float tauMinus, float minWeight, float maxWeight);
private:
    float weightPlus_;
    float weightMinus_;
    float timeConstantPlus_;
    float timeConstantMinus_;
    float minWeight_;
    float maxWeight_;
};

} // namespace nlm