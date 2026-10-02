#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// Provides common interface for synaptic plasticity
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // Implements biological plasticity rules
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change with biological constraints
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // Get learning rate for this rule
    virtual float getLearningRate() const = 0;
    
    // Set learning rate
    virtual void setLearningRate(float rate) = 0;
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// Implements covariance-based learning with synaptic normalization
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
    void setLearningRate(float rate) override;
    float getLearningRate() const override;
    
    // Hebbian-specific parameters
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    void setCovarianceThreshold(float threshold);
    float getCovarianceThreshold() const;
    void setUseNormalization(bool useNorm);
    bool getUseNormalization() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// Implements "out-of-phase" learning for inhibitory plasticity
class AntiHebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "AntiHebbian"; }
    float getLearningRate() const override { return learningRate_; }
    void setLearningRate(float rate) override { learningRate_ = rate; }
    
private:
    float learningRate_ = 0.01f;
};

// Bienenstock-Cooper-Munro (BCM) rule
// Implements sliding threshold Hebbian learning for stable selectivity
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "BCM"; }
    
    // BCM parameters
    void setThetaPlus(float thetaPlus);  // Threshold for potentiation
    float getThetaPlus() const;
    void setThetaMinus(float thetaMinus);  // Threshold for depression
    float getThetaMinus() const;
    void setTauTheta(float tauTheta);  // Theta dynamics time constant
    float getTauTheta() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
