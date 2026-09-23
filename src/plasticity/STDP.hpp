#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity (STDP)
// Implementation of biological STDP based on spike timing correlations
// Supports both standard STDP and reward-modulated variants
// Inspired by biological synaptic plasticity observed in hippocampal and cortical synapses
class STDP : public PlasticityRule {
public:
    /// @brief Construct STDP with default parameters
    /// Default parameters: LTP weight = 0.01, LTD weight = 0.012, time constant = 20.0ms
    STDP();
    
    /// @brief Destroy STDP and clean up resources
    ~STDP() override;
    
    /// @brief Update synaptic weights based on pre- and post-synaptic spike timing
    /// Implements the classic STDP learning rule: pre-before-post strengthens, 
    /// post-before-pre weakens the synapse
    /// @param synapse The synaptic connection to update
    /// @param preSpikes Vector of pre-synaptic spike timestamps (ms)
    /// @param postSpikes Vector of post-synaptic spike timestamps (ms)
    /// @param dt Simulation timestep in seconds
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /// @brief Apply a direct weight change to the synapse
    /// Used for external weight modifications or global weight updates
    /// @param synapse The synaptic connection to modify
    /// @param delta Weight change value (positive for potentiation, negative for depression)
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /// @brief Get the name of this plasticity rule
    /// @return String identifier "STDP"
    const char* getName() const override;
    
    // STDP parameter management
    /// @brief Set the Long-Term Potentiation (LTP) weight (A+)
    /// LTP strength determines how much synapses are strengthened when pre fires before post
    /// @param weight LTP weight (clamped to [0.0, 1.0])
    void setLTPWeight(float weight);
    
    /// @brief Get the Long-Term Potentiation (LTP) weight
    /// @return LTP weight value
    float getLTPWeight() const;
    
    /// @brief Set the Long-Term Depression (LTD) weight (A-)
    /// LTD strength determines how much synapses are weakened when post fires before pre
    /// @param weight LTD weight (clamped to [0.0, 1.0])
    void setLTDWeight(float weight);
    
    /// @brief Get the Long-Term Depression (LTD) weight
    /// @return LTD weight value
    float getLTDWeight() const;
    
    /// @brief Set the STDP time constant (τ)
    /// Controls the time window over which spike timing affects plasticity
    /// Longer τ means plasticity effects last longer after spike
    /// @param tau Time constant in milliseconds (clamped to [1.0, 100.0])
    void setTimeConstant(float tau);
    
    /// @brief Get the STDP time constant
    /// @return Time constant in milliseconds
    float getTimeConstant() const;
    
    /// @brief Configure STDP parameters from configuration values
    /// Convenience method to set all STDP parameters at once
    /// @param ltpWeight Long-term potentiation weight
    /// @param ltdWeight Long-term depression weight  
    /// @param tau Time constant in milliseconds
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    /// @brief Forward declaration of implementation struct (Pimpl pattern)
    struct Impl;
    
    /// @brief Pointer to implementation (encapsulates private data and configuration)
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Extends STDP with reward prediction error modulation
// Implements dopamine-dependent plasticity as observed in biological systems
// Plasticity strength is scaled by reward prediction error signals
class RewardModulatedSTDP : public PlasticityRule {
public:
    /// @brief Default constructor (no implementation yet - placeholder for Phase 2)
    RewardModulatedSTDP();
    
    /// @brief Default destructor
    ~RewardModulatedSTDP() override;
    
    /// @brief Update synaptic weights with reward modulation
    /// Weight change is scaled by reward prediction error signal
    /// @param synapse The synaptic connection to update
    /// @param preSpikes Vector of pre-synaptic spike timestamps
    /// @param postSpikes Vector of post-synaptic spike timestamps
    /// @param dt Simulation timestep in seconds
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /// @brief Apply weight change with reward modulation
    /// @param synapse The synaptic connection to modify
    /// @param delta Weight change value
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /// @brief Get the name of this plasticity rule
    /// @return String identifier "R-STDP"
    const char* getName() const override;
};

// Spike-timing dependent plasticity with triplet interactions
// Advanced STDP implementation that considers sequences of spikes
// Includes contributions from spike triplets (pre-pre-post, pre-post-post)
// Provides more biologically realistic plasticity dynamics
// PLACEHOLDER - Phase 2 will implement complete triplet STDP
class TripletSTDP : public PlasticityRule {
public:
    /// @brief Default constructor (placeholder for Phase 2)
    TripletSTDP();
    
    /// @brief Default destructor
    ~TripletSTDP() override;
    
    /// @brief Update synaptic weights using triplet STDP
    /// Considers sequences of pre- and post-synaptic spikes
    /// @param synapse The synaptic connection to update
    /// @param preSpikes Vector of pre-synaptic spike timestamps
    /// @param postSpikes Vector of post-synaptic spike timestamps
    /// @param dt Simulation timestep in seconds
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /// @brief Apply weight change using triplet STDP
    /// @param synapse The synaptic connection to modify
    /// @param delta Weight change value
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /// @brief Get the name of this plasticity rule
    /// @return String identifier "TripletSTDP"
    const char* getName() const override;
};

} // namespace nlm
