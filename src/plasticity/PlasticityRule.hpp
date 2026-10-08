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

// Original Hebbian plasticity rule: "neurons that fire together, wire together"
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
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    void setCovarianceThreshold(float threshold);
    float getCovarianceThreshold() const;
    
    void setActivityBaseline(float baseline);
    float getActivityBaseline() const;
    
    void setAdaptationRate(float rate);
    float getAdaptationRate() const;
    
    void setActivityWindow(float window);
    float getActivityWindow() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Optimized Hebbian rule for better performance
class OptimizedHebbian : public PlasticityRule {
public:
    OptimizedHebbian();
    ~OptimizedHebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Performance parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    void setActivityWindow(float window);
    float getActivityWindow() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
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
    
    void setThreshold(float threshold);
    float getThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Bienenstock-Cooper-Munro (BCM) rule
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
    
    void setThreshold(float threshold);
    float getThreshold() const;
    
    void setActivityWindow(float window);
    float getActivityWindow() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Reward modulation factor
    void setRewardFactor(float factor);
    float getRewardFactor() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Original STDP implementation
class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Optimized STDP for better performance
class OptimizedSTDP : public PlasticityRule {
public:
    OptimizedSTDP();
    ~OptimizedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Performance parameters
    void setPreTraceDecay(float decay);
    float getPreTraceDecay() const;
    void setPostTraceDecay(float decay);
    float getPostTraceDecay() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Triplet STDP with advanced trace mechanisms
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Triplet trace parameters
    void setPreTraceDecay(float decay);
    float getPreTraceDecay() const;
    void setPostTraceDecay(float decay);
    float getPostTraceDecay() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
