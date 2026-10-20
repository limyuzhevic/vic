#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Forward declaration
class Neuron;

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity
// Based on Tsodyks-Markram model of synaptic dynamics with biological realism

class Synapse {
public:
    /**
     * @brief Constructs a synapse between source and destination neurons
     * 
     * @param id Unique identifier for this synapse
     * @param source Source neuron identifier
     * @param destination Destination neuron identifier
     * 
     * @details
     * Creates a synaptic connection with biologically plausible default parameters:
     * - Initial weight: 0.0 (needs to be set based on connection probability)
     * - Delay: 1 simulation step (typical for fast synapses)
     * - Type: Excitatory (default, can be changed)
     * - Plasticity: All plasticity rules enabled by default
     * - Short-term state: Depression=1.0, Facilitation=0.0 (baseline)
     * - Efficacy: 1.0 (baseline synaptic strength)
     * - Eligibility trace: 0.0 (no previous learning signals)
     * 
     * @note Synapse weights are normalized between -1.0 and 1.0
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper destruction of the pImpl object and cleanup of any
     * allocated memory or resources.
     */
    ~Synapse();
    
    /**
     * @brief Move constructor - transfers ownership of implementation
     * 
     * @param other Source synapse to move from
     * 
     * @details
     * Efficient transfer of the pImpl pointer without deep copying.
     * Source synapse is left in a valid, empty state.
     */
    Synapse(Synapse&& other) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership and cleans up
     * 
     * @param other Source synapse to move from
     * @return Synapse& Reference to this synapse for chaining
     * 
     * @details
     * Proper cleanup of current implementation before transferring ownership.
     * Prevents resource leaks during self-assignment.
     */
    Synapse& operator=(Synapse&& other) noexcept;
    
    /**
     * @brief Explicitly delete copy constructor - synapses cannot be copied
     * 
     * @details
     * Synapses use pImpl idiom with dynamic state. Direct copying
     * would require complex deep copying of spike history and plasticity state.
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * @brief Explicitly delete copy assignment operator - synapses cannot be copied
     * 
     * @details
     * See copy constructor rationale. This prevents accidental synapse duplication.
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Gets the synapse's unique identifier
     * 
     * @return SynapseId The synapse's ID
     */
    SynapseId getId() const;
    
    /**
     * @brief Gets the source neuron identifier
     * 
     * @return NeuronId ID of the presynaptic neuron
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Gets the destination neuron identifier
     * 
     * @return NeuronId ID of the postsynaptic neuron
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Gets the current synaptic weight
     * 
     * @return SynapticWeight Current weight in range [-1.0, 1.0]
     * 
     * @details
     * Synaptic weight determines the strength and sign of the connection.
     * Positive weights = excitatory, negative weights = inhibitory.
     * Weight changes are bounded to prevent runaway excitation.
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Sets the synaptic weight directly
     * 
     * @param weight New weight in range [-1.0, 1.0]
     * 
     * @details
     * Direct weight manipulation. For learning through plasticity rules,
     * prefer using the plasticity system instead of direct weight setting.
     * 
     * @note Weight is clamped to valid range [-1.0, 1.0]
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Adjusts synaptic weight by delta amount
     * 
     * @param delta Change in weight (can be positive or negative)
     * 
     * @details
     * This is the primary method for weight changes during learning.
     * Used by STDP, Hebbian, and reward-modulated plasticity rules.
     * 
     * @note Weight is clamped to valid range [-1.0, 1.0] after adjustment
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Gets the synaptic transmission delay
     * 
     * @return Delay Delay in simulation steps (integer)
     * 
     * @details
     * Represents transmission time from presynaptic to postsynaptic neuron.
     * Real synapses have delays ranging from 0.5ms to several milliseconds.
     * In simulation, delays are measured in timesteps.
     */
    Delay getDelay() const;
    
    /**
     * @brief Sets the synaptic transmission delay
     * 
     * @param delay New delay in simulation steps
     * 
     * @details
     * Delay affects spike timing and STDP effectiveness. Longer delays
     * reduce spike timing precision for learning.
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Gets the synapse type classification
     * 
     * @return SynapseType The synapse type (excitatory, inhibitory, etc.)
     */
    SynapseType getType() const;
    
    /**
     * @brief Sets the synapse type classification
     * 
     * @param type The synapse type to assign
     * 
     * @details
     * Synapse type determines default weight range and learning rules.
     * Excitatory synapses typically have positive weights.
     */
    void setType(SynapseType type);
    
    /**
     * @brief Checks if synapse is excitatory
     * 
     * @return bool True if synapse has positive weight and excitatory type
     */
    bool isExcitatory() const;
    
    /**
     * @brief Checks if synapse is inhibitory
     * 
     * @return bool True if synapse has negative weight or inhibitory type
     */
    bool isInhibitory() const;
    
    /**
     * @brief Records a presynaptic spike event
     * 
     * @param timestamp Time when presynaptic neuron spiked
     * 
     * @details
     * Maintains spike history for spike-timing-dependent plasticity (STDP).
     * STDP uses precise timing of pre- and post-synaptic spikes to modify
     * synaptic weights based on temporal order.
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Records a postsynaptic spike event
     * 
     * @param timestamp Time when postsynaptic neuron spiked
     * 
     * @details
     * Complements presynaptic spike recording for STDP learning.
     * Combined with pre-synaptic history enables timing-dependent plasticity.
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Gets the presynaptic spike history
     * 
     * @return const std::vector<Timestamp>& Vector of presynaptic spike timestamps
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Gets the postsynaptic spike history
     * 
     * @return const std::vector<Timestamp>& Vector of postsynaptic spike timestamps
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clears all spike history records
     * 
     * @details
     * Resets spike history to prepare for new learning period.
     * Called during reset or when plasticity needs to be recalculated.
     */
    void clearHistory();
    
    /**
     * @brief Gets the plasticity flags for this synapse
     * 
     * @return const PlasticityFlags& Reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Gets mutable access to plasticity flags
     * 
     * @return PlasticityFlags& Reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enables specific plasticity rules for this synapse
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * @details
     * Controls which plasticity rules apply to this synaptic connection.
     * Each rule can be independently enabled or disabled.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Gets the eligibility trace for reward-modulated learning
     * 
     * @return float Current eligibility trace value
     * 
     * @details
     * Eligibility traces bridge the gap between actions and delayed rewards.
     * They maintain a trace of recent neural activity that can be modified
     * by reward prediction errors (dopamine signals).
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Sets the eligibility trace value
     * 
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decays the eligibility trace over time
     * 
     * @param decayRate Decay rate per timestep (0.0 to 1.0)
     * 
     * @details
     * Eligibility traces decay over time, giving more weight to recent
     * neural activity. This models the biological trace learning mechanism.
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Gets the synaptic efficacy (use-dependent modulation)
     * 
     * @return float Current efficacy in range [0.0, 1.0]
     * 
     * @details
     * Synaptic efficacy modulates transmission strength based on recent usage.
     * Implements short-term plasticity through mechanisms like depression
     * and facilitation.
     */
    float getEfficacy() const;
    
    /**
     * @brief Sets the synaptic efficacy
     * 
     * @param efficacy New efficacy in range [0.0, 1.0]
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Updates the synapse for one simulation step
     * 
     * @param currentTime Current simulation timestamp
     * 
     * @details
     * Implements Tsodyks-Markram short-term plasticity model:
     * 1. Update synaptic efficacy based on spike history
     * 2. Decay eligibility trace
     * 3. Apply presynaptic and postsynaptic spike effects
     * 4. Compute final synaptic transmission strength
     * 
     * TODO PHASE 2: Implement real synaptic dynamics
     * Current implementation includes short-term plasticity (Tsodyks-Markram model),
     * synaptic efficacy dynamics, and eligibility trace decay
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets the synapse to its initial state
     * 
     * @details
     * Resets all synaptic state variables to initial values:
     * - Weight to initial value
     * - Spike histories to empty
     * - Plasticity state to initial
     * - Efficacy to baseline
     * - Eligibility trace to zero
     */
    void reset();
    
    /**
     * @brief Initializes synapse with random parameters
     * 
     * @param rng Random number generator for stochastic initialization
     * 
     * @details
     * Applies random variations to synapse parameters based on distribution
     * defined in configuration. Used to create diverse connectivity patterns.
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
