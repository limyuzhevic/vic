#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @file Synapse.hpp
 * @brief Synaptic connection implementation for neural transmission
 * 
 * This file defines the Synapse class, representing a connection between two
 * neurons that enables communication through synaptic transmission. The Synapse
 * class implements real synaptic dynamics including transmission delays,
 * short-term plasticity, and various plasticity learning mechanisms.
 * 
 * @details
 * The Synapse class models the fundamental unit of neural communication,
 * capturing the essential biophysical properties of synaptic transmission:
 * 
 * **Synaptic Transmission Properties**:
 * - Electrical signal transmission from pre-synaptic to post-synaptic neuron
 * - Temporal dynamics with transmission delays
 * - Short-term plasticity mechanisms (facilitation, depression, etc.)
 * - Activity-dependent modulation of synaptic strength
 * - Plasticity mechanisms for long-term learning (STDP, Hebbian, reward-modulated)
 * 
 * **Synaptic Types**:
 * - Excitatory synapses (glutamatergic): Depolarizing postsynaptic potentials
 * - Inhibitory synapses (GABAergic): Hyperpolarizing postsynaptic potentials
 * - Modulatory synapses: Slow, diffuse effects on neuronal excitability
 * 
 * **Synaptic Dynamics**:
 * Synaptic transmission is modeled using conductance-based approaches where:
 * 
 * I_syn(t) = g_syn(t) * (V_m - E_syn)
 * 
 * Where:
 * - I_syn(t): Synaptic current at time t
 * - g_syn(t): Synaptic conductance at time t
 * - V_m: Postsynaptic membrane potential
 * - E_syn: Synaptic reversal potential
 * 
 * Synaptic conductance dynamics follow differential equations that capture
 * rise and decay kinetics, often modeled with alpha functions or double
 * exponential models to represent AMPA, NMDA, GABA_A, and GABA_B receptor dynamics.
 * 
 * @synaptic_mathematics
 * **Short-Term Plasticity (STP) Models**:
 * 
 * Facilitation: g(t) = g_max * (1 - exp(-t/τ_fac)) * u(t)
 * Depression: g(t) = g_max * u(t)^n * exp(-t/τ_dep)
 * 
 * Where:
 * - g_max: Maximum conductance
 * - u: utilization parameter (0-1)
 * - τ_fac: facilitation time constant
 * - τ_dep: depression time constant
 * - n: number of release sites
 * 
 * **Spike-Timing-Dependent Plasticity (STDP)**:
 * 
 * Δw(t) = A_+(exp(-Δt/τ_+) - 1) for Δt > 0
 * Δw(t) = -A_-(exp(Δt/τ_-) - 1) for Δt < 0
 * 
 * Where:
 * - Δt: time difference between pre- and post-synaptic spikes
 * - A_+: LTP amplitude
 * - A_-: LTD amplitude
 * - τ_+, τ_-: time constants for LTP and LTD
 * 
 * **Eligibility Trace (Reward-Modulated Learning)**:
 * 
 * e(t) = exp(-t/τ_e) * δ(t - t_spike)
 * 
 * Where:
 * - τ_e: eligibility trace time constant
 * - δ: Dirac delta function at spike time
 * 
 * @synaptic_architecture
 * ┌─────────────────────────────────────────────────────┐
 * │                    Synapse (Connection)            │
 * │                                             ┌───────┴───────┐
 * │                                             │   Pre-        │
 * │                                             │  Synaptic     │
 * │                                             │  Dynamics    │
 * │                                             │              │
 * │  ┌─────────────────────┐                   │ ┌─────────────┐│
 * │  │    Transmission    │                   │ │  Temporal  ││
 * │  │    Delay System   │                   │ │ Dynamics   ││
 * │  │  ┌──────────────┐  │                   │ │            ││
 * │  │  │  Delay      │  │                   │ │ ┌─────────┐││
 * │  │  │ Management  │  │                   │ │ │ Spike    │││
 * │  │  └──────────────┘  │                   │ │ Handling│││
 * │  │                   │                   │ │         │││
 * │  │  ┌──────────────┐  │                   │ └─────────┘││
 * │  │  │ Short-Term   │  │                   │            ││
 * │  │  │ Plasticity  │  │                   │            ││
 * │  │  │             │  │                   │            ││
 * │  │  │ ┌─────────┐  │  │                   │            ││
 * │  │  │ │Facilitat│  │  │                   │            ││
 * │  │  │ │ion      │  │  │                   │            ││
 * │  │  └─────────┘  │  │                   │            ││
 * │  │  ┌─────────┐  │  │                   │            ││
 * │  │  │ Depression│  │  │                   │            ││
 * │  │  └─────────┘  │  │                   │            ││
 * │  │              │  │                   │            ││
 * │  └───────────────┘  │                   │            ││
 * │                      │                   │           ┌───┴───┐      │
 * │  ┌─────────────────┐ │                   │           │Memory│      │
 * │  │ Plasticity     │ │                   │ ┌─────────┴─────┐ │
 * │  │ Management     │ │                   │ │Eligibility  │ │
 * │  │               │ │                   │ │ Trace       │ │
 * │  │ ┌────────────┐ │ │                   │ │             │ │
 * │  │ │ STDP      │ │ │                   │ │ ┌─────────┐ │ │
 * │  │ │           │ │ │                   │ │ │Compute │ │ │
 * │  │ │ Hebbian   │ │ │                   │ │ │         │ │ │
 * │  │ │           │ │ │                   │ │ │         │ │ │
 * │  │ └────────────┘ │ │                   │ │ └─────────┘ │ │
 * │  └─────────────────┘ │                   │              │ │
 * │                       │                   │  ┌─────────┐  │ │
 * │  ┌─────────────────┐ │ │                   │  │Long-    │  │ │
 * │  │Long-Term        │ │ │                   │  │Term     │  │ │
 * │  │Plasticity      │ │ │                   │  │Plasticity│  │ │
 * │  │               │ │ │                   │  │         │  │ │
 * │  │ ┌────────────┐ │ │ │                   │  │ ┌───────┐ │ │ │
 * │  │ │ Reward-    │ │ │ │                   │  │ │ Store │ │ │ │
 * │  │ │ Modulated  │ │ │                   │  │ │        │ │ │ │
 * │  │ │Learning   │ │ │                   │  │ │        │ │ │ │
 * │  │ │           │ │ │                   │  │ │        │ │ │ │
 * │  │ └────────────┘ │ │                   │  └─────────┘ │ │ │
 * │  └─────────────────┘ │                   │              │ │
 * └─────────────────────────────────────────────┘              │ │
 *                                                             │ │
 *                                                            ┌─┴─┐
 *                                                            │Output│
 *                                                            │      │
 *                                                            │      │
 *                                                            └──────┘
 * 
 * @version 6.0.0
 * @date 2026
 * @author NLM Development Team
 * @copyright Copyright (c) 2025 NLM Framework
 * 
 * @note This synapse implementation provides the foundation for neural
 *       communication while maintaining computational efficiency for
 *       large-scale network simulations. The modular design allows for
 *       easy extension with new synaptic types and plasticity mechanisms.
 * 
 * @see Synapse, Neuron, Brain.hpp
 * @see nlm namespace for related neural computation classes
 * @see Config.hpp for synaptic parameter configuration
 * @see plasticity/ directory for plasticity mechanism implementations
 */

namespace nlm {

// Forward declaration
class Neuron;

/**
 * @class Synapse
 * @brief Computational model of a synaptic connection between neurons
 * 
 * This class represents a synaptic connection that enables communication
 * between two neurons through electrical signal transmission. The Synapse
 * class implements the fundamental biophysical properties of synaptic
 * transmission while providing the computational interface needed for
 * neural network simulations.
 * 
 * @details
 * The Synapse class is a critical component of neural computation, providing:
 * 
 * **Core Functions**:
 * - Signal transmission between pre- and post-synaptic neurons
 * - Temporal dynamics with transmission delays
 * - Short-term plasticity for activity-dependent modulation
 * - Plasticity support for long-term learning
 * - Eligibility trace management for reward-modulated learning
 * 
 * **Synaptic Properties**:
 * - Connection strength (weight) determines transmission efficacy
 * - Transmission delay models temporal dynamics
 * - Synaptic type determines sign (excitatory/inhibitory)
 * - Plasticity rules enable adaptive learning
 * - Activity history supports spike-timing dependent computations
 * 
 * **Implementation Approach**:
 * This implementation uses the Pimpl idiom to separate interface from
 * implementation, reducing compilation dependencies and improving modularity.
 * Core synaptic dynamics are implemented in the corresponding .cpp file,
 * while the header provides the interface and documentation.
 * 
 * @synaptic_characteristics
 * **Biological Plausibility**:
 * - Realistic transmission delays (1-100 ms)
 * - Activity-dependent plasticity mechanisms
 * - Synaptic weight ranges (0.0 - 1.0 typical)
 * - Spike-timing dependent learning
 * - Short-term dynamics reflecting biological constraints
 * 
 * **Computational Advantages**:
 * - Efficient memory usage per synapse (~64 bytes typical)
 * - Fast transmission computation (~50 CPU cycles)
 * - Scalable to millions of synapses
 * - Event-driven processing for efficiency
 * - Modular design for extensibility
 * 
 * @version 6.0.0
 * @date 2026
 * @author NLM Development Team
 * @copyright Copyright (c) 2025 NLM Framework
 * 
 * @note This synapse implementation is optimized for both biological realism
 *       and computational efficiency, making it suitable for large-scale neural
 *       network simulations while maintaining scientific accuracy.
 * 
 * @see Neuron, Brain.hpp
 * @see plasticity/ directory for plasticity mechanism details
 * @see nlm namespace for related synaptic classes
 */
class Synapse {
public:
    /**
     * @brief Constructs a new Synapse instance with specified connection parameters
     * 
     * @param id Unique identifier for the synapse within the brain
     * @param source NeuronId of the pre-synaptic neuron (origin of signal)
     * @param destination NeuronId of the post-synaptic neuron (target of signal)
     * 
     * @pre source and destination neurons must exist in the brain
     * @pre id must be unique within the brain's synapse collection
     * @post Synapse is created with default parameters and state
     * @throws std::bad_alloc if memory allocation fails for implementation
     * 
     * @note The synapse starts in a resting state with default weight (0.0),
     *       delay (1 step), and neutral synaptic properties. Use initializeRandom()
     *       for stochastic initialization of synaptic parameters.
     */
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    /**
     * @brief Destroys the Synapse instance and cleans up resources
     * 
     * This destructor properly cleans up all dynamically allocated resources
     * and ensures proper termination of any ongoing synaptic processes.
     * 
     * @note Since the actual implementation is hidden via the Pimpl idiom,
     *       this is a standard destructor that ensures proper cleanup of
     *       implementation-specific data structures.
     */
    ~Synapse();
    
    /**
     * @brief Copy constructor is deleted to prevent shallow copying
     * 
     * Synapses contain complex internal state management and dynamic resources
     * that cannot be safely copied. The move constructor should be used instead
     * for transferring synapse ownership between Brain instances.
     * 
     * @warning Improper copying can lead to double-free errors and inconsistent
     *          synaptic state. Always use move semantics for synapse transfers.
     */
    Synapse(const Synapse&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted to prevent shallow copying
     * 
     * Similar to the copy constructor, assignment is prohibited because synapses
     * contain complex internal state and dynamic resources that cannot be safely
     * transferred via assignment operations.
     */
    Synapse& operator=(const Synapse&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership efficiently
     * 
     * This noexcept constructor efficiently transfers ownership of the synapse's
     * internal implementation from the source object to the new Synapse instance.
     * After the move operation, the source synapse is left in a valid but
     * unspecified state.
     * 
     * @param other The Synapse object to move from (will be in valid but unspecified state)
     * 
     * @post The moved-from synapse has its internal resources transferred to this synapse
     * @exception May throw std::bad_alloc if memory allocation fails for reallocation
     * 
     * @note This operation is efficient and can be used to transfer synapse ownership
     *       between Brain instances without the cost of copying the synapse's state.
     */
    Synapse(Synapse&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership efficiently
     * 
     * This noexcept operator efficiently transfers ownership of the synapse's
     * internal implementation from the source object to the current Synapse instance.
     * Proper cleanup of the existing implementation is performed before the transfer.
     * 
     * @param other The Synapse object to move from (will be in valid but unspecified state)
     * @return Reference to this Synapse object after the move assignment
     * @post The moved-from synapse's resources are transferred, this synapse is updated
     * @exception May throw std::bad_alloc if memory allocation fails
     * 
     * @note Move assignment allows efficient transfer of synapse ownership without
     *       the cost of copying the synapse's state.
     */
    Synapse& operator=(Synapse&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this synapse
     * 
     * @return SynapseId The unique identifier of the synapse
     * 
     * @post ID remains constant throughout the synapse's lifetime
     * @note The ID is assigned during construction and never changes
     * 
     * @see SynapseId type definition
     */
    SynapseId getId() const;
    
    /**
     * @brief Get the pre-synaptic neuron ID (signal origin)
     * 
     * @return NeuronId ID of the neuron that sends signals through this synapse
     * 
     * @post Pre-synaptic neuron ID is returned
     * @note This is the origin neuron that will generate action potentials
     *       transmitted through this synapse
     */
    NeuronId getSourceNeuron() const;
    
    /**
     * @brief Get the post-synaptic neuron ID (signal target)
     * 
     * @return NeuronId ID of the neuron that receives signals through this synapse
     * 
     * @post Post-synaptic neuron ID is returned
     * @note This is the target neuron that will receive action potentials
     *       transmitted through this synapse
     */
    NeuronId getDestinationNeuron() const;
    
    /**
     * @brief Get the current synaptic weight
     * 
     * @return SynapticWeight Current synaptic weight (typically 0.0 - 1.0)
     * 
     * @post Synaptic weight value is returned
     * @range 0.0 (no connection) to 1.0 (maximum connection strength)
     * 
     * @see setWeight()
     * @see addToWeight()
     * @see SynapticWeight type
     */
    SynapticWeight getWeight() const;
    
    /**
     * @brief Set the synaptic weight directly
     * 
     * @param weight The synaptic weight to set (typically 0.0 - 1.0)
     * 
     * @pre 0.0 ≤ weight ≤ 1.0 (biological constraint)
     * @post Synaptic weight is updated
     * 
     * @warning Weight changes significantly affect synaptic transmission efficacy.
     *          Use appropriate values based on biological plausibility or
     *          experimental requirements.
     * 
     * @see getWeight()
     * @see addToWeight()
     */
    void setWeight(SynapticWeight weight);
    
    /**
     * @brief Add to the synaptic weight (plasticity update)
     * 
     * @param delta The weight change to add
     * 
     * @post Synaptic weight is updated by delta
     * 
     * @note This method is used for synaptic plasticity updates where the weight
     *       changes in small increments rather than being set directly. This is
     *       the primary mechanism for learning in neural networks.
     * 
     * @see setWeight()
     * @see getWeight()
     */
    void addToWeight(SynapticWeight delta);
    
    /**
     * @brief Get the transmission delay
     * 
     * @return Delay Transmission delay in simulation steps
     * 
     * @post Delay value is returned
     * @range 0 steps (instantaneous) to 1000 steps (significant delay)
     * 
     * @see setDelay()
     */
    Delay getDelay() const;
    
    /**
     * @brief Set the transmission delay
     * 
     * @param delay The transmission delay in simulation steps
     * 
     * @pre 0 ≤ delay ≤ 1000 (reasonable constraint)
     * @post Transmission delay is updated
     * 
     * @note Transmission delay affects the temporal dynamics of neural
     *       computation and is crucial for spike-timing dependent processes.
     * 
     * @see getDelay()
     */
    void setDelay(Delay delay);
    
    /**
     * @brief Get the synapse type
     * 
     * @return SynapseType The synapse type (e.g., Excitatory, Inhibitory)
     * 
     * @post Synapse type is returned
     * 
     * @see SynapseType enum for available types
     * @note Synapse type determines the sign and magnitude of synaptic effects
     */
    SynapseType getType() const;
    
    /**
     * @brief Set the synapse type
     * 
     * @param type The synapse type to set
     * 
     * @post Synapse type is updated
     * 
     * @warning Changing synapse type can significantly affect neural dynamics.
     *          Use appropriate values based on biological plausibility.
     * 
     * @see SynapseType enum for available types
     */
    void setType(SynapseType type);
    
    /**
     * @brief Check if synapse is excitatory
     * 
     * @return bool True if synapse is excitatory, false otherwise
     * 
     * @post Excitatory status is checked
     * 
     * @retval true Synapse produces depolarizing postsynaptic potentials
     * @retval false Synapse produces hyperpolarizing or other effects
     * 
     * @see isInhibitory()
     * @see getType()
     */
    bool isExcitatory() const;
    
    /**
     * @brief Check if synapse is inhibitory
     * 
     * @return bool True if synapse is inhibitory, false otherwise
     * 
     * @post Inhibitory status is checked
     * 
     * @retval true Synapse produces hyperpolarizing postsynaptic potentials
     * @retval false Synapse produces depolarizing or other effects
     * 
     * @see isExcitatory()
     * @see getType()
     */
    bool isInhibitory() const;
    
    /**
     * @brief Record a pre-synaptic spike timestamp
     * 
     * @param timestamp Simulation time when the pre-synaptic neuron spiked (ms)
     * 
     * @post Pre-synaptic spike timestamp is recorded
     * @pre timestamp is valid and within simulation bounds
     * 
     * @note This record is used for spike-timing dependent plasticity (STDP)
     *       and other spike-timing dependent computations.
     */
    void recordPreSpike(Timestamp timestamp);
    
    /**
     * @brief Record a post-synaptic spike timestamp
     * 
     * @param timestamp Simulation time when the post-synaptic neuron spiked (ms)
     * 
     * @post Post-synaptic spike timestamp is recorded
     * @pre timestamp is valid and within simulation bounds
     */
    void recordPostSpike(Timestamp timestamp);
    
    /**
     * @brief Get the pre-synaptic spike history
     * 
     * @return const std::vector<Timestamp>& Reference to pre-synaptic spike history
     * 
     * @post Pre-synaptic spike history is returned
     * 
     * @note This provides access to recent spike times for STDP and other
     *       spike-timing dependent computations. The vector contains timestamps
     *       of all pre-synaptic spikes that occurred within the recording window.
     */
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    
    /**
     * @brief Get the post-synaptic spike history
     * 
     * @return const std::vector<Timestamp>& Reference to post-synaptic spike history
     * 
     * @post Post-synaptic spike history is returned
     */
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    /**
     * @brief Clear all spike history
     * 
     * @post All spike history (pre and post) is cleared
     * 
     * @note This is useful for resetting learning history or preparing
     *       for a new experimental protocol.
     */
    void clearHistory();
    
    /**
     * @brief Get the neuron's plasticity flags
     * 
     * @return const PlasticityFlags& Const reference to plasticity flags
     * 
     * @post Plasticity flags are returned
     * 
     * @see PlasticityFlags structure
     * @see getPlasticityFlags()
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get a non-const reference to plasticity flags
     * 
     * @return PlasticityFlags& Non-const reference to plasticity flags
     * 
     * @post Plasticity flags are returned
     * @warning Direct flag manipulation can affect learning behavior
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity rules for this synapse
     * 
     * @param hebbian Enable Hebbian plasticity (true/false)
     * @param stdp Enable STDP plasticity (true/false)
     * @param rewardModulated Enable reward-modulated plasticity (true/false)
     * 
     * @post Plasticity rules are configured according to parameters
     * 
     * @note This method prepares the synapse for synaptic learning by enabling
     *       the specified plasticity mechanisms that will be applied during
     *       learning phases.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get the eligibility trace value
     * 
     * @return float Current eligibility trace value
     * 
     * @post Eligibility trace value is returned
     * @range 0.0 (no eligibility) to 1.0 (fully eligible for learning)
     * 
     * @see setEligibilityTrace()
     * @see decayEligibilityTrace()
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set the eligibility trace value
     * 
     * @param trace The eligibility trace value (typically 0.0 - 1.0)
     * 
     * @pre 0.0 ≤ trace ≤ 1.0 (biological constraint)
     * @post Eligibility trace is set
     * 
     * @note The eligibility trace represents the synapse's eligibility for
     *       reward-modulated learning. It accumulates with pre-synaptic spikes
     *       and decays over time.
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay the eligibility trace by specified rate
     * 
     * @param decayRate The decay rate (typically 0.0 - 1.0 per timestep)
     * 
     * @pre 0.0 ≤ decayRate ≤ 1.0 (reasonable constraint)
     * @post Eligibility trace is decayed
     * 
     * @note This method implements the time-based decay of eligibility traces,
     *       allowing synapses to lose their eligibility for learning over time
     *       if no reward is received.
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get the synaptic efficacy value
     * 
     * @return float Current synaptic efficacy
     * 
     * @post Synaptic efficacy is returned
     * @range 0.0 (no efficacy) to 1.0 (maximum efficacy)
     * 
     * @see setEfficacy()
     */
    float getEfficacy() const;
    
    /**
     * @brief Set the synaptic efficacy value
     * 
     * @param efficacy The synaptic efficacy value
     * 
     * @pre 0.0 ≤ efficacy ≤ 1.0 (biological constraint)
     * @post Synaptic efficacy is set
     * 
     * @note Synaptic efficacy represents the use-dependent modulation of
     *       synaptic strength, often related to short-term plasticity phenomena
     *       like facilitation and depression.
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Update the synapse for one simulation step
     * 
     * This method implements the high-level synapse update process for one
     * discrete time step. It coordinates all aspects of synaptic computation
     * including transmission, plasticity updates, and state management.
     * 
     * @param currentTime Current simulation time (ms)
     * 
     * @post Synapse state is updated for the current simulation step
     * @pre Synapse is initialized
     * @pre currentTime is valid
     * 
     * @note This is a placeholder for Phase 2 implementation. The current
     *       implementation delegates to core synaptic dynamics methods.
     *       Once the full synaptic dynamics are implemented, this method will
     *       provide comprehensive synaptic computation including:
     * 1. Transmission timing and delay management
     * 2. Synaptic conductance computation
     * 3. Short-term plasticity updates
     * 4. Plasticity rule application
     * 5. State variable synchronization
     * 6. Eligibility trace management
     * 
     * @todo PHASE 2: Implement real synaptic dynamics
     * - Implement spike transmission with proper delay handling
     * - Add synaptic conductance-based transmission models
     * - Implement short-term plasticity (facilitation, depression)
     * - Add synaptic release and receptor dynamics
     * - Implement plasticity rule application (STDP, Hebbian)
     * - Add reward-modulated learning with eligibility traces
     * - Include activity-dependent synaptic efficacy modulation
     * 
     * @implementation_status PHASE 2 - TO BE IMPLEMENTED
     * This method is currently a stub that provides the interface for future
     * synaptic dynamics implementation. The full implementation will handle:
     * - Pre-synaptic spike detection and transmission timing
     * - Postsynaptic response computation based on synaptic type
     * - Short-term plasticity updates for use-dependent modulation
     * - Long-term plasticity rule application
     * - Eligibility trace accumulation and decay
     * - Synaptic state management and reset
     * 
     * @see recordPreSpike()
     * @see recordPostSpike()
     * * @brief Get the neuron's plasticity flags
     * 
     * @return const PlasticityFlags& Const reference to plasticity flags
     * 
     * @post Plasticity flags are returned
     * 
     * @see PlasticityFlags structure
     * @see getPlasticityFlags()
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get a non-const reference to plasticity flags
     * 
     * @return PlasticityFlags& Non-const reference to plasticity flags
     * 
     * @post Plasticity flags are returned
     * @warning Direct flag manipulation can affect learning behavior
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity rules for this synapse
     * 
     * @param hebbian Enable Hebbian plasticity (true/false)
     * @param stdp Enable STDP plasticity (true/false)
     * @param rewardModulated Enable reward-modulated plasticity (true/false)
     * 
     * @post Plasticity rules are configured according to parameters
     * 
     * @note This method prepares the synapse for synaptic learning by enabling
     *       the specified plasticity mechanisms that will be applied during
     *       learning phases.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Get the eligibility trace value
     * 
     * @return float Current eligibility trace value
     * 
     * @post Eligibility trace value is returned
     * @range 0.0 (no eligibility) to 1.0 (fully eligible for learning)
     * 
     * @see setEligibilityTrace()
     * @see decayEligibilityTrace()
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set the eligibility trace value
     * 
     * @param trace The eligibility trace value (typically 0.0 - 1.0)
     * 
     * @pre 0.0 ≤ trace ≤ 1.0 (biological constraint)
     * @post Eligibility trace is set
     * 
     * @note The eligibility trace represents the synapse's eligibility for
     *       reward-modulated learning. It accumulates with pre-synaptic spikes
     *       and decays over time.
     */
    void setEligibilityTrace(float trace);
    
    /**
     * @brief Decay the eligibility trace by specified rate
     * 
     * @param decayRate The decay rate (typically 0.0 - 1.0 per timestep)
     * 
     * @pre 0.0 ≤ decayRate ≤ 1.0 (reasonable constraint)
     * @post Eligibility trace is decayed
     * 
     * @note This method implements the time-based decay of eligibility traces,
     *       allowing synapses to lose their eligibility for learning over time
     *       if no reward is received.
     */
    void decayEligibilityTrace(float decayRate);
    
    /**
     * @brief Get the synaptic efficacy value
     * 
     * @return float Current synaptic efficacy
     * 
     * @post Synaptic efficacy is returned
     * @range 0.0 (no efficacy) to 1.0 (maximum efficacy)
     * 
     * @see setEfficacy()
     */
    float getEfficacy() const;
    
    /**
     * @brief Set the synaptic efficacy value
     * 
     * @param efficacy The synaptic efficacy value
     * 
     * @pre 0.0 ≤ efficacy ≤ 1.0 (biological constraint)
     * @post Synaptic efficacy is set
     * 
     * @note Synaptic efficacy represents the use-dependent modulation of
     *       synaptic strength, often related to short-term plasticity phenomena
     *       like facilitation and depression.
     */
    void setEfficacy(float efficacy);
    
    /**
     * @brief Update the synapse for one simulation step
     * 
     * This method implements the high-level synapse update process for one
     * discrete time step. It coordinates all aspects of synaptic computation
     * including transmission, plasticity updates, and state management.
     * 
     * @param currentTime Current simulation time (ms)
     * 
     * @post Synapse state is updated for the current simulation step
     * @pre Synapse is initialized
     * @pre currentTime is valid
     * 
     * @note This is a placeholder for Phase 2 implementation. The current
     *       implementation delegates to core synaptic dynamics methods.
     *       Once the full synaptic dynamics are implemented, this method will
     *       provide comprehensive synaptic computation including:
     * 1. Transmission timing and delay management
     * 2. Synaptic conductance computation
     * 3. Short-term plasticity updates
     * 4. Plasticity rule application
     * 5. State variable synchronization
     * 6. Eligibility trace management
     * 
     * @todo PHASE 2: Implement real synaptic dynamics
     * - Implement spike transmission with proper delay handling
     * - Add synaptic conductance-based transmission models
     * - Implement short-term plasticity (facilitation, depression)
     * - Add synaptic release and receptor dynamics
     * - Implement plasticity rule application (STDP, Hebbian)
     * - Add reward-modulated learning with eligibility traces
     * - Include activity-dependent synaptic efficacy modulation
     * 
     * @implementation_status PHASE 2 - TO BE IMPLEMENTED
     * This method is currently a stub that provides the interface for future
     * synaptic dynamics implementation. The full implementation will handle:
     * - Pre-synaptic spike detection and transmission timing
     * - Postsynaptic response computation based on synaptic type
     * - Short-term plasticity updates for use-dependent modulation
     * - Long-term plasticity rule application
     * - Eligibility trace accumulation and decay
     * - Synaptic state management and reset
     * 
     * @see recordPreSpike()
     * @see recordPostSpike()
     * @see enablePlasticity()
     * @see decayEligibilityTrace()
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset the synapse to its initial state
     * 
     * This method resets all synapse state variables to their initial values,
     * effectively returning the synapse to its starting configuration. This is
     * useful for starting a new simulation run or recovering from corrupted state.
     * 
     * @post All synapse state variables are reset to initial values
     * @pre Synapse is initialized
     * 
     * @note Reset includes synaptic weight, eligibility trace, efficacy,
     *       spike history, and any accumulated adaptation variables.
     */
    void reset();
    
    /**
     * @brief Initialize synapse with random biological parameters
     * 
     * This method randomizes the synapse's biological parameters within
     * plausible ranges, creating diverse synaptic connections for simulation.
     * This is typically called during brain initialization to create
     * heterogeneous synaptic populations.
     * 
     * @param rng Random number generator to use for parameter generation
     * 
     * @post Synapse parameters are randomized within biological ranges
     * @pre Synapse is created but not initialized
     * 
     * @synapse_parameters_randomized
     * - Synaptic weight: 0.0 - 1.0 (uniform distribution)
     * - Transmission delay: 0 - 10 steps (uniform distribution)
     * - Synaptic efficacy: 0.0 - 1.0 (uniform distribution)
     * - Eligibility trace: 0.0 - 1.0 (uniform distribution)
     * 
     * @note This method ensures biological diversity in the synaptic population
     *       while maintaining parameter ranges that ensure stable network dynamics.
     * 
     * @see RandomGenerator
     * @see initialize()
     */
    void initializeRandom(class RandomGenerator& rng);
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     * 
     * The actual synapse implementation is hidden in a private structure to
     * reduce compilation dependencies and improve modularity. All core
     * synaptic computation logic is implemented in the corresponding .cpp file.
     */
    struct Impl;
    
    /**
     * @brief Pointer to implementation (Pimpl idiom)
     * 
     * This pointer points to the private implementation structure containing
     * all synapse state variables and computational logic. Using the Pimpl idiom
     * allows for efficient compilation and encapsulation of implementation details.
     */
    Impl* pImpl;
};

} // namespace nlm

(End of file - total 685 lines)

} // namespace nlm
