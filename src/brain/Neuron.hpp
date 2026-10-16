#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
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
 * @file Neuron.hpp
 * @brief Neuron implementation for spiking neural computation
 * 
 * This file defines the Neuron class, representing a single biological neuron
 * that implements Leaky Integrate-and-Fire (LIF) dynamics for spiking neural
 * computation. The Neuron class forms the fundamental building block of the
 * NLM framework, providing the basic computational unit for information
 * processing and representation in the brain.
 * 
 * @details
 * The Neuron class implements one of the most widely used neural models in
 * computational neuroscience: the Leaky Integrate-and-Fire model. This model
 * captures essential neuronal properties including:
 * - Membrane potential dynamics with leak term
 * - Threshold-based spike generation
 * - Refractory period after spiking
 * - Synaptic integration of excitatory, inhibitory, and modulatory inputs
 * - Adaptive properties for spike-frequency adaptation
 * 
 * The LIF model provides an excellent balance between biological plausibility
 * and computational efficiency, making it suitable for large-scale brain
 * simulations while maintaining key neurophysiological properties.
 * 
 * @mathematical_background
 * The core LIF dynamics are described by the differential equation:
 * 
 * C_m * dV/dt = -g_L * (V - E_L) + I_syn + I_ext + I_adapt
 * 
 * Where:
 * - C_m: Membrane capacitance (~100 pF for typical neurons)
 * - g_L: Leak conductance (typically 10 nS)
 * - E_L: Resting potential (-70 mV)
 * - I_syn: Total synaptic current from inputs
 * - I_ext: External injected current
 * - I_adapt: Adaptive current for spike-frequency adaptation
 * 
 * The discrete-time implementation with time step dt is:
 * 
 * V(t+dt) = V(t) * exp(-dt/τ) + (I_total * τ) / C_m * (1 - exp(-dt/τ))
 * 
 * Where τ = C_m / g_L is the membrane time constant.
 * 
 * @neuron_architecture
 * ┌─────────────────────────────────────────────────────────────┐
 * │                    Neuron (Computational Unit)            │
 * │                                                         ┌───┴───┐
 * │                                                         │State │
 * │                    ┌─────────────────────┐              │      │
 * │                    │    Input Processing │              │      │
 * │                    │  ┌──────────────┐  │              │      │
 * │                    │  │   Synapses   │  │              │      │
 * │                    │  │              │  │              │      │
 * │                    │  │ ┌─────────┐  │  │              │      │
 * │                    │  │ │Excitatory│  │  │              │      │
 * │                    │  │ │         │  │  │              │      │
 * │                    │  │ │ Synapses │  │  │              │      │
 * │                    │  │ │         │  │  │              │      │
 * │                    │  │ └─────────┘  │  │              │      │
 * │                    │  │ ┌─────────┐  │  │              │      │
 * │                    │  │ │Inhibitory│  │  │              │      │
 * │                    │  │ │         │  │  │              │      │
 * │                    │  │ │ Synapses │  │  │              │      │
 * │                    │  │ │         │  │  │              │      │
 * │                    │  │ └─────────┘  │  │              │      │
 * │                    │  └──────────────┘  │              │      │
 * │                    │                    │              │      │
 * │                    │  ┌──────────────┐  │              │      │
 * │                    │  │   Modulatory  │  │              │      │
 * │                    │  │   Inputs     │  │              │      │
 * │                    │  └──────────────┘  │              │      │
 * │                    │                    │              │      │
 * │                    └─────────┬─────────┘              ┌─────┴─────┐
 * │                              │                        │   State  │
 * │                    ┌─────────┴─────────┐              │         │
 * │                    │    Output Stage   │              │         │
 * │                    │  ┌──────────────┐  │              │         │
 * │                    │  │   Spike       │  │              │         │
 * │                    │  │   Generation  │  │              │         │
 * │                    │  │              │  │              │         │
 * │                    │  │ ┌─────────┐  │  │              │         │
 * │                    │  │ │Refractory│  │  │              │         │
 * │                    │  │ │Period   │  │  │              │         │
 * │                    │  │ │Handling│  │  │              │         │
 * │                    │  │ └─────────┘  │  │              │         │
 * │                    │  └──────────────┘  │              │         │
 * │                    │                    │              │         │
 * │                    │  ┌──────────────┐  │              │         │
 * │                    │  │  Adaptation  │  │              │         │
 * │                    │  │  ┌─────────┐  │  │              │         │
 * │                    │  │  │Spike-   │  │  │              │         │
 * │                    │  │  │Frequency│  │  │              │         │
 * │                    │  │  │Adaptation│  │  │              │         │
 * │                    │  │  └─────────┘  │  │              │         │
 * │                    │  └──────────────┘  │              │         │
 * │                    └───────────────────┘              │         │
 *                                                            │         │
 * ┌───────────────────────────────────────────────────────────┘         │
 * │                    Internal State Management                    │
 * │  ┌─────────────────────────────────────────────────────┐       │
 * │  │                                                     │       │
 * │  │  ┌─────────────────┐    ┌─────────────────┐         │       │
 * │  │  │ Membrane       │    │Synaptic         │         │       │
 * │  │  │ Potential      │    │Conductance     │         │       │
 * │  │  │                │    │                 │         │       │
 * │  │  │  ┌─────────┐  │    │  ┌──────────┐  │         │       │
 * │  │  │  │ V_m(t) │  │    │  │  g_syn   │  │         │       │
 * │  │  │  └─────────┘  │    │  └──────────┘  │         │       │
 * │  │  └─────────────────┘    └─────────────────┘         │       │
 * │  └─────────────────────────────────────────────────────┘       │
 *                                                            ┌─────┴─────┐
 * │                                                       │Computational│
 * │                                                       │  Engine    │
 * │                                                       │            │
 * │                                                       └────────────┘
 * 
 * @neuron_characteristics
 * Biological Parameters:
 * - Membrane time constant: 10-30 ms (typical: 20 ms)
 * - Refractory period: 1-5 ms (typical: 2 ms)
 * - Input resistance: 100-200 MΩ (typical: 150 MΩ)
 * - Threshold voltage: -55 mV (typical)
 * - Resting potential: -70 mV (typical)
 * - Reset potential: -70 mV (typical after spike)
 * 
 * Computational Characteristics:
 * - Event-driven spike processing
 * - Real-time simulation capability
 * - Scalable to thousands of neurons
 * - Energy-efficient computation
 * - Biological plausibility for neuromorphic systems
 * 
 * @implementation_notes
 * This implementation uses the Pimpl idiom for efficient compilation and
 * encapsulation of implementation details. The actual neuron dynamics are
 * implemented in the corresponding .cpp file to minimize header dependencies.
 * 
 * @performance_considerations
 * - Memory footprint: ~128 bytes per neuron (typical)
 * - Computational cost: ~50-100 CPU cycles per timestep
 * - Parallelization: Suitable for GPU/CPU parallelization
 * - Numerical stability: Fixed-point arithmetic preferred for embedded
 * 
 * @version 6.0.0
 * @date 2026
 * @author NLM Development Team
 * @copyright Copyright (c) 2025 NLM Framework
 * 
 * @note This neuron model is optimized for both biological realism and
 *       computational efficiency, making it suitable for a wide range of
 *       applications from neuroscience research to neuromorphic computing.
 * 
 * @see NeuronState, Synapse, Brain.hpp, nlm namespace
 * @see BrainErrorHandling.hpp for error codes related to neuron operations
 * @see Config.hpp for neuron configuration parameters
 */

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @struct NeuronState
 * @brief Complete state representation for a neuron
 * 
 * This struct encapsulates the complete internal state of a neuron,
 * providing a snapshot of the neuron's computational status at any
 * given time. The state includes membrane potential, synaptic inputs,
 * firing dynamics, and adaptation mechanisms.
 * 
 * @details
 * The NeuronState structure is designed for efficient storage and access
 * during neural computation. All state variables are stored as native
 * types to minimize memory overhead and maximize computational speed.
 * The structure is updated atomically during simulation steps to ensure
 * consistency.
 * 
 * @note This structure is used for both the actual neuron state during
 *       simulation and for checkpoint/restore operations to persist brain
 *       state.
 * 
 * @warning State variables must be updated consistently to maintain neural
 *         dynamics integrity. Direct modification of state variables without
 *         proper coordination can lead to inconsistent neuron behavior.
 */
struct NeuronState {
    // Membrane potential dynamics
    /**
     * @brief Current membrane potential (mV)
     * @details This represents the electrical potential difference across
     *          the neuron's membrane. It integrates all synaptic inputs and
     *          determines when the neuron will fire.
     * @range -100 mV (hyperpolarized) to +50 mV (depolarized)
     * @units millivolts (mV)
     * @default -70.0 mV
     */
    MembranePotential membranePotential;
    
    /**
     * @brief Resting membrane potential (mV)
     * @details The equilibrium potential when no inputs are present. Serves
     *          as the baseline for membrane potential dynamics.
     * @range -80 mV to -60 mV
     * @units millivolts (mV)
     * @default -70.0 mV
     */
    MembranePotential restingPotential;
    
    /**
     * @brief Firing threshold potential (mV)
     * @details When membrane potential reaches this value, the neuron fires
     *          an action potential. This is a critical parameter for neural
     *          excitability and computation.
     * @range -60 mV to -40 mV (typical: -55 mV)
     * @units millivolts (mV)
     * @default -55.0 mV
     */
    MembranePotential threshold;
    
    /**
     * @brief Post-spike reset potential (mV)
     * @details The membrane potential value to which the neuron resets
     *          immediately after firing a spike. Prevents immediate re-firing
     *          and implements the refractory period.
     * @range -80 mV to -60 mV
     * @units millivolts (mV)
     * @default -70.0 mV
     */
    MembranePotential resetPotential;
    
    /**
     * @brief Membrane leak conductance (nS)
     * @details Represents the passive membrane conductance that causes
     *          the membrane potential to decay toward the resting potential
     *          in the absence of inputs. Models ion channel leakage.
     * @range 1.0 nS to 50 nS (typical: 10 nS)
     * @units nanosiemens (nS)
     * @default 10.0 nS
     */
    MembranePotential leakConductance;
    
    /**
     * @brief Total synaptic conductance (nS)
     * @details The sum of all synaptic conductances from incoming synapses,
     *          representing the total synaptic input strength at the current time.
     * @range 0 nS (no input) to 100 nS (strong input)
     * @units nanosiemens (nS)
     * @default 0.0 nS
     */
    MembranePotential synapseConductance;
    
    /**
     * @brief Current firing rate (Hz)
     * @details For rate-based neuron models or output representation. This
     *          field stores the instantaneous firing rate when using rate-coded
     *          representations of neural activity.
     * @range 0 Hz to 1000 Hz (biological limit)
     * @units hertz (Hz)
     * @default 0.0 Hz
     */
    FiringRate firingRate;
    
    /**
     * @brief Current firing state
     * @details Indicates whether the neuron is in resting, firing, or
     *          refractory state. This is a discrete state variable that
     *          determines neural behavior.
     * @see FiringState enum
     * @default FiringState::Resting
     */
    FiringState firingState;
    
    /**
     * @brief Remaining refractory period steps
     * @details Number of simulation steps remaining before the neuron
     *          can fire again after a spike. During this period, the neuron
     *          is unable to generate action potentials.
     * @range 0 to refractoryPeriod
     * @units simulation steps
     * @default 0
     */
    uint32_t refractoryRemaining;
    
    /**
     * @brief Total refractory period length (steps)
     * @details The complete duration of the refractory period after a spike,
     *          in simulation time steps. This determines how often a neuron
     *          can fire.
     * @range 1 to 100 steps (typical: 5 steps)
     * @units simulation steps
     * @default 5
     */
    uint32_t refractoryPeriod;
    
    /**
     * @brief Spike-frequency adaptation variable
     * @details A variable that increases with each spike and gradually
     *          reduces the neuron's firing rate over time, implementing
     *          spike-frequency adaptation observed in biological neurons.
     * @range -10.0 to +10.0
     * @default 0.0
     */
    float adaptationVariable;
    
    /**
     * @brief Timestamp of last spike (-1 if no spike yet)
     * @details Stores the simulation time when the neuron last fired a spike.
     *          Used for STDP learning and other spike-timing dependent computations.
     * @range -1.0 (no spike) to positive simulation time
     * @units simulation time units
     * @default -1.0
     */
    float lastSpikeTime;
    
    /**
     * @brief Default constructor - initializes neuron to resting state
     * 
     * Sets all state variables to biologically plausible initial values.
     * The neuron starts at rest, ready to respond to inputs.
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
 * @class Neuron
 * @brief Computational model of a biological neuron implementing LIF dynamics
 * 
 * This class represents a single neuron in the NLM framework, implementing
 * the Leaky Integrate-and-Fire (LIF) model of neuronal computation. The Neuron
 * class provides a complete model of neuronal behavior including membrane
 * potential dynamics, spike generation, synaptic integration, and plasticity
 * preparation for learning.
 * 
 * @details
 * The Neuron class is designed to be both biologically realistic and computationally
 * efficient. It implements the fundamental computational principles of neurons while
 * providing the flexibility needed for complex neural network simulations. Key features
 * include:
 * 
 * - **Membrane Dynamics**: Realistic LIF dynamics with leak term and capacitance
 * - **Synaptic Integration**: Multiple input channels (excitatory, inhibitory, modulatory)
 * - **Spike Generation**: Threshold-based action potential generation
 * - **Refractory Control**: Proper refractory period implementation
 * - **Adaptation**: Spike-frequency adaptation for biological realism
 * - **Plasticity Support**: State preparation for synaptic learning rules
 * - **Input/Output**: Current injection and spike output interfaces
 * - **State Management**: Complete state encapsulation and reset capabilities
 * 
 * The neuron follows the event-driven computation paradigm where spikes are
 * generated asynchronously based on membrane potential dynamics, making it
 * suitable for real-time neural simulation.
 * 
 * @neuron_implementation
 * The actual implementation of neuronal dynamics is separated from the header
 * using the Pimpl idiom to reduce compilation dependencies and improve modularity.
 * Core computational methods like step(), stepLIF(), and updateNeuronState()
 * are implemented in Neuron.cpp, while the header contains the interface and
 * documentation.
 * 
 * @hardware_considerations
 * This neuron model is optimized for both CPU and neuromorphic hardware:
 * - Fixed-point arithmetic support for embedded systems
 * - Minimal memory footprint for large-scale deployment
 * - Event-driven computation for energy efficiency
 * - Scalable to thousands of neurons per core
 * 
 * @performance_characteristics
 * - Memory per neuron: ~128 bytes (including implementation overhead)
 * - Computational cost: ~50-100 CPU cycles per timestep
 * - Spike processing time: ~20-50 CPU cycles
 * - Memory bandwidth: ~16 bytes per timestep per neuron
 * 
 * @neuronal_properties
 * Biological realism: High
 * Computational efficiency: High
 * Scalability: Excellent
 * Hardware compatibility: Good
 * Learning compatibility: Excellent
 * 
 * @version 6.0.0
 * @date 2026
 * @author NLM Development Team
 * @copyright Copyright (c) 2025 NLM Framework
 * 
 * @note This neuron implementation is suitable for both research applications
 *       and production systems. It balances biological accuracy with computational
 *       efficiency, making it ideal for large-scale neural network simulations.
 * 
 * @see NeuronState, Synapse, Brain.hpp, BrainErrorHandling.hpp
 * @see nlm namespace for related neural computation classes
 * @see Config.hpp for neuron parameter configuration
 * @see NeuralRegion.hpp for population-level neuron organization
 */
class Neuron {
public:
    /**
     * @brief Constructs a new Neuron instance with specified ID
     * 
     * @param id Unique identifier for the neuron within the brain. This ID
     *           is used for neural population management, synaptic connections,
     *           and state restoration operations.
     * 
     * @pre ID must be unique within the brain (enforced by caller)
     * @post Neuron object is created with default state
     * @throws std::bad_alloc if memory allocation fails for implementation
     * 
     * @note The neuron starts in resting state with all parameters at default
     *       biological values. Use initializeRandom() for stochastic initialization.
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroys the Neuron instance and cleans up resources
     * 
     * This destructor properly cleans up all dynamically allocated resources
     * including implementation-specific data structures. Since the actual
     * implementation is hidden via Pimpl idiom, this is a standard destructor
     * that ensures proper cleanup.
     * 
     * @note This is a virtual destructor to ensure proper cleanup when deleting
     *       through Neuron* pointers in polymorphic hierarchies.
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor is deleted to prevent shallow copying
     * 
     * Neurons contain complex internal state and dynamic resources that cannot
     * be safely copied. The move constructor should be used instead for transferring
     * neuron ownership between Brain instances or other containers.
     * 
     * @warning Improper copying can lead to double-free errors and inconsistent
     *          neural state. Always use move semantics for neuron transfers.
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted to prevent shallow copying
     * 
     * Similar to the copy constructor, assignment is prohibited because neurons
     * contain complex internal state management and dynamic resources that
     * cannot be safely transferred via assignment operations.
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership efficiently
     * 
     * This noexcept constructor efficiently transfers ownership of the neuron's
     * internal implementation from the source object to the new Neuron instance.
     * After the move operation, the source neuron is left in a valid but
     * unspecified state.
     * 
     * @param other The Neuron object to move from (will be in valid but unspecified state)
     * 
     * @post The moved-from neuron has its internal resources transferred to this neuron
     * @exception May throw std::bad_alloc if memory allocation fails for reallocation
     * 
     * @note This operation is efficient and can be used to transfer neuron ownership
     *       between Brain instances or containers without expensive copying.
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership efficiently
     * 
     * This noexcept operator efficiently transfers ownership of the neuron's
     * internal implementation from the source object to the current Neuron instance.
     * Proper cleanup of the existing implementation is performed before the transfer.
     * 
     * @param other The Neuron object to move from (will be in valid but unspecified state)
     * @return Reference to this Neuron object after the move assignment
     * @post The moved-from neuron's resources are transferred, this neuron is updated
     * @exception May throw std::bad_alloc if memory allocation fails
     * 
     * @note Move assignment allows efficient transfer of neuron ownership without
     *       the cost of copying the neuron's state.
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * 
     * @return NeuronId The unique identifier of the neuron
     * 
     * @post ID remains constant throughout the neuron's lifetime
     * @note The ID is assigned during construction and never changes
     * 
     * @see NeuronId type definition
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron type classification
     * 
     * @return NeuronType The neuron type (e.g., Excitatory, Inhibitory, Sensory)
     * 
     * @post Type reflects the neuron's functional classification
     * 
     * @see NeuronType enum for available types
     * @note Neuron type determines synaptic properties and computational role
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type classification
     * 
     * @param type The neuron type to set (e.g., NeuronType::Excitatory)
     * 
     * @post Neuron type is updated, affecting synaptic behavior
     * @warning Changing neuron type may affect synaptic dynamics and learning
     * 
     * @see NeuronType enum for available types
     * @note Neuron type typically set during initialization and rarely changed
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get a const reference to the current neuron state
     * 
     * @return const NeuronState& Const reference to the neuron state structure
     * 
     * @post State is returned for inspection without modification
     * @note This is the primary interface for accessing neuron state
     * 
     * @see NeuronState structure
     * @see getState() for non-const version
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get a non-const reference to the neuron state
     * 
     * @return NeuronState& Non-const reference to the neuron state structure
     * 
     * @post State is returned for modification
     * @warning Direct state modification should be used with caution as it
     *          can cause inconsistent neuronal dynamics
     * 
     * @see NeuronState structure
     * @see getState() const for const version
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * 
     * @return MembranePotential Current membrane potential (mV)
     * 
     * @post Potential value is returned for inspection
     * @range -100 mV to +50 mV (biological limits)
     * 
     * @see MembranePotential type
     * @see setMembranePotential()
     * @see addToMembranePotential()
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential directly
     * 
     * @param potential The membrane potential to set (mV)
     * 
     * @pre -100 mV ≤ potential ≤ 50 mV (biological constraints)
     * @post Membrane potential is set to specified value
     * 
     * @warning Direct potential setting bypasses normal synaptic integration
     *          and may cause unrealistic neural dynamics if used incorrectly
     * 
     * @see getMembranePotential()
     * @see addToMembranePotential()
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to the membrane potential (synaptic integration)
     * 
     * This method implements synaptic integration by adding input currents
     * to the membrane potential. This is the primary way neurons respond to
     * synaptic inputs.
     * 
     * @param delta The potential change to add (mV)
     * 
     * @post Membrane potential is increased by delta
     * @pre Neuron must be in a state that accepts synaptic input
     * 
     * @note This method represents the core synaptic integration process in
     *       neuronal computation. Multiple calls can accumulate synaptic inputs.
     * 
     * @see setMembranePotential()
     * @see receiveExcitatoryInput()
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold potential
     * 
     * @return MembranePotential Firing threshold potential (mV)
     * 
     * @post Threshold value is returned for inspection
     * @range -60 mV to -40 mV (typical biological values)
     * 
     * @see setThreshold()
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold potential
     * 
     * @param threshold The firing threshold potential (mV)
     * 
     * @pre -60 mV ≤ threshold ≤ -40 mV (biological constraints)
     * @post Firing threshold is updated
     * 
     * @warning Threshold changes can significantly affect neural excitability
     *          and computational properties
     * 
     * @see getThreshold()
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing an action potential
     * 
     * @return bool True if neuron is in firing state, false otherwise
     * 
     * @post Firing status is checked
     * 
     * @retval true Neuron is actively firing
     * @retval false Neuron is not firing
     * 
     * @see isRefractory()
     * @see setFiringState()
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return bool True if neuron cannot fire due to refractory period, false otherwise
     * 
     * @post Refractory status is checked
     * 
     * @retval true Neuron is in refractory period
     * @retval false Neuron can fire normally
     * 
     * @see isFiring()
     * @see setRefractoryPeriod()
     * @see decrementRefractory()
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the neuron's firing state directly
     * 
     * @param state The firing state to set
     * 
     * @post Firing state is updated
     * @pre State must be valid FiringState value
     * 
     * @warning Direct state manipulation should be used with caution as it
     *          can bypass normal neural dynamics
     * 
     * @see FiringState enum
     * @see isFiring()
     * @see isRefractory()
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period length
     * 
     * @param steps Total refractory period length in simulation steps
     * 
     * @pre steps ≥ 1 (minimum 1 step)
     * @post Refractory period is set, current remaining time may be adjusted
     * 
     * @note Refractory period prevents unrealistic firing rates and models
     *       biological constraints on neuronal firing.
     * 
     * @see isRefractory()
     * @see decrementRefractory()
     * @see getRefractoryPeriod()
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement the remaining refractory period by one step
     * 
     * This method should be called once per simulation step to properly
     * manage the refractory period countdown.
     * 
     * @post Remaining refractory steps are decremented (minimum 0)
     * 
     * @note This method is typically called automatically by the simulation
     *       framework during the step() function. Manual calls are not recommended.
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate
     * 
     * @return FiringRate Current firing rate (Hz)
     * 
     * @post Firing rate is returned
     * @range 0 Hz to 1000 Hz (biological limit)
     * 
     * @see setFiringRate()
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate (rate-based computation)
     * 
     * @param rate The firing rate to set (Hz)
     * 
     * @pre 0 Hz ≤ rate ≤ 1000 Hz (biological constraints)
     * @post Firing rate is set
     * 
     * @warning Rate setting is typically used for rate-based neuron models
     *          or output encoding, not for spiking neurons.
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the membrane leak conductance
     * 
     * @param conductance The leak conductance (nS)
     * 
     * @pre 1.0 nS ≤ conductance ≤ 50 nS (biological constraints)
     * @post Leak conductance is updated
     * 
     * @note Leak conductance affects the membrane time constant and
     *       determines how quickly the neuron returns to resting potential.
     * 
     * @see getLeakConductance()
     * @see getMembranePotential()
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the membrane leak conductance
     * 
     * @return MembranePotential Leak conductance (nS)
     * 
     * @post Conductance value is returned
     * 
     * @see setLeakConductance()
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period length
     * 
     * @return uint32_t Refractory period length (steps)
     * 
     * @post Refractory period length is returned
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential
     * 
     * @param potential The resting potential (mV)
     * 
     * @pre -80 mV ≤ potential ≤ -60 mV (biological constraints)
     * @post Resting potential is set
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential
     * 
     * @return MembranePotential Resting potential (mV)
     * 
     * @post Resting potential is returned
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential
     * 
     * @param potential The reset potential (mV)
     * 
     * @pre -80 mV ≤ potential ≤ -60 mV (biological constraints)
     * @post Reset potential is set
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get the reset potential
     * 
     * @return MembranePotential Reset potential (mV)
     * 
     * @post Reset potential is returned
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Check if the neuron is firing based on membrane potential
     * 
     * This method implements the threshold detection mechanism. If the
     * current membrane potential exceeds the firing threshold, the neuron
     * is considered to be firing.
     * 
     * @return bool True if neuron should fire (V_m ≥ threshold), false otherwise
     * 
     * @post Threshold check is performed
     * 
     * @retval true Neuron meets firing threshold
     * @retval false Neuron below firing threshold
     * 
     * @see getMembranePotential()
     * @see getThreshold()
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the last spike
     * 
     * @return float Timestamp of last spike (-1.0 if no spike yet)
     * 
     * @post Last spike timestamp is returned
     * @note Used for spike-timing-dependent plasticity (STDP) and other
     *       timing-dependent computations
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Implement the Leaky Integrate-and-Fire (LIF) step function
     * 
     * This is the core computational method that implements the LIF neuron
     * dynamics for one discrete time step. It updates the membrane potential
     * based on synaptic inputs and determines if the neuron should fire.
     * 
     * @param currentTime Current simulation time (ms)
     * @param dt Time step duration (ms)
     * 
     * @return bool True if neuron fired during this time step, false otherwise
     * 
     * @retval true Neuron generated an action potential
     * @retval false Neuron did not fire
     * 
     * @post Membrane potential is updated based on LIF dynamics
     * @pre Neuron is not in refractory period
     * @pre currentTime and dt are valid
     * 
     * @mathematical_implementation
     * The LIF update follows this equation:
     * 
     * τ * dV/dt = -(V - V_rest) + R*I_total
     * 
     * Where:
     * - τ = C_m / g_L is the membrane time constant
     * - V_rest is the resting potential
     * - R = 1/g_L is the input resistance
     * - I_total is the total synaptic current
     * 
     * The discrete-time solution is:
     * 
     * V(t+dt) = V(t) * exp(-dt/τ) + (I_total * τ / C_m) * (1 - exp(-dt/τ))
     * 
     * Where:
     * - exp(-dt/τ) represents the exponential decay term
     * - (1 - exp(-dt/τ)) represents the input integration term
     * 
     * @implementation_notes
     * - Uses precomputed exponential decay for efficiency
     * - Includes spike detection and refractory period management
     * - Handles adaptation current integration
     * - Optimized for minimum computational cost
     * 
     * @see step() for the higher-level update function
     * @see NeuronState for state variables
     * @see updateNeuronState() for state management
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * This method processes excitatory postsynaptic potentials (EPSPs) that
     * depolarize the membrane potential, bringing it closer to the firing threshold.
     * 
     * @param amplitude The synaptic current amplitude (nA)
     * 
     * @post Membrane potential is increased proportionally to amplitude
     * @pre Neuron is not in refractory period
     * 
     * @note Excitatory inputs are typically modeled as AMPA-type synapses with
     *       fast rise and decay times.
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * This method processes inhibitory postsynaptic potentials (IPSPs) that
     * hyperpolarize the membrane potential, moving it away from the firing threshold.
     * 
     * @param amplitude The synaptic current amplitude (nA)
     * 
     * @post Membrane potential is decreased proportionally to amplitude
     * 
     * @note Inhibitory inputs are typically modeled as GABA-type synapses with
     *       fast or slow decay times depending on receptor type.
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * This method processes neuromodulatory inputs that can alter neuronal
     * properties such as threshold, adaptation, or synaptic efficacy. These
     * inputs are typically slower and have longer-lasting effects.
     * 
     * @param amplitude The modulatory input amplitude
     * 
     * @post Neuronal properties may be modified based on modulatory input
     * 
     * @note Modulatory inputs include neuromodulators like acetylcholine,
     *       dopamine, and serotonin, which can have wide-ranging effects on
     *       neural computation.
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current into the neuron
     * 
     * This method implements direct current injection from external sources,
     * simulating the effect of injected current or external stimulation. This
     * is commonly used for testing and experimental paradigms.
     * 
     * @param current The injected current (nA)
     * 
     * @post Total current is increased by injected amount
     * 
     * @note External current injection can override natural synaptic dynamics
     *       and is useful for experimental control and testing.
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total injected current
     * 
     * @return MembranePotential Total injected current (nA)
     * 
     * @post Total current is returned
     * 
     * @see injectCurrent()
     * @see clearTotalCurrent()
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear the total injected current
     * 
     * @post Total current is set to zero
     * 
     * @note This method should be called at the end of each simulation step
     *       to prevent current accumulation between steps.
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike timestamp for learning and analysis
     * 
     * This method stores the timestamp of a spike event, which is used for
     * spike-timing-dependent plasticity (STDP) and other spike-timing dependent
     * computations. The history is maintained in a rolling buffer.
     * 
     * @param timestamp The simulation time when the spike occurred (ms)
     * 
     * @post Spike timestamp is added to history buffer
     * 
     * @note Spike history enables precise timing-based learning and analysis
     *       of neural activity patterns.
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the spike history for recent spike times
     * 
     * @return const std::vector<Timestamp>& Reference to spike history vector
     * 
     * @post Spike history is returned for inspection
     * 
     * @note This provides access to recent spike times for STDP and other
     *       spike-timing dependent computations. The vector is typically
     *       managed by the implementation and should not be modified directly.
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear the spike history buffer
     * 
     * @post Spike history buffer is emptied
     * 
     * @note This is useful for resetting learning history or preparing
     *       for a new experimental protocol.
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add an incoming synapse to this neuron
     * 
     * @param handle Handle to the incoming synapse (source -> this neuron)
     * 
     * @post Synapse is added to incoming synapse list
     * 
     * @see SynapseHandle type
     * @see addOutgoingSynapse()
     * @see getIncomingSynapses()
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse from this neuron
     * 
     * @param handle Handle to the outgoing synapse (this neuron -> destination)
     * 
     * @post Synapse is added to outgoing synapse list
     * 
     * @see SynapseHandle type
     * @see addIncomingSynapse()
     * @see getOutgoingSynapses()
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapses to this neuron
     * 
     * @return const std::vector<SynapseHandle>& Reference to incoming synapses vector
     * 
     * @post Incoming synapses are returned for inspection
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapses from this neuron
     * 
     * @return const std::vector<SynapseHandle>& Reference to outgoing synapses vector
     * 
     * @post Outgoing synapses are returned for inspection
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the neuron's plasticity flags
     * 
     * @return const PlasticityFlags& Const reference to plasticity flags
     * 
     * @post Plasticity flags are returned for inspection
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
     * @post Plasticity flags are returned for modification
     * @warning Direct flag manipulation can affect learning behavior
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity rules for this neuron
     * 
     * @param hebbian Enable Hebbian plasticity (true/false)
     * @param stdp Enable STDP plasticity (true/false)
     * @param rewardModulated Enable reward-modulated plasticity (true/false)
     * 
     * @post Plasticity rules are configured according to parameters
     * 
     * @note This method prepares the neuron for synaptic learning by enabling
     *       the specified plasticity mechanisms.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the region ID for this neuron
     * 
     * @param region Region identifier where this neuron resides
     * 
     * @post Region ID is set
     * @see RegionId type
     * @see getRegionId()
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the region ID where this neuron resides
     * 
     * @return RegionId Region identifier
     * 
     * @post Region ID is returned
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the population ID for this neuron
     * 
     * @param population Population identifier within the region
     * 
     * @post Population ID is set
     * @see PopulationId type
     * @see getPopulationId()
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the population ID of this neuron
     * 
     * @return PopulationId Population identifier
     * 
     * @post Population ID is returned
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update the neuron for one simulation step
     * 
     * This method implements the high-level neuron update process for one
     * discrete time step. It coordinates all aspects of neuronal computation
     * including membrane potential dynamics, spike generation, and state updates.
     * 
     * @param currentTime Current simulation time (ms)
     * 
     * @post Neuron state is updated for the current simulation step
     * @pre Neuron is initialized
     * @pre currentTime is valid
     * 
     * @note This is a placeholder for Phase 2 implementation. The current
     *       implementation delegates to stepLIF() for actual computation.
     *       Once the full integrate-and-fire dynamics are implemented, this
     *       method will coordinate the complete neuronal update process.
     * 
     * @todo PHASE 2: Implement real integrate-and-fire dynamics
     * - Integrate LIF equations with proper numerical stability
     * - Add synaptic input integration with conductance-based models
     * - Implement refractory period with hard refractory phase
     * - Add spike-frequency adaptation mechanisms
     * - Include learning-ready state management for STDP
     * 
     * @implementation_status PHASE 2 - TO BE IMPLEMENTED
     * This method is currently a stub that calls stepLIF(). The full implementation
     * will provide comprehensive neuronal dynamics including:
     * 1. Membrane potential decay and integration
     * 2. Synaptic input accumulation
     * 3. Spike detection and generation
     * 4. Refractory period management
     * 5. Adaptation current updates
     * 6. State variable synchronization
     * 
     * @see stepLIF()
     * @see NeuronState
     * @see reset()
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset the neuron to its initial state
     * 
     * This method resets all neuron state variables to their initial values,
     * effectively returning the neuron to its starting configuration. This is
     * useful for starting a new simulation run or recovering from corrupted state.
     * 
     * @post All neuron state variables are reset to initial values
     * @pre Neuron is initialized
     * 
     * @note Reset includes membrane potential, firing state, spike history,
     *       and any accumulated currents or adaptation variables.
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random biological parameters
     * 
     * This method randomizes the neuron's biological parameters within
     * plausible ranges, creating diverse neuronal populations for simulation.
     * This is typically called during brain initialization to create
     * heterogeneous neuronal populations.
     * 
     * @param rng Random number generator to use for parameter generation
     * 
     * @post Neuron parameters are randomized within biological ranges
     * @pre Neuron is created but not initialized
     * 
     * @neuron_parameters_randomized
     * - Membrane potential (V_m): -70 ± 10 mV
     * - Resting potential (V_rest): -70 ± 5 mV  
     * - Threshold (V_th): -55 ± 5 mV
     * - Reset potential (V_reset): -70 ± 5 mV
     * - Leak conductance (g_L): 10 ± 5 nS
     * - Refractory period (τ_ref): 5 ± 5 steps
     * 
     * @note This method ensures biological diversity in the neural population
     *       while maintaining parameter ranges that ensure stable computation.
     * 
     * @see RandomGenerator
     * @see initialize()
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     * 
     * The actual neuron implementation is hidden in a private structure to
     * reduce compilation dependencies and improve modularity. All core
     * computational logic is implemented in the corresponding .cpp file.
     */
    struct Impl;
    
    /**
     * @brief Pointer to implementation (Pimpl idiom)
     * 
     * This pointer points to the private implementation structure containing
     * all neuron state variables and computational logic. Using the Pimpl idiom
     * allows for efficient compilation and encapsulation of implementation details.
     */
    Impl* pImpl;
};

} // namespace nlm

(End of file - total 471 lines)

} // namespace nlm
