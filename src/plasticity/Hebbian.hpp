#pragma once

#include "PlasticityRule.hpp"
#include <vector>

namespace nlm {

// Hebbian learning implementation
//
// This plasticity rule implements the classical Hebbian learning principle:
// "Neurons that fire together, wire together." Hebbian learning is based on
// the correlation between pre-synaptic and post-synaptic activity.
//
// Biological and theoretical foundations:
// - Donald Hebb (1949): "If the synaptic connections between two neurons
//   are strong enough to permit impulses to fire in one, then they will
//   fire in the other" (Hebbian learning)
// - Sparse coding and efficient representation
// - Competitive learning and normalization
// - Homeostatic plasticity for stability
//
// Mathematical formulation:
// - Δw ∝ x_pre * x_post (simple Hebbian)
// - Δw ∝ (x_pre - ⟨x_pre⟩) * (x_post - ⟨x_post⟩) (normalized Hebbian)
// - Includes competitive terms to prevent runaway excitation
//
// Applications:
// - Pattern separation and association
// - Memory encoding in recurrent networks
// - Feature detection in sensory systems
// - Invariant representation learning
//
// Key characteristics:
// - Activity-dependent strengthening
// - Competitive dynamics between synapses
// - Covariance-based learning
// - Typically unbounded without homeostatic mechanisms
//
// @note This is a Phase 1 placeholder implementation
//       Real Hebbian learning should incorporate:
//       - Spike-timing precision (STDP-like components)
//       - Neuromodulatory gating
//       - Homeostatic regulation
//       - Target-specific learning
class Hebbian : public PlasticityRule {
public:
// Create Hebbian learning system with default parameters
//
// @note Default parameters:
//       - Learning rate: 0.01 (moderate learning speed)
//       - Maximum weight: 1.0 (standard normalization)
//
// @note These defaults can be adjusted based on:
//       - Network stability requirements
//       - Learning speed requirements
//       - Target network properties
//       - Behavioral task demands
//
// @warning Learning rate too high can cause instability
//         Learning rate too low can prevent effective learning
// Destructor
//
// @note Automatic memory management via Pimpl idiom
    
// Update synaptic weights based on activity correlation
//
// @param synapse Target synapse to modify
// @param preSpikes Timestamps of presynaptic spikes
// @param postSpikes Timestamps of postsynaptic spikes
// @param dt Simulation timestep duration
//
// @note Hebbian learning algorithm:
//       1. Calculate covariance between pre- and post-synaptic activity
//       2. Add competitive pressure from other synapses
//       3. Apply activity-dependent modulation
//       4. Generate weight changes proportional to activity correlation
//
// @note Implementation should:
//       1. Normalize activity for stable learning
//       2. Include competitive terms to prevent saturation
//       3. Consider synaptic eligibility traces
//       4. Respect weight bounds (via applyWeightChange())
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;

    // Apply weight change to synapse
//
// @param synapse Target synapse to modify
// @param delta Weight change amount (positive or negative)
//
// @note Same as STDP::applyWeightChange()
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;

    // Get plasticity rule name
//
// @return const char* String "Hebbian" identifying this plasticity rule
    const char* getName() const override;
    
// Set learning rate for Hebbian plasticity
//
// @param rate Learning rate (typically 0.001 to 0.1)
//
// @note Learning rate controls:
//       - Speed of synaptic change
//       - Stability vs. plasticity balance
//       - Network dynamics
//       - Learning efficiency
//
// @note Typical values:
//       - 0.001: Slow, stable learning
//       - 0.01: Moderate learning
//       - 0.1: Fast learning (potentially unstable)
//
// @warning Learning rate too high can cause instability
//         Learning rate too low can prevent effective learning
    void setLearningRate(float rate);

    // Get current learning rate
//
// @return float Current learning rate
    float getLearningRate() const;

    // Set maximum synaptic weight
//
// @param maxWeight Maximum weight value (typically 1.0)
//
// @note Maximum weight controls:
//       - Learning ceiling
//       - Network capacity
//       - Saturation behavior
//       - Competitive dynamics
//
// @note Maximum weight works with:
//       - Normalization schemes
//       - Competition algorithms
//       - Homeostatic mechanisms
//       - Weight clipping
    void setMaxWeight(float maxWeight);

    // Get current maximum weight
//
// @return float Current maximum weight
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
// Calculate activity covariance between pre- and post-synaptic spikes
//
// @param preSpikes Timestamps of presynaptic spikes
// @param postSpikes Timestamps of postsynaptic spikes
// @param dt Simulation timestep duration
//
// @return float Covariance measure between pre- and post-synaptic activity
//
// @note Covariance measures:
//       - Joint activity patterns
//       - Temporal correlation
//       - Synaptic strength predictor
//       - Learning signal strength
//
// @note Implementation typically:
//       1. Convert spike trains to activity variables
//       2. Compute time-averaged values
//       3. Calculate (x_pre - ⟨x_pre⟩) * (x_post - ⟨x_post⟩)
//       4. Apply normalization factors
//
// @note High covariance indicates:
//       - Strong synaptic association
//       - Coordinated activity
//       - Learning potential
//       - Pattern strength
    float calculateCovariance(const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) const;

    // Calculate competitive pressure from other synapses in the network
//
// @param synapse Target synapse
//
// @return float Competitive pressure value (0.0 to 1.0)
//
// @note Competitive pressure:
//       - Normalizes synaptic changes across network
//       - Prevents runaway excitation
//       - Ensures resource allocation
//       - Maintains network stability
//
// @note Typical implementation:
//       - Sum of competing synaptic activities
//       - Normalization by total network activity
//       - Modulates learning rate based on competition
//       - Prevents saturation of strong synapses
    float calculateCompetitivePressure(const Synapse* synapse) const;

    // Calculate activity deviation from network average
//
// @param preSpikes Timestamps of presynaptic spikes
// @param postSpikes Timestamps of postsynaptic spikes
//
// @return float Activity deviation measure
//
// @note Activity deviation:
//       - Measures how unusual the activity pattern is
//       - Identifies novel or salient events
//       - Guides learning prioritization
//       - Reduces redundant learning
//
// @note Typical implementation:
//       1. Subtract average activity
//       2. Apply gain modulation
//       3. Enhance learning for novel patterns
//       4. Suppress learning for common patterns
    float calculateActivityDeviation(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes) const;
};

} // namespace nlm
