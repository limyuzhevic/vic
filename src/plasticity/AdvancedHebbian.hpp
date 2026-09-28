#pragma once

#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Competitive Hebbian with normalization: "only the best connections win"
//
// This advanced Hebbian variant implements competitive learning where neurons
// compete for synaptic resources, preventing runaway excitation while maintaining
// learning efficiency. The implementation includes:
//
// 1. Competitive inhibition: Only the strongest synapses are strengthened
// 2. Normalization: Total synaptic input to postsynaptic neuron is bounded
// 3. Activity-dependent competition: Synaptic eligibility is activity-dependent
// 4. Soft competition: Gradual rather than winner-take-all selection
//
// Biological basis:
// - Implementing homeostatic plasticity mechanisms
// - Modeling synaptic competition in developing neural circuits
// - Reflecting natural limits on synaptic resources
//
// Mathematical formulation:
// For each pre-synaptic neuron, compute contribution to postsynaptic activity:
//   w_i' = w_i * (1 + η * (coactivity_i - baseline))
// Where weights are normalized to prevent saturation:
//   w_i_normalized = w_i / (sum of all weights)

class CompetitiveHebbian : public Hebbian {
public:
    CompetitiveHebbian();
    ~CompetitiveHebbian() override;
    
    // Update synaptic weights with competitive learning
    //
    // Implements O(N log N) competitive Hebbian learning with normalization
    // to prevent runaway excitation while maintaining pattern separation
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply weight change with competitive normalization
    //
    // Competitive application where synaptic modifications are scaled based on
    // the overall synaptic weight distribution to maintain stability
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get the human-readable name of this plasticity rule
    const char* getName() const override;
    
    // Competitive learning parameters
    void setCompetitionFactor(float factor);  // Competition strength (0.0-1.0)
    float getCompetitionFactor() const;
    
    // Normalization parameters
    void setTargetSum(float target);        // Target total synaptic weight
    float getTargetSum() const;
    
    // Learning rate for competitive learning
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Normalized Oja's rule: Principal component learning
//
// Implements Oja's learning rule for extracting principal components from neural activity.
// This is particularly useful for:
//
// 1. Dimensionality reduction in neural representations
// 2. Feature extraction and invariant pattern recognition
// 3. Competitive learning in sensory processing
// 4. Learning stable neural assemblies
//
// Mathematical formulation (Oja's rule):
//   Δw = η * y * (x - y * w)
// Where:
//   x = pre-synaptic activity
//   y = post-synaptic output (firing rate)
//   w = synaptic weight
//   η = learning rate
//
// This rule maintains weight normalization while extracting principal components,
// making it ideal for unsupervised learning and feature discovery.

class NormalizedOjaRule : public Hebbian {
public:
    NormalizedOjaRule();
    ~NormalizedOjaRule() override;
    
    // Update synaptic weights using Oja's principal component rule
    //
    // Implements weight update that normalizes synaptic strength while preserving
    // the direction of maximum variance in input activity
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply Oja weight change with normalization
    //
    // Ensures that synaptic weights maintain biological constraints while
    // extracting principal components from neural activity
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get the human-readable name of this plasticity rule
    const char* getName() const override;
    
    // Learning rate for principal component extraction
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Enable/disable weight normalization
    void enableNormalization(bool enable);
    bool isNormalizationEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Synaptic tagging and capture (STC) rule
//
// Implements synaptic tagging and capture mechanism for associative memory.
// This rule allows for:
//
// 1. Input-specific synaptic tagging
// 2. Memory consolidation via neuromodulatory capture
// 3. Memory specificity through biochemical tags
// 4. Interaction with reward systems
//
// Biological basis:
// - Models synaptic tagging and capture for memory consolidation
// - Implements biochemical tagging of active synapses
// - Reflects interaction between learning and neuromodulation
//
// Mathematical formulation:
// For each pre-post spike pair:
//   Tag strength: τ(t) = τ(t-1) + λ * δ(t - t_spike)
//   Weight change: Δw = A * τ * reward_signal
// Where:
//   τ = synaptic tag
//   λ = tag increment factor
//   A = learning amplitude
//   δ = Dirac delta (spike event)

class SynapticTaggingCapture : public Hebbian {
public:
    SynapticTaggingCapture();
    ~SynapticTaggingCapture() override;
    
    // Update synaptic weights using tag-and-capture mechanism
    //
    // Implements synaptic tagging that accumulates with activity and is
    // captured by neuromodulatory signals to produce learning
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply tag-and-capture weight change
    //
    // Implements the actual weight modification based on accumulated tags
    // and neuromodulatory capture signals
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get the human-readable name of this plasticity rule
    const char* getName() const override;
    
    // Tagging parameters
    void setTagIncrement(float increment);  // Rate of tag accumulation
    float getTagIncrement() const;
    
    void setTagDecay(float decay);          // Tag decay rate
    float getTagDecay() const;
    
    // Capture parameters
    void setCaptureEfficiency(float efficiency);  // Neuromodulatory capture efficiency
    float getCaptureEfficiency() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm