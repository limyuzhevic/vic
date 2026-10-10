#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

namespace nlm {

// STDP (Spike-Timing-Dependent Plasticity)
// Core synaptic learning mechanism based on relative spike timing
//
// STDP Architecture:
// =================
// STDP implements one of the most well-documented forms of synaptic plasticity
// in biological neural networks. It enables neurons to learn temporal relationships
// between their firing patterns, forming the basis for sequence learning and
// temporal prediction.
//
// Biological Foundations:
// ---------------------
// - **NMDA Receptor Activation**: Pre-before-post spikes activate NMDA receptors
// - **Calcium Signaling**: Different spike timing produces different calcium dynamics
// - **Signaling Cascades**: Calcium triggers different kinases depending on timing
// - **Synaptic Tagging**: Eligibility traces mark synapses for potential modification
// - **Reward Modulation**: Neuromodulators gate STDP learning (verified in mammals)
//
// STDP Learning Rules:
// -------------------
// STDP is based on the timing difference between pre-synaptic (pre) and
// post-synaptic (post) spikes:
//
// 1. **Long-Term Potentiation (LTP)**: Pre before post
//    - Δw = A⁺ * exp(-Δt/τ⁺)  where Δt = t_post - t_pre > 0
//    - Strengthens synapses when pre fires before post
//    - Represents NMDA receptor activation
//
// 2. **Long-Term Depression (LTD)**: Post before pre
//    - Δw = -A⁻ * exp(Δt/τ⁻)  where Δt = t_post - t_pre < 0
//    - Weakens synapses when post fires before pre
//    - Represents back-propagating action potentials
//
// Key Parameters:
// --------------
// - **A⁺ (LTP weight)**: Maximum potentiation (typically 0.01-0.1)
// - **A⁻ (LTD weight)**: Maximum depression (typically 0.01-0.012)
// - **τ⁺ (LTP time constant)**: ~20ms (typically 15-30ms)
// - **τ⁻ (LTD time constant)**: ~20ms (typically 15-30ms)
//
// STDP Mathematical Formulation:
// -----------------------------
// For each spike pair (pre at t_pre, post at t_post):
//
// If t_post > t_pre (pre before post):
//   Δw = A⁺ * exp(-(t_post - t_pre)/τ⁺)
//   Result: Potentiation (weight increase)
//
// If t_post < t_pre (post before pre):
//   Δw = -A⁻ * exp((t_post - t_pre)/τ⁻)
//   Result: Depression (weight decrease)
//
// If t_post = t_pre (simultaneous):
//   Δw = 0 (no change)
//
// STDP in Network Learning:
// -------------------------
// STDP enables several important network behaviors:
//
// 1. **Temporal Sequence Learning**: Neurons learn which neurons tend to
//    follow others in time, forming sequence representations
//
// 2. **Prediction Learning**: STDP helps predict when post-synaptic neurons
//    will fire based on pre-synaptic activity
//
// 3. **Causal Inference**: STDP is sensitive to causal order, helping the
//    network distinguish cause from correlation
//
// 4. **Sparse Learning**: Only synapses with correct timing patterns are modified
//
// 5. **Replay Learning**: STDP can learn from experience replay during sleep
//
// STDP Variants:
// -------------
// Standard STDP implementations include:
//
// 1. **Basic STDP**: Simple pairwise spike timing rule
// 2. **Reward-Modulated STDP (R-STDP)**: Gated by reward prediction errors
// 3. **Triplet STDP**: Incorporates spike triplets for more complex timing
// 4. **Additive STDP**: Weight changes are additive (not subtractive)
// 5. **Symmetric STDP**: Same time constants for LTP and LTD
//
// NLM STDP Implementation:
// ------------------------
// The NLM STDP implementation includes:
//
// - **Eligibility Traces**: Track recent activity for delayed learning
// - **Multi-Step Processing**: Handle multiple spikes in time windows
// - **Vectorized Updates**: Efficient batch processing
// - **Reward Integration**: Optional reward-modulated learning
// - **Hardware Optimization**: SIMD vectorization for performance
//
// Learning Scenarios:
// ------------------
// STDP naturally supports several learning scenarios:
//
// 1. **Sensory-Learning**: Sensory neurons learn to predict more abstract neurons
// 2. **Motor-Outcome**: Motor neurons learn consequences of actions
// 3. **Temporal-Processing**: Sequential neurons learn temporal dependencies
// 4. **Predictive-Coding**: Higher areas predict lower area activity
//
// STDP Limitations and Extensions:
// ------------------------------
// 1. **Biological Accuracy**: Real STDP is more complex (different receptors)
// 2. **Multi-Modal**: STDP interacts with other plasticity rules
// 3. **Neuromodulation**: Reward signals modulate STDP effectiveness
// 4. **Structural Plasticity**: STDP can lead to synapse formation/removal
// 5. **Homeostatic Plasticity**: STDP balanced by global weight regulation
//
// Computational Considerations:
// ----------------------------
// Implementing STDP efficiently requires:
//
// 1. **Efficient Storage**: Spike history must be accessible quickly
// 2. **Memory Management**: Handle large numbers of spike events
// 3. **Parallel Processing**: Independent synapses can be updated concurrently
// 4. **Numerical Stability**: Prevent runaway weight changes
// 5. **Temporal Precision**: Accurate spike timing is crucial
//
// Configuration Parameters:
// -------------------------
// - **LTPWeight**: Maximum potentiation per spike pair
// - **LTDWeight**: Maximum depression per spike pair
// - **TimeConstant**: STDP time window (ms)
// - **MaxWeight**: Upper weight bound
// - **MinWeight**: Lower weight bound
// - **LearningRate**: Global scaling factor
// - **TraceDecay**: Eligibility trace decay rate
//
// Integration with NLM Systems:
// -----------------------------
// STDP works with other NLM components:
//
// - **Neuromodulation**: Dopamine signals gate STDP learning
// - **WorkingMemory**: STDP strengthens representations in working memory
// - **PredictionSystem**: STDP learns temporal predictions
// - **PlasticityRules**: STDP can be combined with Hebbian learning
// - **Development**: STDP drives developmental wiring changes
//
// Future Directions:
// -----------------
// Current STDP implementation can be extended with:
//
// 1. **Cellular Details**: NMDA/AMPA receptor dynamics
// 2. **Calcium Modeling**: Multiple calcium compartments
// 3. **Gene Regulation**: Activity-dependent gene expression
// 4. **Structural Changes**: Synapse formation via STDP
// 5. **Network Effects**: Population-level STDP dynamics
//
// The STDP implementation in NLM provides a biologically-inspired,
// computationally efficient foundation for temporal learning in neural systems.

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// PLACEHOLDER - Phase 2
class RewardModulatedSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "R-STDP"; }
};

// Spike-timing dependent plasticity with triplet interactions
// PLACEHOLDER - Phase 2
class TripletSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm
