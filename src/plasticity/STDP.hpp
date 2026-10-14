#pragma once

#include "PlasticityRule.hpp"
#include "RewardModulatedSTDP.hpp"
#include "TripletSTDP.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Real STDP implementation based on spike-timing correlation
// 
// Mathematical formulation:
// For each pre-post spike pair with timing difference Δt = t_post - t_pre:
// 
// If Δt > 0 (pre before post): POTENTIATION
//   Δw = A+ * exp(-Δt / τ+)
//   
// If Δt < 0 (post before pre): DEPRESSION
//   Δw = A- * exp(Δt / τ-)
// 
// Where:
//   A+ = ltpWeight (potentiation amplitude)
//   A- = ltdWeight (depression amplitude)  
//   τ+ = τ- = timeConstant (STDP time window)
// 
// Biological inspiration:
// - Reflects NMDA receptor-mediated calcium signaling
// - Pre-before-post activates NMDA receptors when postsynaptic spikes
// - Post-before-pre causes backpropagating action potentials
// 
// Enhanced implementations:
// - RewardModulatedSTDP: Combines spike-timing dependence with reward prediction error
// - TripletSTDP: Includes three-spike interactions (pre-post-post, post-post-pre, post-pre-pre)
// - Supports event-driven computation with synaptic eligibility traces
// 
// Phase 2 Implementation:
// - Real synaptic dynamics with exponential windows
// - Reward prediction error modulation for reinforcement learning
// - Triplet interaction models for complex spike patterns
// - Integration with dopamine neuromodulation system
// - Performance-optimized for large-scale networks
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
// Combines spike-timing dependence with reward modulation
class RewardModulatedSTDP : public STDP {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    using STDP::update;  // Inherit base class update with different signature
    
    // Override with reward-modulated version
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt,
                 float rewardPredictionError = 0.0f) override;
    
    // Reward modulation function
    void setRewardSensitivity(float beta);  // β parameter
    float getRewardSensitivity() const;
    
    // Enable/disable reward modulation
    void enableRewardModulation(bool enable);
    bool isRewardModulationEnabled() const;
    
    // Get current modulation factor
    float getCurrentModulationFactor() const;
    
    // Set reward prediction error signal
    void setPredictionError(float error) { predictionError = error; }
    float getPredictionError() const { return predictionError; }

private:
    struct Impl;
    Impl* pImpl;
    
    // External reward prediction error signal
    float predictionError;
    bool rewardModulationEnabled;
};

// Spike-timing dependent plasticity with triplet interactions
// Extends standard STDP to include three-spike interactions
class TripletSTDP : public STDP {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    using STDP::update;  // Inherit base class update
    
    // Override with triplet interaction version
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Triplet parameters
    void setTripletTimeConstant(float tau1, float tau2);  // Fast and slow time constants
    void setTripletWeights(float w1, float w2, float w3);   // Triplet weights
    
    float getTripletTimeConstant1() const;
    float getTripletTimeConstant2() const;
    float getTripletWeight1() const;
    float getTripletWeight2() const;
    float getTripletWeight3() const;
    
    // Enable/disable triplet interactions
    void enableTripletInteractions(bool enable);
    bool areTripletInteractionsEnabled() const;
    
    // Get current triplet contribution
    float getTripletContribution() const;

private:
    struct Impl;
    Impl* pImpl;
    bool tripletEnabled;
};

} // namespace nlm

