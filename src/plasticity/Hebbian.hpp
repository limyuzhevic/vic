#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian plasticity rule implementation header
// 
// This file defines the Hebbian class which implements activity-dependent
// synaptic plasticity based on correlated neural firing. The "fire together,
// wire together" principle is fundamental to memory formation and learning
// in biological neural systems.
// 
// Hebbian learning is a correlation-based rule where synaptic strength
// increases with the degree of pre- and post-synaptic activity correlation.
// It's particularly important for:
// - Pattern recognition and storage
// - Memory consolidation
// - Feature extraction in neural networks
// 
// Implementation details:
// - Counts spike coincidences within a temporal window
// - Applies weight changes proportional to coincidence count
// - Maintains synaptic weight bounds to prevent runaway excitation
// - Uses learning rate parameter for control of plasticity strength
// 
// TODO PHASE 2: Enhanced Hebbian implementation
// - Implement covariance rule (BCM theory)
// - Add spike-timing components (STDP-Hebbian integration)
// - Implement synaptic tagging for eligibility traces
// - Add activity-dependent learning rates

class Hebbian : public PlasticityRule {
public:
    // Default constructor creates a Hebbian rule with default parameters
    Hebbian();
    
    // Virtual destructor for proper cleanup of pImpl
    ~Hebbian() override;
    
    // Update synaptic weights based on correlated pre/post spike activity
    // 
    // This method implements the core Hebbian learning algorithm:
    // 1. Count spike coincidences within a temporal window
    // 2. Compute weight change based on coincidence count
    // 3. Apply change with bounds checking
    // 
    // Parameters:
    // - synapse: The synapse to update
    // - preSpikes: Vector of pre-synaptic spike timestamps
    // - postSpikes: Vector of post-synaptic spike timestamps
    // - dt: Simulation time step
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Apply weight change with bounds checking
    // 
    // Parameters:
    // - synapse: The synapse to modify
    // - delta: Weight change to apply (can be positive or negative)
    // 
    // Ensures weight stays within [minWeight, maxWeight] range
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get the name of this plasticity rule
    // 
    // Returns:
    // - String "Hebbian" identifying this rule type
    const char* getName() const override;
    
    // Learning rate configuration
    // 
    // The learning rate (η) determines how strongly correlated activity
    // affects synaptic strength. Typical values range from 0.001 to 0.1.
    // 
    // Parameters:
    // - rate: Learning rate (clamped to [0.0, 1.0])
    void setLearningRate(float rate);
    
    // Get current learning rate
    // 
    // Returns:
    // - Current learning rate parameter
    float getLearningRate() const;
    
    // Maximum synaptic weight configuration
    // 
    // Sets the upper bound for synaptic weights to prevent runaway excitation.
    // Typical values range from 1.0 to 10.0 depending on the neural model.
    // 
    // Parameters:
    // - maxWeight: Maximum allowed synaptic weight (clamped to [0.0, 10.0])
    void setMaxWeight(float maxWeight);
    
    // Get current maximum synaptic weight
    // 
    // Returns:
    // - Current maximum weight parameter
    float getMaxWeight() const;
    
private:
    // Forward declaration of implementation details
    // This uses the Pimpl idiom to hide implementation details
    struct Impl;
    Impl* pImpl;  // Pointer to internal implementation
};

} // namespace nlm
