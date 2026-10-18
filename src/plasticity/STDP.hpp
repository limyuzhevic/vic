#pragma once

#include "PlasticityRule.hpp"
#include <stdexcept>
#include <cmath>

namespace nlm {

// Spike-Timing-Dependent Plasticity (STDP)
// 
// Real biological STDP implementation based on experimental evidence
// from studies on synaptic learning in the brain (mostly from hippocampus and cortex)
// 
// Biological Foundation:
// - STDP was first described by Artola & Brugge (1991) for visual cortex
// - Later confirmed by Song, Miller & Abbott (2000) for neocortical synapses
// - Reflects NMDA receptor-dependent calcium signaling
// - Mediates synaptic competition and synaptic scaling
// 
// Mathematical Model (implementation in STDP.cpp):
// For each pre-post spike pair with timing difference Δt = t_post - t_pre:
// 
// If Δt > 0 (pre before post): POTENTIATION (LTP)
//   Δw = A+ * exp(-Δt / τ+)
// 
// If Δt < 0 (post before pre): DEPRESSION (LTD)  
//   Δw = A- * exp(Δt / τ-)
// 
// Where:
//   A+ = ltpWeight (potentiation amplitude, typically 0.01-0.1)
//   A- = ltdWeight (depression amplitude, typically 0.012-0.015)
//   τ+ = τ- = timeConstant (STDP time window, typically 20-50ms)
// 
// Key Biological Properties:
// 1. Asymmetric timing: Only pre-before-post potentiates
// 2. Exponential decay: Effect decays with increasing time difference
// 3. Direction-specific: Depression requires post-before-pre
// 4. Magnesium blockade: NMDA receptor Mg2+ block dependency
// 5. Learning rate modulation by neuromodulators (dopamine)

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Disable copying, enable moving
    STDP(const STDP&) = delete;
    STDP& operator=(const STDP&) = delete;
    STDP(STDP&&) noexcept;
    STDP& operator=(STDP&&) noexcept;
    
    // Update synaptic weights based on spike timing
    // This is the core learning mechanism of STDP
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply a specific weight change to a synapse
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get the name of this plasticity rule
    const char* getName() const override;
    
    // STDP parameters (biologically plausible ranges)
    void setLTPWeight(float weight);    // Long-term potentiation weight (A+)
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight (A-)
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (τ+, τ- in ms)
    float getTimeConstant() const;
    
    // Update all parameters from configuration
    // Provides a convenient way to configure from config files
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Validation and utility methods
    bool isValidConfiguration() const;
    std::string getConfigurationSummary() const;
    
    // Error handling
    static bool validateSTDPParameters(float ltpWeight, float ltdWeight, float tau);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Reward-modulated STDP (R-STDP)
// Extends standard STDP with reward prediction error signals
// Implemented based on reinforcement learning principles in biological systems
// 
// Biological Basis:
// - Dopamine modulates STDP in ventral tegmental area and striatum
// - Reward prediction error (RPE) gates synaptic learning
// - Combines temporal and motivational learning signals
// - Critical for credit assignment in reinforcement learning
// 
// Mathematical Model:
// Δw = (A+ * exp(-Δt/τ+) - A- * exp(Δt/τ-)) * (1 + RPE * novelty_factor)
// 
// Where:
//   RPE = reward prediction error (0 = no change, +1 = potentiation, -1 = depression)
//   novelty_factor = scaling factor based on novelty detection
// 
// Applications:
// - Dopamine-driven learning in corticostriatal pathways
// - Reward-based synaptic modification
// - Action-value learning

class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    const char* getName() const override { return "R-STDP"; }
    
    // Set reward prediction error scaling
    void setRewardPredictionError(float error);
    float getRewardPredictionError() const;
    
    // Set novelty scaling factor
    void setNoveltyFactor(float factor);
    float getNoveltyFactor() const;
    
    // Enable/disable reward modulation
    void setEnabled(bool enabled);
    bool isEnabled() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Extended STDP model that accounts for spike triplets
// Based on competition models and experimental data from cortical slices
// 
// Biological Motivation:
// - Single-pair STDP insufficient to explain some experimental results
// - Triplet models account for higher-order spike interactions
// - Reflects complex calcium dynamics and synaptic state
// - More accurate prediction of experimental data
// 
// Mathematical Model (Gerstner et al., 2002):
// Combines pre-post pair interactions with triplet contribution
// from sequences: +++ → +++, + → +++, ++ → +++, + → +++, etc.
// 
// Applications:
// - More accurate network models of cortical plasticity
// - Better explanation of learning in structured spike trains
// - Enhanced predictive power for experimental data

class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    const char* getName() const override { return "TripletSTDP"; }
    
    // Set triplet interaction weights
    void setTripletWeights(float w_plus, float w_minus);
    float getTripletWeightPlus() const;
    float getTripletWeightMinus() const;
    
    // Set triplet time constants
    void setTripletTimeConstants(float tau1, float tau2);
    float getTripletTimeConstant1() const;
    float getTripletTimeConstant2() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
