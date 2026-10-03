#pragma once

#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @class STDP
 * @brief Spike-Timing-Dependent Plasticity (STDP) implementation
 * 
 * STDP is a fundamental learning mechanism in biological neural networks
 * where synaptic strength changes depend on the precise timing of
 * pre- and post-synaptic spikes. This implementation provides a complete
 * biologically realistic model of STDP with configurable parameters.
 * 
 * The mathematical formulation follows the classic pair-based STDP rule:
 * - If post fires after pre (Δt > 0): potentiation Δw = A⁺·exp(-Δt/τ⁺)
 * - If pre fires after post (Δt < 0): depression Δw = -A⁻·exp(Δt/τ⁻)
 * 
 * Biological inspiration:
 * - Reflects NMDA receptor-mediated calcium signaling
 * - Implements spike-timing dependent plasticity at glutamatergic synapses
 * - Captures learning in hippocampal and cortical connections
 * 
 * References:
 * - Bi and Poo (1998), "Synaptic modification by coactivation of afferent
 *   and postsynaptic sites"
 * - Song et al. (2000), "Competitive learning in recurrent neural networks
 *   via spike-timing dependent plasticity"
 * - Dan & Zukin (2004), "Synaptic mechanisms of memory"
 * 
 * Key features:
 * - Configurable LTP/LTD weights and time constants
 * - Eligibility trace support for reward-modulated variants
 * - Synaptic weight bounds and clamping
 * - Efficient spike pair computation
 * 
 * Performance characteristics:
 * - O(N×M) for spike pair computation (N pre, M post spikes)
 * - Thread-safe parameter updates
 * - Minimal memory footprint per synapse
 */
class STDP : public PlasticityRule {
public:
    /**
     * @brief Construct a new STDP plasticity rule with default parameters
     * 
     * Default parameters:
     * - LTP weight (A⁺): 0.01
     * - LTD weight (A⁻): 0.012
     * - Time constant (τ): 20.0 ms
     * - Weight range: [-1.0, 1.0]
     */
    STDP();
    
    /**
     * @brief Destroy the STDP plasticity rule
     * 
     * Clean up pImpl implementation and release allocated memory
     */
    ~STDP() override;
    
    /**
     * @brief Update synaptic weights based on pre- and post-synaptic spike timing
     * 
     * Computes spike-timing dependent plasticity using all combinations
     * of pre- and post-synaptic spike times within the observation window.
     * 
     * @param synapse Pointer to the synapse being modified
     * @param preSpikes Vector of pre-synaptic spike timestamps (sorted)
     * @param postSpikes Vector of post-synaptic spike timestamps (sorted)
     * @param dt Simulation timestep (not directly used in STDP computation)
     * 
     * @note Time complexity: O(N×M) where N = preSpikes.size(), M = postSpikes.size()
     * @note Assumes spike times are in milliseconds
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply a direct weight change to the synapse
     * 
     * This method is used for direct weight modifications, such as those
     * resulting from reward-modulated STDP or other plasticity rules.
     * 
     * @param synapse Pointer to the synapse being modified
     * @param delta Weight change to apply
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the human-readable name of this plasticity rule
     * 
     * @return const char* Name of the plasticity rule ("STDP")
     */
    const char* getName() const override;
    
    /**
     * @brief Set the long-term potentiation (LTP) weight parameter
     * 
     * The LTP weight (A⁺) determines the magnitude of synaptic strengthening
     * when post-synaptic spikes follow pre-synaptic spikes.
     * 
     * @param weight LTP weight (clamped to [0.0, 1.0])
     */
    void setLTPWeight(float weight);
    
    /**
     * @brief Get the current long-term potentiation weight
     * 
     * @return float Current LTP weight
     */
    float getLTPWeight() const;
    
    /**
     * @brief Set the long-term depression (LTD) weight parameter
     * 
     * The LTD weight (A⁻) determines the magnitude of synaptic weakening
     * when post-synaptic spikes precede pre-synaptic spikes.
     * 
     * @param weight LTD weight (clamped to [0.0, 1.0])
     */
    void setLTDWeight(float weight);
    
    /**
     * @brief Get the current long-term depression weight
     * 
     * @return float Current LTD weight
     */
    float getLTDWeight() const;
    
    /**
     * @brief Set the STDP time constant (tau)
     * 
     * The time constant controls the decay of the STDP window.
     * Longer time constants allow for more distant spike timing to
     * have an effect on synaptic strength.
     * 
     * @param tau Time constant in milliseconds (clamped to [1.0, 100.0])
     */
    void setTimeConstant(float tau);
    
    /**
     * @brief Get the current STDP time constant
     * 
     * @return float Current time constant in milliseconds
     */
    float getTimeConstant() const;
    
    /**
     * @brief Configure STDP parameters from configuration values
     * 
     * Convenience method to set all three main STDP parameters at once.
     * 
     * @param ltpWeight Long-term potentiation weight
     * @param ltdWeight Long-term depression weight  
     * @param tau Time constant
     */
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class RewardModulatedSTDP
 * @brief Reward-modulated STDP (R-STDP) combining spike timing with reward signals
 * 
 * R-STDP extends standard STDP by incorporating reward prediction errors
 * through eligibility traces. This implements a biologically plausible
 * mechanism for reinforcement learning where synaptic changes are gated
 * by neuromodulatory signals like dopamine.
 * 
 * Mathematical formulation:
 *   dE/dt = -E/τ + Σ STDP_pre→post(t)
 *   Δw = η·α·E·R
 * 
 * Where E is the eligibility trace, R is the reward prediction error,
 * and α is the reward modulation strength.
 * 
 * Biological inspiration:
 * - Dopamine signaling in the midbrain (VTA/SNc)
 * - Credit assignment through eligibility traces
 * - Temporal difference learning in reinforcement learning
 * 
 * References:
 * - Dayan & Abbott (2001), "Theoretical Neuroscience"
 * - Schultz et al. (1997), "Neuronal coding of prediction errors"
 * - Montague et al. (1996), "Biology of the prediction error"
 * 
 * Key features:
 * - Eligibility trace for credit assignment
 * - Configurable reward modulation strength
 * - Integration with neuromodulatory systems
 * - Support for temporal difference learning
 */
class RewardModulatedSTDP : public PlasticityRule {
public:
    /**
     * @brief Construct a new RewardModulatedSTDP plasticity rule
     * 
     * Default parameters:
     * - Learning rate: 0.01
     * - Reward factor: 1.0
     * - Trace decay: 100.0 ms
     */
    RewardModulatedSTDP();
    
    /**
     * @brief Destroy the RewardModulatedSTDP plasticity rule
     */
    ~RewardModulatedSTDP() override;
    
    /**
     * @brief Update synaptic weights with reward modulation
     * 
     * Combines spike-timing dependent plasticity with reward signals
     * using eligibility traces to assign credit to specific spike patterns.
     * 
     * @param synapse Pointer to the synapse being modified
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Simulation timestep
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply a reward-modulated weight change
     * 
     * @param synapse Pointer to the synapse being modified
     * @param delta Reward-modulated weight change
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the human-readable name of this plasticity rule
     * 
     * @return const char* Name of the plasticity rule ("R-STDP")
     */
    const char* getName() const override;
    
    /**
     * @brief Set the overall learning rate
     * 
     * @param rate Learning rate (clamped to [0.0, 1.0])
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get the current learning rate
     * 
     * @return float Current learning rate
     */
    float getLearningRate() const;
    
    /**
     * @brief Set the reward modulation factor
     * 
     * Controls the strength of reward modulation on synaptic changes.
     * 
     * @param factor Reward factor (clamped to [0.0, 5.0])
     */
    void setRewardFactor(float factor);
    
    /**
     * @brief Get the current reward factor
     * 
     * @return float Current reward factor
     */
    float getRewardFactor() const;
    
    /**
     * @brief Set the eligibility trace decay time constant
     * 
     * Controls how quickly eligibility traces decay without reward signals.
     * 
     * @param decay Trace decay time constant in ms (clamped to [10.0, 1000.0])
     */
    void setTraceDecay(float decay);
    
    /**
     * @brief Get the current trace decay time constant
     * 
     * @return float Current trace decay time constant
     */
    float getTraceDecay() const;
    
    /**
     * @brief Configure R-STDP parameters from configuration values
     * 
     * @param learningRate Overall learning rate
     * @param rewardFactor Reward modulation strength
     * @param traceDecay Eligibility trace decay time constant
     */
    void configure(float learningRate, float rewardFactor, float traceDecay);
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class TripletSTDP
 * @brief Spike-timing dependent plasticity with triplet spike interactions
 * 
 * Triplet STDP extends standard STDP by incorporating second-order spike
 * interactions that capture more complex temporal dynamics. This implementation
 * includes effects from three-factor spike interactions that are important
 * for understanding synaptic plasticity in vivo.
 * 
 * Mathematical formulation:
 * - First-order: Standard STDP rule
 * - Second-order: Triple spike interactions
 * - NMDA saturation: Non-linear modulation
 * 
 * Biological inspiration:
 * - NMDA receptor saturation effects
 * - Calcium dynamics with delayed components
 * - In vivo spike train statistics
 * - Synaptic plasticity in awake behaving animals
 * 
 * References:
 * - Debanne et al. (2006), "How spike timing affects synaptic plasticity"
 * - Geisler et al. (2005), "Synaptic plasticity in the prefrontal cortex"
 * - Peyrache et al. (2009), "Replay of nested fast oscillations in the
 *   hippocampus and memory consolidation"
 * 
 * Key features:
 * - Triple spike interaction rules
 * - NMDA receptor saturation modeling
 * - Spike history tracking
 * - Non-linear weight dependence
 */
class TripletSTDP : public PlasticityRule {
public:
    /**
     * @brief Construct a new TripletSTDP plasticity rule
     * 
     * Default parameters:
     * - LTP weight: 0.01
     * - LTD weight: 0.012
     * - Time constant: 20.0 ms
     * - Second-order potentiation (A2): 0.005
     * - Second-order depression (B2): 0.006
     */
    TripletSTDP();
    
    /**
     * @brief Destroy the TripletSTDP plasticity rule
     */
    ~TripletSTDP() override;
    
    /**
     * @brief Update synaptic weights with triplet interactions
     * 
     * Computes spike-timing dependent plasticity including both first-order
     * (single spike pair) and second-order (triple spike interaction) effects.
     * 
     * @param synapse Pointer to the synapse being modified
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Simulation timestep
     */
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    /**
     * @brief Apply a triplet STDP weight change
     * 
     * @param synapse Pointer to the synapse being modified
     * @param delta Triplet STDP weight change
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the human-readable name of this plasticity rule
     * 
     * @return const char* Name of the plasticity rule ("TripletSTDP")
     */
    const char* getName() const override;
    
    /**
     * @brief Set the long-term potentiation (LTP) weight
     * 
     * @param weight LTP weight (clamped to [0.0, 1.0])
     */
    void setLTPWeight(float weight);
    
    /**
     * @brief Get the current LTP weight
     * 
     * @return float Current LTP weight
     */
    float getLTPWeight() const;
    
    /**
     * @brief Set the long-term depression (LTD) weight
     * 
     * @param weight LTD weight (clamped to [0.0, 1.0])
     */
    void setLTDWeight(float weight);
    
    /**
     * @brief Get the current LTD weight
     * 
     * @return float Current LTD weight
     */
    float getLTDWeight() const;
    
    /**
     * @brief Set the time constant (tau)
     * 
     * @param tau Time constant in ms (clamped to [5.0, 100.0])
     */
    void setTimeConstant(float tau);
    
    /**
     * @brief Get the current time constant
     * 
     * @return float Current time constant in ms
     */
    float getTimeConstant() const;
    
    /**
     * @brief Set the second-order potentiation weight (A2)
     * 
     * Controls the strength of triple pre-synaptic spike potentiation.
     * 
     * @param weight A2 weight (clamped to [0.0, 0.1])
     */
    void setA2Weight(float weight);
    
    /**
     * @brief Get the current second-order potentiation weight
     * 
     * @return float Current A2 weight
     */
    float getA2Weight() const;
    
    /**
     * @brief Set the second-order depression weight (B2)
     * 
     * Controls the strength of triple post-synaptic spike depression.
     * 
     * @param weight B2 weight (clamped to [0.0, 0.1])
     */
    void setB2Weight(float weight);
    
    /**
     * @brief Get the current second-order depression weight
     * 
     * @return float Current B2 weight
     */
    float getB2Weight() const;
    
    /**
     * @brief Set the NMDA receptor threshold voltage
     * 
     * @param theta NMDA threshold in mV (clamped to [-70.0, 50.0])
     */
    void setNMDATheta(float theta);
    
    /**
     * @brief Get the current NMDA receptor threshold
     * 
     * @return float Current NMDA threshold in mV
     */
    float getNMDATheta() const;
    
    /**
     * @brief Configure TripletSTDP parameters from configuration values
     * 
     * @param ltpWeight Long-term potentiation weight
     * @param ltdWeight Long-term depression weight
     * @param tau Time constant
     * @param a2Weight Second-order potentiation weight
     * @param b2Weight Second-order depression weight
     */
    void configure(float ltpWeight, float ltdWeight, float tau, float a2Weight, float b2Weight);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
