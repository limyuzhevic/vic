/**
 * @file Synapse.hpp
 * @brief Synaptic connection between neurons with real dynamics
 * @author NLM Team
 * @date 2026
 * @version Phase 2
 *
 * This file implements synaptic connections between neurons with:
 * - Synaptic transmission with configurable delays
 * - Short-term plasticity (facilitation and depression)
 * - Eligibility traces for reward-modulated learning
 * - Synaptic efficacy (use-dependent modulation)
 * - Spike-timing-dependent plasticity (STDP) support
 *
 * The synapse implements realistic synaptic dynamics including:
 * - Conductance-based transmission
 * - Short-term plasticity (Tsodyks-Markram model)
 * - Activity-dependent efficacy changes
 * - Spike history tracking for plasticity
 */

#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @brief Synaptic connection between two neurons
 * @details Models realistic synaptic transmission with:
 * - Fixed or variable synaptic delays
 * - Short-term plasticity (STP) for dynamic conductance modulation
 * - Eligibility traces for reward-modulated learning
 * - Synaptic efficacy (use-dependent modulation)
 * - Spike history tracking for STDP and other plasticity rules
 */
class Synapse {
public:
    /**
     * @brief Constructs a synaptic connection
     * @param id Unique identifier for this synapse
     * @param source Source neuron identifier
     * @param destination Target neuron identifier
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destructor - releases implementation pointer
     */
    ~Synapse();
    
    /**
     * @brief Disables copying (synapses cannot be shared via copy)
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * @brief Disables copy assignment (synapses cannot be shared via assignment)
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Enables move construction (transfers ownership of implementation)
     * @param other Synapse to move from (will be left in valid but unspecified state)
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Enables move assignment (transfers ownership of implementation)
     * @param other Synapse to move from
     * @return Reference to this synapse (for chaining)
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Gets the synapse's unique identifier
     * @return SynapseId Unique identifier
     */
    SynapseId getId() const;
    
    /**
     * @brief Gets the source neuron identifier
     * @return NeuronId Source neuron identifier
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Gets the target neuron identifier
     * @return NeuronId Target neuron identifier
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Gets the current synaptic weight
     * @return SynapticWeight Current synaptic weight (-1.0 to 1.0)
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Sets the synaptic weight
     * @param weight New synaptic weight
     * @note Weight is clamped to valid range [-1.0, 1.0] to prevent instability
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Adds to the synaptic weight (in-place)
     * @param delta Weight change
     * @note Resulting weight is clamped to valid range [-1.0, 1.0]
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Gets the synaptic delay
     * @return Delay Synaptic delay in simulation steps
     */
    Delay getDelay() const;
    
    /**
     * @brief Sets the synaptic delay
     * @param delay Synaptic delay in simulation steps
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Gets the synapse type
     * @return SynapseType Synapse type (excitatory, inhibitory, modulatory)
     */
    SynapseType getType() const;
    
    /**
     * @brief Sets the synapse type
     * @param type Synapse type to set
     */
    void setType(SynapseType type);
    
    /**
     * @brief Checks if this is an excitatory synapse
     * @return bool True if synapse is excitatory
     */
    bool isExcitatory() const;
    
    /**
     * @brief Checks if this is an inhibitory synapse
     * @return bool True if synapse is inhibitory
     */
    bool isInhibitory() const;
    
    /**
     * @brief Records a presynaptic spike
     * @param timestamp Simulation timestamp when pre-synaptic neuron spiked
     * @details Updates spike history and drives short-term plasticity
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Records a postsynaptic spike
     * @param timestamp Simulation timestamp when post-synaptic neuron spiked
     * @details Updates spike history and drives STDP plasticity
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Gets presynaptic spike history
     * @return const std::vector<Timestamp>& Vector of pre-synaptic spike timestamps
     * @details Maintains rolling window of recent presynaptic spikes (max MAX_SPIKE_HISTORY)
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Gets postsynaptic spike history
     * @return const std::vector<Timestamp>& Vector of post-synaptic spike timestamps
     * @details Maintains rolling window of recent postsynaptic spikes (max MAX_SPIKE_HISTORY)
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clears spike history
     * @details Resets both pre- and post-synaptic spike timestamp buffers
     */
    void clearHistory();
    
    /**
     * @brief Gets plasticity flags controlling which plasticity rules are active
     * @return const PlasticityFlags& Current plasticity configuration
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Gets plasticity flags (mutable)
     * @return PlasticityFlags& Mutable plasticity configuration
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enables/disables specific plasticity rules
     * @param hebbian Enable Hebbian plasticity (SSRN)
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Gets eligibility trace value
     * @return float Current eligibility trace value
     * @details Used for reward-modulated learning, decays over time
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Sets eligibility trace value
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decays eligibility trace by specified factor
     * @param decayRate Decay factor per timestep (0.0 to 1.0)
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Gets synaptic efficacy
     * @return float Current synaptic efficacy (0.0 to 2.0)
     * @details Use-dependent modulation of synaptic strength
     */
    float getEfficacy() const;
    
    /**
     * @brief Sets synaptic efficacy
     * @param efficacy New synaptic efficacy (0.0 to 2.0)
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Performs one simulation step
     * @param currentTime Current simulation timestamp
     * @details Updates synaptic dynamics including:
     * - Short-term plasticity decay
     * - Eligibility trace decay
     * - Weight bounds enforcement
     * - Use-dependent efficacy updates
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets synapse to initial state
     * @details Clears spike history, resets weight, efficacy, and plasticity state
     */
    void reset();
    
    /**
     * @brief Initializes synapse with random biological parameters
     * @param rng Random number generator for stochastic initialization
     * @details Sets biologically plausible parameters based on synapse type:
     * - Excitatory: positive weights (0.1-0.4), moderate efficacy (0.8-1.0)
     * - Inhibitory: negative weights (-0.1 to -0.4), moderate efficacy (0.8-1.0)
     * - Other types: small random weights (-0.1 to 0.1), high efficacy (0.9-1.0)
     */
    void initializeRandom(class RandomGenerator& rng);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
