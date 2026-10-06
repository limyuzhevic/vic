#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// Implements real synaptic plasticity mechanisms for artificial developmental brain

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// Implements symmetric Hebbian learning for cooperative synaptic strengthening
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float weight);
    float getMaxWeight() const;
    void setMinWeight(float weight);
    float getMinWeight() const;
    void setTimeWindow(float window);
    float getTimeWindow() const;
    void setEligibilityTraceDecay(float decay);
    float getEligibilityTraceDecay() const;
    void setRewardModulation(float modulation);
    float getRewardModulation() const;
    
    // Configuration
    void configure(float learningRate, float maxWeight, float minWeight,
                   float timeWindow, float traceDecay, float rewardModulation);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// Implements anti-correlation learning for competition and synaptic weakening
class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float weight);
    float getMaxWeight() const;
    void setMinWeight(float weight);
    float getMinWeight() const;
    void setTimeWindow(float window);
    float getTimeWindow() const;
    
    // Configuration
    void configure(float learningRate, float maxWeight, float minWeight, float timeWindow);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Bienenstock-Cooper-Munro (BCM) rule
// Implements homeostatic plasticity with dynamic threshold for stability and criticality
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float weight);
    float getMaxWeight() const;
    void setMinWeight(float weight);
    float getMinWeight() const;
    void setKappa(float kappa);        // BCM threshold variable
    float getKappa() const;
    void setMu(float mu);              // Sliding threshold rate
    float getMu() const;
    void setTimeWindow(float window);
    float getTimeWindow() const;
    
    // Configuration
    void configure(float learningRate, float maxWeight, float minWeight,
                   float kappa, float mu, float timeWindow);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
