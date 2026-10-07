#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// Enhanced with reward-modulated learning support

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // TODO PHASE 2: Implement real plasticity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Enhanced update with reward-modulated learning support
    // Many modern plasticity rules (R-STDP, R-Hebbian) incorporate reward signals
    // to guide learning towards beneficial behaviors
    virtual void updateWithReward(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt,
                                  float rewardSignal) {
        // Default implementation: regular update followed by reward modulation
        // Subclasses can override for more sophisticated reward integration
        update(synapse, preSpikes, postSpikes, dt);
        
        if (rewardSignal != 0.0f) {
            // Apply reward modulation if available
            float currentTrace = synapse->getEligibilityTrace();
            float rewardModulatedDelta = rewardSignal * currentTrace;
            applyWeightChange(synapse, rewardModulatedDelta);
            
            // Decay eligibility trace
            synapse->decayEligibilityTrace(0.01f);
        }
    }
    
    // Apply weight change
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // Get current weight for a synapse (useful for debugging)
    virtual SynapticWeight getCurrentWeight(Synapse* synapse) const {
        return synapse ? synapse->getWeight() : 0.0f;
    }
    
    // Reset synapse state (clears eligibility traces, spike history, etc.)
    virtual void reset(Synapse* synapse) {
        if (synapse) {
            synapse->clearHistory();
            synapse->setEligibilityTrace(0.0f);
        }
    }
    
    // Get plasticity statistics
    virtual std::string getStatistics() const {
        return "PlasticityRule statistics not available";
    }

protected:
    PlasticityRule() : enabled_(true) {}
    
    // Helper function for common plasticity operations
    void applyDeltaWithBounds(Synapse* synapse, SynapticWeight delta, 
                             float minWeight = -1.0f, float maxWeight = 1.0f) {
        if (!synapse) return;
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, minWeight, maxWeight);
        synapse->setWeight(newWeight);
    }

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
