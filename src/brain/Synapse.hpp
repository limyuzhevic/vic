#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @struct InterRegionConnection
 * @brief Represents long-range synaptic connection between brain regions
 * @description Defines connectivity between different neural regions, including source,
 *              target, weight, delay, and plasticity configuration. These connections
 *              enable communication across functional brain areas.
 * @version 1.0.0
 */
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor
     * @description Creates a zero-initialized connection with default values.
     *              Weight and delay default to zero, plasticity flags all false.
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor
     * @param src Source region identifier
     * @param tgt Target region identifier
     * @param w Connection weight (default: 0.0)
     * @param d Transmission delay in steps (default: 1)
     * @description Creates a configured connection between regions.
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Synapse
 * @brief Represents synaptic connection between two neurons with real transmission dynamics
 * @description Implements synaptic communication between neurons including weight-based
 *              transmission, delay, spike-timing-dependent plasticity, and short-term plasticity.
 *              Synapses track pre- and post-synaptic activity for learning and eligibility traces.
 * @note Essential for neural computation - all information flows through synapses
 * @warning During development phase, step() method is a placeholder
 * @version 1.0.0
 * @copyright Copyright (c) 2024 NLM Team
 */
class Synapse {
public:
    /**
     * @brief Construct synapse with connection identifiers
     * @param id Unique synapse identifier
     * @param source Neuron ID of presynaptic (source) neuron
     * @param destination Neuron ID of postsynaptic (target) neuron
     * @description Creates a new synaptic connection between two neurons.
     *              The synapse initializes with default weight and plasticity settings.
     * @warning Synapse IDs must be unique across the entire brain system
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @virtual Destructor
     * @description Cleans up synapse resources and performs any necessary cleanup.
     *              Handles internal state management and activity history.
     */
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get synapse identifier
     * @return Synapse ID
     * @description Returns the unique identifier assigned at construction.
     *              Used for addressing and debugging.
     */
    SynapseId getId() const;
    
    /**
     * @brief Get source neuron identifier
     * @return Presynaptic neuron ID
     * @description Returns the identifier of the neuron that sends signals
     *              through this synapse.
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get target neuron identifier
     * @return Postsynaptic neuron ID
     * @description Returns the identifier of the neuron that receives signals
     *              through this synapse.
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get synaptic weight
     * @return Connection strength (-1.0 to 1.0, sign indicates excitation/inhibition)
     * @description Returns the current synaptic weight, which determines the strength
     *              of the connection. Positive weights indicate excitation, negative
     *              weights indicate inhibition.
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set synaptic weight
     * @param weight New connection strength (-1.0 to 1.0)
     * @description Configures the synaptic weight, immediately affecting transmission strength.
     * @warning Clamping may occur internally to maintain valid weight range
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Adjust synaptic weight by delta
     * @param delta Change in synaptic weight (can be positive or negative)
     * @description Increments or decrements the synaptic weight by the specified amount.
     *              Used for incremental learning and adjustment.
     * @note Preserves sign and may clamp to valid range
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get transmission delay
     * @return Delay in simulation steps (positive integer)
     * @description Returns the number of simulation steps required for signals
     *              to travel from source to target neuron.
     * @note Contributes to temporal dynamics and causality in neural computation
     */
    Delay getDelay() const;
    
    /**
     * @brief Set transmission delay
     * @param delay New delay in simulation steps
     * @description Configures the transmission delay, affecting timing of signal delivery.
     *              Larger delays create temporal offsets in neural communication.
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get synapse type
     * @return Synapse type classification
     * @description Returns the synapse's functional type (Excitatory, Inhibitory, etc.).
     *              This determines the sign and effect of synaptic transmission.
     */
    SynapseType getType() const;
    
    /**
     * @brief Set synapse type
     * @param type New synapse type
     * @description Changes the synapse's functional classification, affecting
     *              how it transmits signals and participates in plasticity.
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     * @return true if synapse primarily excites postsynaptic neuron
     * @description Returns true for excitatory synapses (typically positive weights).
     *              Useful for network analysis and synaptic management.
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * @return true if synapse primarily inhibits postsynaptic neuron
     * @description Returns true for inhibitory synapses (typically negative weights).
     *              Useful for network analysis and synaptic management.
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record presynaptic spike for plasticity
     * @param timestamp Simulation time when presynaptic neuron fired
     * @description Stores the presynaptic spike time for use by spike-timing-dependent
     *              plasticity (STDP) and other activity-dependent learning rules.
     * @note Presynaptic spikes are crucial for temporal learning rules
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record postsynaptic spike for plasticity
     * @param timestamp Simulation time when postsynaptic neuron fired
     * @description Stores the postsynaptic spike time for use by STDP and other
     *              learning rules that depend on both spike timing events.
     * @note Postsynaptic spikes enable LTP/LTD learning in STDP
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get presynaptic spike history
     * @return Const reference to vector of presynaptic spike timestamps
     * @description Returns timestamps of all presynaptic spikes recorded since last clear.
     *              Used by STDP and other presynaptic activity-dependent rules.
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get postsynaptic spike history
     * @return Const reference to vector of postsynaptic spike timestamps
     * @description Returns timestamps of all postsynaptic spikes recorded since last clear.
     *              Used by STDP and other postsynaptic activity-dependent rules.
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear activity history
     * @description Removes all recorded pre- and postsynaptic spike timestamps.
     *              Used to reset eligibility traces and learning history.
     * @note Called during synapse reset operation
     */
    void clearHistory();
    
    /**
     * @brief Get plasticity configuration flags
     * @return Const reference to plasticity flags structure
     * @description Returns flags indicating which plasticity rules are active for this synapse
     *              (Hebbian, STDP, reward-modulated, structural).
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get plasticity configuration flags (mutable)
     * @return Mutable reference to plasticity flags structure
     * @description Returns mutable reference for directly modifying plasticity eligibility.
     * @note Typically modified through enablePlasticity() method
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity rules for synapse
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * @description Configures which plasticity rules can modify this synapse's weight.
     *              Different synapses may have different plasticity profiles.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get eligibility trace value
     * @return Current eligibility trace (-1.0 to 1.0)
     * @description Returns the eligibility trace used for reward-modulated learning.
     *              This value integrates presynaptic and postsynaptic activity for
     *              delayed reward prediction error signals.
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace value
     * @param trace New eligibility trace value
     * @description Configures the eligibility trace directly.
     *              Used for initializing or resetting the trace.
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay eligibility trace by factor
     * @param decayRate Decay factor (typically 0.0-0.1 per timestep)
     * @description Reduces the eligibility trace by the specified decay rate.
     *              This implements the decay of learning eligibility over time.
     * @note Eligibility traces decay even without new spikes
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get synaptic efficacy
     * @return Current synaptic efficacy (0.0 to 1.0)
     * @description Returns the synaptic efficacy that modulates transmission strength
     *              based on recent use and neuromodulatory signals.
     */
    float getEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy
     * @param efficacy New efficacy value (0.0 to 1.0)
     * @description Configures synaptic efficacy directly.
     *              Efficacy represents temporary potentiation/depression.
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Execute one synaptic update step
     * @param currentTime Current simulation time in seconds
     * @description Updates synaptic state based on recorded spike activity and
     *              applies plasticity rules. This is typically called by the brain
     *              system's plasticity updater.
     * @note During development phase, this method is a placeholder
     * @see recordPreSpike
     * @see recordPostSpike
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset synapse to initial state
     * @description Resets all synapse parameters and history to initial values.
     *              Clears spike history, resets weight, and clears eligibility traces.
     * @note Called by brain's reset() method
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random parameters
     * @param rng Random number generator reference
     * @description Randomizes synapse parameters such as weight, type, and delay.
     *              Used for initializing synaptic connections in neural populations.
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
