#pragma once

/**
 * @file Neuron.hpp
 * @brief Neuron class representing a single neuron in the NLM neural network.
 * 
 * This header defines the Neuron class, which implements a Leaky Integrate-and-Fire
 * (LIF) neuron model. Each neuron is the basic computational unit of the network,
 * receiving synaptic inputs, maintaining membrane potential, and generating spike
 * outputs when threshold is reached.
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0.0
 * @see src/core/Types/Types.hpp
 * @see src/plasticity/PlasticityRule.hpp
 * @see src/plasticity/STDP.hpp
 * @see src/plasticity/Hebbian.hpp
 * @see src/agent/AgentBrain.hpp
 * @see TODO Complete LIF implementation (Phase 2)
 */

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

/**
 * Neuron state structure for efficient storage and access.
 * 
 * Contains all state variables needed to simulate neuron dynamics.
 * Stored as a contiguous block in memory for optimal cache performance.
 * Initialized with biologically plausible default values.
 * 
 * @see Neuron
 * @see FiringState
 */
struct NeuronState {
    MembranePotential membranePotential;    /**< Current membrane potential (mV) */
    MembranePotential restingPotential;    /**< Resting potential (mV) */
    MembranePotential threshold;           /**< Firing threshold (mV) */
    MembranePotential resetPotential;       /**< Post-spike reset value (mV) */
    MembranePotential leakConductance;      /**< Leak conductance (nS) */
    MembranePotential synapseConductance;   /**< Synaptic conductance (nS) */
    FiringRate firingRate;                 /**< Current firing rate (Hz) */
    FiringState firingState;               /**< Current firing state */
    uint32_t refractoryRemaining;          /**< Steps remaining in refractory period */
    uint32_t refractoryPeriod;             /**< Total refractory period (steps) */
    float adaptationVariable;              /**< For spike-frequency adaptation */
    float lastSpikeTime;                   /**< Timestamp of last spike (-1 if none) */
    
    /**
     * Default constructor initializes neuron state with biologically plausible values.
     */
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

/**
 * Neuron class implementing Leaky Integrate-and-Fire (LIF) dynamics.
 * 
 * The Neuron class represents a single computational unit in the neural network.
 * It implements the LIF model where the membrane potential integrates incoming
 * synaptic inputs and decays exponentially over time. When the potential
 * reaches threshold, a spike is generated.
 * 
 * Key features:
 * - LIF dynamics with exponential decay
 * - Refractory period after spike
 * - Synaptic integration (excitatory, inhibitory, modulatory)
 * - Plasticity support (STDP, Hebbian, reward-modulated)
 * - Random initialization for variability
 * 
 * @see NeuronState
 * @see PlasticityFlags
 * @see Synapse
 * @see RandomGenerator
 * @warning Current implementation is incomplete - full LIF dynamics pending
 *          (Phase 2: Integration - will complete integrate-and-fire)
 * @note Each neuron maintains connections to other neurons via SynapseHandle
 *       objects, allowing for efficient sparse representation of the network.
 */
class Neuron {
public:
    /**
     * @name Object Lifetime Management
     * @warning Copying neurons is disabled (Config&) to prevent
     * unintended sharing of neural state and connections.
     */
    /// @{
    
    /**
     * @brief Construct neuron with unique identifier
     * 
     * @param id Unique neuron identifier
     * @see NeuronId
     * @see getId()
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor ensures proper cleanup of implementation details
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor is deleted to prevent copying.
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted to prevent copying.
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor enables efficient transfer of neuron state.
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator enables efficient transfer of neuron state.
     */
    Neuron& operator=(Neuron&&) noexcept;
    /// @}
    
    /**
     * @name Identity and Classification
     * 
     * Methods for managing neuron identity and type classification.
     */
    /// @{
    
    /**
     * @brief Get the neuron's unique identifier
     * 
     * @return NeuronId Unique identifier of this neuron
     * @see NeuronId
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's functional type
     * 
     * @return NeuronType Functional type (excitatory, inhibitory, etc.)
     * @see NeuronType
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's functional type
     * 
     * @param type New neuron type
     * @see NeuronType
     */
    void setType(NeuronType type);
    /// @}
    
    /**
     * @name State Management
     * 
     * Methods for accessing and modifying neuron state variables.
     */
    /// @{
    
    /**
     * @brief Get the neuron's current state
     * 
     * @return const NeuronState& Reference to internal state (read-only)
     * @see NeuronState
     * @see getState()
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get the neuron's current state (modifiable)
     * 
     * @return NeuronState& Reference to internal state (modifiable)
     * @see NeuronState
     * @see getState() const
     */
    NeuronState& getState();
    /// @}

} // namespace nlm
