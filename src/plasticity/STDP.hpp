#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity (STDP) implementation
// 
// STDP is a fundamental learning mechanism in neuroscience where the timing
// of pre- and post-synaptic spikes determines synaptic modification. If a
// pre-synaptic spike occurs just before a post-synaptic spike, the synapse
// is strengthened (Long-Term Potentiation, LTP). If the post-synaptic spike
// occurs just before the pre-synaptic spike, the synapse is weakened
// (Long-Term Depression, LTD). This precise timing-based learning is crucial
// for temporal coding, pattern separation, and memory formation in the brain.
// 
// Mathematical formulation:
// For Δt = t_post - t_pre > 0 (pre before post): LTP
//   Δw = A+ * exp(-Δt / τ+)
// For Δt = t_post - t_pre < 0 (post before pre): LTD  
//   Δw = -A- * exp(Δt / τ-)
// 
// Biological basis: STDP reflects NMDA receptor-dependent calcium signaling
// in dendrites. Pre-before-post activation opens NMDA receptors, leading to
// calcium influx and LTP. Post-before-pre activates back-propagating action
// potentials that interact with weaker NMDA currents, causing LTD.
// 
// This implementation provides a realistic STDP model suitable for simulating
// learning in spiking neural networks, with applications in memory, attention,
// and temporal processing.

class STDP : public PlasticityRule {
public:
    /// \brief Constructor - initializes with biological parameters from literature
    /// \note Default parameters based on experimental data from hippocampal
    ///       slice recordings (e.g., Bi and Poo, 1998; Froemke et al., 2003)
    STDP();
    
    /// \brief Virtual destructor for proper cleanup
    ~STDP() override;
    
    /// \brief Update synaptic weights using spike-timing-dependent plasticity
    /// \param synapse The synapse to modify
    /// \param preSpikes Vector of pre-synaptic spike timestamps
    /// \param postSpikes Vector of post-synaptic spike timestamps
    /// \param dt Current simulation timestep
    /// \note Computes pairwise spike interactions within biologically plausible
    ///       time windows (typically 20-100ms), implementing the classic
    ///       STDP rule with exponential decay functions
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /// \brief Apply direct weight change to synapse with bounds checking
    /// \param synapse The synapse to modify
    /// \param delta Weight change to apply
    /// \note Ensures synaptic weights remain within biologically plausible
    ///       limits (-1.0 to 1.0), preventing runaway excitation/inhibition
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /// \brief Get rule name
    /// \return Constant string "STDP"
    const char* getName() const override;
    
    /// \brief Configure STDP learning parameters
    /// \param ltpWeight LTP amplitude A+ (typical: 0.01-0.05)
    /// \param ltdWeight LTD amplitude A- (typically slightly larger than A+)
    /// \param tau STDP time constant τ (typically 20-100ms)
    /// \note Parameters should be optimized based on target network behavior
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    /// \brief Set long-term potentiation weight with validation
    /// \param weight New LTP weight (clamped to [0.0, 1.0])
    void setLTPWeight(float weight);
    
    /// \brief Get current LTP weight
    /// \return Current LTP weight
    float getLTPWeight() const;
    
    /// \brief Set long-term depression weight with validation
    /// \param weight New LTD weight (clamped to [0.0, 1.0])
    void setLTDWeight(float weight);
    
    /// \brief Get current LTD weight
    /// \return Current LTD weight
    float getLTDWeight() const;
    
    /// \brief Set STDP time constant with validation
    /// \param tau New time constant in milliseconds (clamped to [1.0, 100.0])
    void setTimeConstant(float tau);
    
    /// \brief Get current STDP time constant
    /// \return Current time constant in milliseconds
    float getTimeConstant() const;
    
    /// \brief Set minimum synaptic weight bound
    /// \param min Minimum weight (typically -1.0)
    void setMinWeight(float min);
    
    /// \brief Get minimum synaptic weight bound
    /// \return Current minimum weight
    float getMinWeight() const;
    
    /// \brief Set maximum synaptic weight bound
    /// \param max Maximum weight (typically 1.0)
    void setMaxWeight(float max);
    
    /// \brief Get maximum synaptic weight bound
    /// \return Current maximum weight
    float getMaxWeight() const;

private:
    /// \brief Internal implementation details for efficient parameter storage
    struct Impl;
    
    /// \brief Pointer to private implementation
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
