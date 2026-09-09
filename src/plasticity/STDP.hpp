#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Forward declarations for helper functions
class TripletSTDP;

/**
 * @class STDP
 * @brief Spike-Timing-Dependent Plasticity implementation
 * 
 * Implements classic STDP based on the timing difference between pre- and
 * post-synaptic spikes. Uses exponential learning windows with separate
 * potentiation and depression amplitudes.
 * 
 * The mathematical formulation is:
 * - Δw = A+ × exp(-Δt/τ+) for Δt > 0 (pre before post, potentiation)
 * - Δw = A- × exp(Δt/τ-) for Δt < 0 (post before pre, depression)
 * 
 * Where Δt = t_post - t_pre. This implements the "fire together, wire together"
 * principle with timing specificity, reflecting NMDA receptor dynamics and
 * backpropagating action potentials.
 */

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
    
    /**
     * @brief Set long-term potentiation weight (A+)
     * 
     * Controls the maximum weight increase for pre-before-post spike pairs.
     * Typical values: 0.001-0.01
     * 
     * @param weight LTP weight parameter
     */
    void setLTPWeight(float weight);    
    
    /**
     * @brief Get long-term potentiation weight (A+)
     * 
     * @return Current LTP weight value
     */
    float getLTPWeight() const;
    
    /**
     * @brief Set long-term depression weight (A-)
     * 
     * Controls the maximum weight decrease for post-before-pre spike pairs.
     * Typical values: 0.001-0.015 (slight asymmetry for stability)
     * 
     * @param weight LTD weight parameter
     */
    void setLTDWeight(float weight);    
    
    /**
     * @brief Get long-term depression weight (A-)
     * 
     * @return Current LTD weight value
     */
    float getLTDWeight() const;
    
    /**
     * @brief Set STDP time constant (τ)
     * 
     * Controls the width of the learning window. Longer τ means more
     * spikes contribute to learning, at the cost of temporal precision.
     * Typical values: 10-50 ms
     * 
     * @param tau Time constant in milliseconds
     */
    void setTimeConstant(float tau);    
    
    /**
     * @brief Get STDP time constant (τ)
     * 
     * @return Current time constant value
     */
    float getTimeConstant() const;
    
    /**
     * @brief Configure STDP parameters from configuration values
     * 
     * Convenience method to set all parameters at once. Useful for loading
     * from configuration files or applying standard STDP settings.
     * 
     * @param ltpWeight Long-term potentiation weight
     * @param ltdWeight Long-term depression weight
     * @param tau STDP time constant
     */
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class RewardModulatedSTDP
 * @brief STDP with reward-modulated learning via eligibility traces
 * 
 * Combines spike-timing dependent plasticity with reward signals using
 * an eligibility trace mechanism. Synaptic changes only occur when both
 * a reward is received AND there was appropriate spike timing, implementing
 * the dopamine prediction error signal in reinforcement learning.
 * 
 * This rule addresses the credit assignment problem in temporally extended
 * tasks by maintaining a trace of recent spike timing relationships that
 * can be updated when reward is received.
 */

class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    /**
     * @brief Set reward modulation weight
     * 
     * Controls the strength of the reward signal's effect on synaptic plasticity.
     * Higher values make learning more sensitive to reward.
     * 
     * @param weight Reward modulation weight (0.0-1.0)
     */
    void setRewardWeight(float weight);
    
    /**
     * @brief Get reward modulation weight
     * 
     * @return Current reward weight
     */
    float getRewardWeight() const;
    
    /**
     * @brief Set eligibility trace decay rate
     * 
     * Controls how quickly the eligibility trace decays in the absence of
     * reward. Faster decay means more temporally precise reward signaling.
     * 
     * @param decayRate Decay rate per millisecond (0.001-0.1)
     */
    void setTraceDecay(float decayRate);
    
    /**
     * @brief Get eligibility trace decay rate
     * 
     * @return Current decay rate
     */
    float getTraceDecay() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class TripletSTDP
 * @brief Enhanced STDP with triplet interactions
 * 
 * Extends standard STDP to include triplet interactions (presynaptic,
 * postsynaptic, and single-spike terms) to better capture the dynamics
 * of real neural circuits. This formulation can reproduce complex
 * calcium-dependent plasticity rules observed in biological systems.
 * 
 * Triplet STDP captures:
 * - Presynaptic spike + postsynaptic spike within window
 * - Postsynaptic spike + presynaptic spike within window  
 * - Single spike interactions with autapical and afterhyperpolarization terms
 */

class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    /**
     * @brief Set autapical potentiation weight
     * 
     * Controls the strength of pre-before-post triplet interactions.
     * 
     * @param weight Autapical weight (0.0-0.1)
     */
    void setAPWeight(float weight);      
    
    /**
     * @brief Get autapical potentiation weight
     * 
     * @return Current AP weight
     */
    float getAPWeight() const;
    
    /**
     * @brief Set postsynaptic potentiation weight
     * 
     * Controls the strength of post-before-pre triplet interactions.
     * 
     * @param weight PSP weight (0.0-0.1)
     */
    void setPSPWeight(float weight);     
    
    /**
     * @brief Get postsynaptic potentiation weight
     * 
     * @return Current PSP weight
     */
    float getPSPWeight() const;
    
    /**
     * @brief Set afterhyperpolarization depression weight
     * 
     * Controls the strength of depression in triplet interactions.
     * 
     * @param weight AHP weight (0.0-0.1)
     */
    void setAHPWeight(float weight);     
    
    /**
     * @brief Get afterhyperpolarization depression weight
     * 
     * @return Current AHP weight
     */
    float getAHPWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Helper functions for triplet STDP computation
    float computePairwiseSTDP(const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             float apWeight, float ahpWeight, float tau);
    
    float computeAutapicalPotentiation(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       float weight, float tau);
    
    float computePostsynapticPotentiation(const std::vector<Timestamp>& postSpikes,
                                          const std::vector<Timestamp>& preSpikes,
                                          float weight, float tau);
    
    float computeAfterhyperpolarizationDepression(const std::vector<Timestamp>& preSpikes,
                                                  const std::vector<Timestamp>& postSpikes,
                                                  float weight, float tau);
};

} // namespace nlm
