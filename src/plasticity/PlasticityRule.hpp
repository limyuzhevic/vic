#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Abstract base class for plasticity rules
// PLACEHOLDER - Phase 2 will implement real plasticity rules

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // TODO PHASE 2: Implement real plasticity
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
// PLACEHOLDER - Phase 2 will implement real Hebbian learning
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
    
    // BCM (Bienenstock-Cooper-Munro) rule parameters
    void setMu(float mu);
    float getMu() const;
    void setThetaM(float theta_m);
    float getThetaM() const;
    void setThetaPlus(float theta_plus);
    float getThetaPlus() const;
    void setThetaMinus(float theta_minus);
    float getThetaMinus() const;
    
    // Calcium dynamics
    void setCalciumDecay(float decay);
    float getCalciumDecay() const;
    
    // Weight normalization
    void setWeightNormTarget(float target);
    float getWeightNormTarget() const;
    void setWeightNormRate(float rate);
    float getWeightNormRate() const;
    
    // Neuromodulator gating
    void setNeuromodulatorGating(float gating);
    float getNeuromodulatorGating() const;
    
    // Metaplasticity
    void setMetaplasticRate(float rate);
    float getMetaplasticRate() const;
    
    // Hebbian-LTD parameters
    void setLTDThreshold(float threshold);
    float getLTDThreshold() const;
    void setLTDRate(float rate);
    float getLTDRate() const;
    
    // Spike history
    void setSpikeHistorySize(float size);
    float getSpikeHistorySize() const;
    
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

// Helper functions for HebbianRule
namespace HebbianHelpers {
    void computeCovariance(const std::vector<Timestamp>& spikes, 
                          float& mean, float& variance, float n);
    void computeCrossCovariance(const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              float& cov, float n);
    float computeBCMWeightChange(float covariance, float calcium, 
                                float theta_m, float learningRate);
    float applyOjaNormalization(float delta, float weight, float weightSq);
    float normalizeWeight(float weight, float target, float rate);
}

} // namespace nlm
