#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @class Synapse
 * @brief Neural connection implementing synaptic transmission and short-term plasticity
 * 
 * This class represents a synaptic connection between neurons with:
 * - Delay-based spike transmission (event-driven)
 * - Short-term plasticity using Tsodyks-Markram model
 * - Weight modulation and synaptic efficacy
 * - Eligibility traces for reward-modulated learning
 * - Spike history tracking for STDP
 * - Biological type classification (excitatory, inhibitory, modulatory)
 * 
 * @details The synapse model combines multiple temporal scales of synaptic
 * modulation, from millisecond-scale transmission to minute-scale eligibility
 * traces. The Tsodyks-Markram model provides a biologically realistic description
 * of synaptic dynamics including depression and facilitation.
 * 
 * **Tsodyks-Markram STP Model**:
 * ```
 * dx/dt = (1 - x)/τ_x - u * x * R(t)
 * du/dt = (U_max - u)/τ_u + U_max * x * f(t)
 * dy/dt = (y_0 - y)/τ_y + u * x * R(t)
 * ```
 * where:
 * - x: fraction of resources available for transmission
 * - u: utilization parameter (fraction of resources used per spike)
 * - y: recovery variable for depression
 * - R(t): spike train (1 at spike times, 0 otherwise)
 * - U_max: maximum utilization (baseline + facilitation)
 * - τ_x, τ_u, τ_y: time constants for recovery and facilitation
 * 
 * **Synaptic Transmission Process**:
 * 1. Pre-synaptic spike → facilitates resources
 * 2. Current transmits after delay through available resources
 * 3. Post-synaptic neuron receives weighted input
 * 4. Eligibility trace updated for reward modulation
 * 5. Synaptic efficacy modulated by recent activity
 * 
 * @see Brain.cpp:269 - spike event registration and delivery
 * @see ../plasticity/STDP.hpp - long-term plasticity
 * @see ../neuromodulation/Neuromodulator.hpp - reward-modulated learning
 */

class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId() const;
    
    // Connection
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    Delay getDelay() const;
    void setDelay(Delay delay);
    
        /**
     * @brief Create synaptic connection between neurons
     * 
     * @details Synapse constructor initializes connection state with
     * biologically plausible parameters and sets up dynamic state management
     * for short-term plasticity and reward-modulated learning.
     * 
     * **Initial STP State**:
     * - Resources available (x): 1.0 (fully available)
     * - Utilization (u): 0.0 (no utilization)
     * - Depression resources (xd): 1.0 (baseline level)
     * - Facilitation: 0.0 (no facilitation)
     * - Weight: 0.0 (will be set during initialization)
     * 
     * @param id Unique synapse identifier
     * @param source Neuron ID of pre-synaptic neuron
     * @param destination Neuron ID of post-synaptic neuron
     * @pre Both neurons must exist in brain
     * @post Synapse state initialized with biological parameters
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    /**
     * @brief Get synapse identifier
     * 
     * @return Unique synapse ID
     * @note ID is assigned by brain during initialization
     */
    SynapseId getId() const;
    
    // Connection
    /**
     * @brief Get pre-synaptic neuron ID
     * 
     * @return Neuron ID that emits spikes for this synapse
     */
    NeuronId getSourceNeuron() const;
    /**
     * @brief Get post-synaptic neuron ID
     * 
     * @return Neuron ID that receives input from this synapse
     */
    NeuronId getDestinationNeuron() const;
    
    // Weight
    /**
     * @brief Get synaptic weight
     * 
     * @return Synaptic weight (positive = excitatory, negative = inhibitory)
     * @note Constrained by synapse type (excitatory, inhibitory, modulatory)
     */
    SynapticWeight getWeight() const;
    /**
     * @brief Set synaptic weight
     * 
     * @details Weight sets the baseline efficacy of synaptic transmission.
     * Type-specific constraints ensure biological plausibility:
     * - Excitatory: [-0.1, 1.0]
     * - Inhibitory: [-1.0, 0.1]
     * - Modulatory: [-0.5, 0.5]
     * 
     * @param weight New synaptic weight
     * @post Weight is clamped to type-specific bounds
     */
    void setWeight(SynapticWeight weight);
    /**
     * @brief Add delta to synaptic weight
     * 
     * @details Used by plasticity rules (STDP, Hebbian) to modify weights
     * incrementally while respecting type constraints.
     * 
     * @param delta Weight change amount
     * @post Weight updated and clamped to valid range
     */
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    /**
     * @brief Get synaptic transmission delay
     * 
     * @return Delay in simulation steps (typically 1-5 steps)
     * @note Delay affects spike transmission timing and coordination
     */
    Delay getDelay() const;
    /**
     * @brief Set synaptic transmission delay
     * 
     * @param delay Delay in simulation steps
     * @post Affects spike transmission timing
     */
    void setDelay(Delay delay);
    
    // Type
    /**
     * @brief Get synapse type
     * 
     * @return Synapse type (Excitatory, Inhibitory, Modulatory)
     * @note Type determines weight constraints and functional role
     */
    SynapseType getType() const;
    /**
     * @brief Set synapse type
     * 
     * @param type New synapse type
     * @post Updates weight constraints and functional properties
     */
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    /**
     * @brief Check if synapse is excitatory
     * 
     * @return true if synapse releases excitatory neurotransmitters
     * @see getType()
     */
    bool isExcitatory() const;
    /**
     * @brief Check if synapse is inhibitory
     * 
     * @return true if synapse releases inhibitory neurotransmitters
     * @see getType()
     */
    bool isInhibitory() const;
    
    // Activity history for plasticity
    /**
     * @brief Record pre-synaptic spike event
     * 
     * @details Records timing of pre-synaptic spikes for plasticity
     * computations, particularly STDP (spike-timing dependent plasticity).
     * 
     * @param timestamp Time of pre-synaptic spike (ms)
     * @post Added to pre-spike history for plasticity calculations
     * @see getPreSpikeHistory()
     * @see recordPostSpike()
     */
    void recordPreSpike(Timestamp timestamp);
    /**
     * @brief Record post-synaptic spike event
     * 
     * @details Records timing of post-synaptic spikes for plasticity
     * computations.
     * 
     * @param timestamp Time of post-synaptic spike (ms)
     * @post Added to post-spike history for plasticity calculations
     * @see getPostSpikeHistory()
     * @see recordPreSpike()
     */
    void recordPostSpike(Timestamp timestamp);
    /**
     * @brief Get pre-synaptic spike history
     * 
     * @return Const reference to vector of pre-synaptic spike timestamps
     * @note Used by STDP and other spike-timing based plasticity rules
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    /**
     * @brief Get post-synaptic spike history
     * 
     * @return Const reference to vector of post-synaptic spike timestamps
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    /**
     * @brief Clear spike history
     * 
     * @post Removes all spike timestamps (used for plasticity reset)
     */
    void clearHistory();
    
    // Plasticity flags
    /**
     * @brief Get plasticity flags
     * 
     * @return Const reference to plasticity configuration flags
     * @see enablePlasticity()
     */
    const PlasticityFlags& getPlasticityFlags() const;
    /**
     * @brief Get plasticity flags (non-const version)
     * 
     * @return Reference to plasticity configuration flags
     */
    PlasticityFlags& getPlasticityFlags();
    /**
     * @brief Enable/disable plasticity rules
     * 
     * @details Configures which plasticity mechanisms are active:
     * 
     * **Hebbian Plasticity**:
     * ```
     * Δw = η · (pre·post) · (1 - w)
     * ```
     * where η is learning rate, pre/post are pre/post spike indicators
     * 
     * **STDP Plasticity**:
     * ```
     * Δw(t) = A_+(t_post - t_pre) if t_pre < t_post
     * Δw(t) = -A_-(t_pre - t_post) if t_post < t_pre
     * ```
     * 
     * **Reward-Modulated Plasticity**:
     * ```
     * Δw = R(t) · E(t) · δ
     * ```
     * where R(t) is reward, E(t) is eligibility trace, δ is learning rate
     * 
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable STDP learning
     * @param rewardModulated Enable reward-modulated learning
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Synaptic efficacy (use-dependent modulation)
    /**
     * @brief Get current synaptic efficacy
     * 
     * @return Efficacy factor (0.0 to efficacyMax)
     * @note Modulates transmission strength independently of weight
     * @see getEfficacyBaseline()
     * @see getEligibilityTrace()
     */
    float getEfficacy() const;
    /**
     * @brief Set synaptic efficacy
     * 
     * @param efficacy New efficacy factor
     * @post Efficacy clamped to [0.0, efficacyMax]
     */
    void setEfficacy(float efficacy);
    /**
     * @brief Get baseline synaptic efficacy
     * 
     * @return Baseline efficacy (default: 1.0)
     * @note Long-term average efficacy level
     */
    float getEfficacyBaseline() const;
    
    // Enhanced STP state (for debugging and analysis)
    /**
     * @brief Get utilization parameter
     * 
     * @return Utilization factor u (0.0 to U_max)
     * @note Fraction of resources used per spike transmission
     * @see getAvailableResources()
     */
    float getUtilization() const;  // u - utilization factor
    /**
     * @brief Get available resources
     * 
     * @return Resources available for transmission (x)
     * @note Scaled by utilization: effective transmission = x * (1-u)
     */
    float getAvailableResources() const;  // x - resources available for transmission
    /**
     * @brief Get depression resources
     * 
     * @return Depression recovery resources (xd)
     * @note Tracks recovery from depression (low values = depressed state)
     */
    float getDepressionResources() const;  // xd - depression recovery resources
    /**
     * @brief Get facilitation variable
     * 
     * @return Facilitation variable (higher = more facilitation)
     * @note Tracks recent activity history (increases with spikes)
     */
    float getFacilitationVariable() const;  // Facilitation variable
    
    // Eligibility trace (for reward-modulated learning)
    /**
     * @brief Get eligibility trace value
     * 
     * @return Eligibility trace (accumulates during learning)
     * @note Tracks synaptic history for reward-modulated plasticity
     * @see decayEligibilityTrace()
     */
    float getEligibilityTrace() const;
    /**
     * @brief Set eligibility trace value
     * 
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    /**
     * @brief Decay eligibility trace over time
     * 
     * @details Eligibility traces decay exponentially:
     * ```
     * E(t+dt) = E(t) * exp(-λ * dt)
     * ```
     * where λ is decay rate
     * 
     * @param decayRate Decay rate λ
     * @post Trace value exponentially decayed
     */
    void decayEligibilityTrace(float decayRate);
    
    // Update synapse for one simulation step
    /**
     * @brief Complete synapse simulation step
     * 
     * @details Updates all synaptic dynamic variables for one timestep:
     * 
     * 1. **Recovery**: Resources recover from depression
     * 2. **Facilitation**: Previous spikes decay
     * 3. **STP update**: New utilization computed
     * 4. **Eligibility trace**: Decays over time
     * 5. **Efficacy**: Modulates based on activity
     * 6. **Weight clamping**: Respects type constraints
     * 
     * **Complete STP Equations**:
     * ```
     * u(t) = U_max * (1 - facilitating_vars)
     * x(t) = xd(t) * (1 - u(t))
     * xd(t+dt) = xd(t) + (xd_0 - xd(t)) * (1 - exp(-dt/τ_xd))
     * facilitating_vars(t+dt) = facilitating_vars(t) * exp(-dt/τ_facilitation)
     * E(t+dt) = E(t) * exp(-λ * dt)
     * efficacy(t+dt) = efficacy(t) + learning_rate * (baseline - efficacy(t))
     * ```
     * 
     * @param currentTime Current simulation time (ms)
     * @post All STP variables updated according to equations above
     * @see ../brain/Neuron.cpp:226 - synapse step integration
     */
    void step(Timestamp currentTime);
    
    // Reset to initial state
    /**
     * @brief Reset synapse to initial state
     * 
     * @details Complete state reset including:
     * - Reset weight to zero
     * - Clear spike history
     * - Reset eligibility trace
     * - Restore baseline efficacy
     * - Reset STP variables to neutral state
     * 
     * @post Synapse ready for new connectivity pattern
     */
    void reset();
    
    // Initialize with random parameters
    /**
     * @brief Initialize synapse with biologically plausible random parameters
     * 
     * @details Generates random synaptic parameters based on type:
     * - **Excitatory**: weight [-0.1, 1.0], efficacy [0.9, 1.2], delay [1,3]
     * - **Inhibitory**: weight [-1.0, -0.1], efficacy [0.8, 1.1], delay [2,5]
     * - **Modulatory**: weight [-0.5, 0.5], efficacy [0.85, 1.0], delay [3,7]
     * 
     * @param rng Reference to random number generator
     * @post Synapse parameters randomly initialized within biological ranges
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm