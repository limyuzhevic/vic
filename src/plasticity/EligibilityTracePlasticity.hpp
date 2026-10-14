#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

// Eligibility trace plasticity rule
// Implements reward-modulated learning with eligibility traces
// 
// Background:
// Biological motivation: Eligibility traces in synapses allow for delayed reward signals
// to affect synaptic changes, matching how dopamine signals work in the brain.
// When a reward is received, it "traces back" along the synaptic chain to strengthen
// connections that were recently active.
//
// Implementation details:
// - Each synapse maintains an eligibility trace that decays over time
// - When reward is received, the trace is multiplied by reward signal
// - The trace accumulates pre-post spike correlations
// - Final weight change = trace * reward signal
//
// Advantages over standard STDP:
// - Can work with delayed rewards
// - More biologically plausible for reinforcement learning
// - Allows for integration over longer time windows
// - Can handle sparse rewards

class EligibilityTracePlasticity : public PlasticityRule {
public:
    EligibilityTracePlasticity();
    ~EligibilityTracePlasticity() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Eligibility trace parameters
    void setTraceDecayRate(float decayRate);    // Rate at which trace decays (per ms)
    float getTraceDecayRate() const;
    void setTraceLearningRate(float learningRate);  // Learning rate for trace updates
    float getTraceLearningRate() const;
    
    // Reward modulation parameters
    void setRewardScale(float scale);           // Scaling factor for reward signals
    float getRewardScale() const;
    void setMaxTrace(float maxTrace);           // Maximum trace value (prevents overflow)
    float getMaxTrace() const;
    
    // Update eligibility trace based on spike pairs
    void updateEligibilityTrace(Synapse* synapse, 
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt);
    
    // Apply reward signal to update synapses
    void applyRewardSignal(float reward, const std::vector<Synapse*>& synapses);
    
    // Reset eligibility trace for a synapse
    void resetEligibilityTrace(Synapse* synapse);
    
    // Get current eligibility trace for a synapse
    float getEligibilityTrace(Synapse* synapse) const;
    
    // Batch update for efficiency (for simulation performance)
    void updateBatch(const std::vector<std::pair<Synapse*, std::vector<Timestamp>>>& preSpikes,
                    const std::vector<std::pair<Synapse*, std::vector<Timestamp>>>& postSpikes,
                    TimestepDuration dt);
    
    // Apply reward batch
    void applyRewardBatch(float reward, const std::vector<Synapse*>& synapses);
    
    // Configuration from system
    void configureFromConfig(std::shared_ptr<Config> config);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
