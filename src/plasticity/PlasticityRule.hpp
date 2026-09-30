#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// Base class for real plasticity rule implementations (STDP, Hebbian, etc.)

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
// Update synaptic weights based on pre/post synaptic activity
// Real plasticity implementations (STDP, Hebbian, etc.) override this method
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
// Alternative implementation - main Hebbian is in Hebbian.cpp
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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// PLACEHOLDER - Phase 2
class AntiHebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "AntiHebbian"; }
};

// Bienenstock-Cooper-Munro (BCM) rule
// PLACEHOLDER - Phase 2
class BCMRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "BCM"; }
};

} // namespace nlm
