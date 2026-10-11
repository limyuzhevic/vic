#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
// Implements the classic Hebb's rule: "Neurons that fire together, wire together"
// Also known as "Hebbian plasticity" or "co-activation dependent learning"
// If pre- and post-synaptic neurons fire simultaneously (within time window): strengthen connection
// Mathematical formulation: Δw = η × (pre × post)
// Where η is learning rate, pre is pre-synaptic activity, post is post-synaptic activity
// Biological basis: NMDA receptor-dependent synaptic strengthening
// 
// Note: Traditional Hebbian learning can lead to unbounded weight growth
// This implementation includes weight normalization and bounds to prevent instability

class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    // Update synaptic weights based on co-activation
    // Implements Hebbian learning: strengthen synapses when pre- and post-synaptic
    // neurons fire together (co-activation)
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   preSpikes - Vector of pre-synaptic spike times (used for activity calculation)
    //   postSpikes - Vector of post-synaptic spike times (used for activity calculation)
    //   dt - Simulation timestep (used for rate calculations)
    // Note: Modern implementations often use sliding windows or rate-based models
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply a weight change (e.g., from reward-modulated plasticity)
    // Parameters:
    //   synapse - Target synapse for weight modification
    //   delta - Weight change to apply (positive for potentiation, negative for depression)
    // Note: This allows Hebbian learning to be combined with other plasticity mechanisms
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get plasticity rule name
    // Returns: String identifier ("Hebbian")
    const char* getName() const override;
    
    // Hebbian learning parameters
    
    // Set learning rate (η)
    // Parameters: rate - Learning rate (typically 0.001-0.01)
    // Controls strength of synaptic modification
    // Higher rate = faster learning but less stability
    void setLearningRate(float rate);
    
    // Get current learning rate
    float getLearningRate() const;
    
    // Set maximum synaptic weight
    // Parameters: maxWeight - Maximum allowed weight (typically 1.0 for excitatory)
    // Prevents unbounded weight growth
    // Helps maintain network stability
    void setMaxWeight(float maxWeight);
    
    // Get current maximum weight
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
