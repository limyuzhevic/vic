#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

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
    
    // Parameters
    void setThreshold(float threshold);
    float getThreshold() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
