/**
 * @file Neuron.hpp
 * @brief Single neuron implementation with LIF dynamics
 * 
 * This header defines the Neuron class, representing a single biological neuron
 * implementing the Leaky Integrate-and-Fire (LIF) model of neuronal dynamics.
 * Neurons are the fundamental computational units in the NLM framework,
 * capable of receiving inputs, processing them through membrane dynamics,
 * and generating spikes when threshold is reached.
 * 
 * @details Neuron implementation features:
 * - Leaky Integrate-and-Fire (LIF) dynamics with membrane potential integration
 * - Multiple input types (excitatory, inhibitory, modulatory)
 * - Spike detection and refractory period management
 * - Synaptic connectivity management (incoming/outgoing synapses)
 * - Plasticity state management (Hebbian, STDP, reward-modulated)
 * - Type system for different neuron specializations
 * - Region and population membership for network organization
 * - Random parameter initialization for reproducible simulations
 * 
 * @note The LIF model is a simplified but biologically plausible model of
 * neuronal dynamics that balances computational efficiency with biological
 * realism. The model captures essential neuronal behaviors including:
 * - Membrane potential integration
 * - Threshold-based spike generation
 * - Refractory period after spiking
 * - Leak conductance
 * - Synaptic conductances
 * 
 * @author NLM Authors
 * @date Created: 2024
 * @version 1.0
 * 
 * @copyright MIT License
 * 
 * @warning Neuron parameters must be set appropriately for the simulation
 * time step (dt). Incompatible parameters may lead to numerical instability
 * or unrealistic behavior.
 * 
 * @see NeuronState for neuron state representation
 * @see Synapse for synaptic connection representation
 * @see Brain for neuron population management
 * @see RandomGenerator for random parameter initialization
 * @see PlasticityRule for plasticity mechanisms
 * 
 * @ingroup NeuralComponents
 */
#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @struct NeuronState
 * @brief Complete state representation of a single neuron
 * 
 * NeuronState provides a comprehensive snapshot of a neuron's current state,
 * including membrane potentials, firing rates, synaptic inputs, and behavioral
 * flags. This structure is designed for efficient storage and fast access patterns
 * during simulation.
 * 
 * @details The state structure contains:
 * - Membrane potential dynamics (current, resting, threshold, reset)
 * - Synaptic conductances (leak and synaptic)
 * - Firing behavior (rate, state, refractory period)
 * - History and adaptation variables
 * - Plasticity and connectivity metadata
 * 
 * All values use the appropriate membrane potential type (MembranePotential)
 * for consistency and unit compatibility. The structure is designed to be
 * copied efficiently and provide thread-safe read access.
 * 
 * @note The neuron state is designed for vectorization and cache efficiency.
 * Fields are ordered by access frequency and data type for optimal memory layout.
 * 
 * @see Neuron for methods accessing state
 * @see MembranePotential for data type
 * 
 * @warning State values may become NaN if parameters are set incorrectly.
 * Always validate parameters before simulation.
 */
struct NeuronState {
    MembranePotential membranePotential;    ///< Current membrane potential (mV)
    MembranePotential restingPotential;    ///< Resting potential (mV)
    MembranePotential threshold;           ///< Firing threshold (mV)
    MembranePotential resetPotential;       ///< Post-spike reset value (mV)
    MembranePotential leakConductance;      ///< Leak conductance (nS)
    MembranePotential synapseConductance;   ///< Synaptic conductance (nS)
    FiringRate firingRate;                 ///< Current firing rate (Hz)
    FiringState firingState;               ///< Current firing state
    uint32_t refractoryRemaining;          ///< Steps remaining in refractory period
    uint32_t refractoryPeriod;             ///< Total refractory period (steps)
    float adaptationVariable;              ///< For spike-frequency adaptation
    float lastSpikeTime;                   ///< Timestamp of last spike (-1 if none)
    
    /**
     * @brief Default constructor initializes neuron to resting state
     * 
     * Sets the neuron to a biologically realistic resting state with:
     * - Membrane potential at -70 mV (typical for cortical neurons)
     * - Resting potential matching membrane potential
     * - Threshold at -55 mV (standard for LIF neurons)
     * - Reset potential at resting level
     * - Leak conductance at 10 nS (typical value)
     * - No synaptic input
     * - Zero firing rate
     * - Resting firing state
     * - No refractory period
     * - No adaptation
     * - No spike history
     * 
     * @note This represents a healthy, unstimulated neuron ready for simulation.
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
 * @brief Single neuron implementing Leaky Integrate-and-Fire dynamics
 * 
 * Neuron represents a single computational unit in the NLM framework, implementing
 * the Leaky Integrate-and-Fire (LIF) model of neuronal dynamics. Neurons receive
 * synaptic inputs, integrate membrane potential, and generate spikes when the
 * threshold is reached.
 * 
 * @details Neuron capabilities:
 * - **Input processing**: Receives excitatory, inhibitory, and modulatory inputs
 * - **Dynamics simulation**: Implements LIF equations for membrane potential
 * - **Spike generation**: Produces spikes when threshold is crossed
 * - **Refractory management**: Enforces refractory periods after spiking
 * - **Synaptic connectivity**: Manages incoming and outgoing synaptic connections
 * - **Plasticity integration**: Supports Hebbian, STDP, and reward-modulated plasticity
 * - **Network integration**: Supports region and population membership
 * - **State management**: Maintains complete neuron state for simulation
 * - **Random initialization**: Provides reproducible random parameter setup
 * 
 * **LIF Dynamics**:
 * The neuron implements the classic LIF equations:
 * - dV/dt = (-g_leak * (V - V_rest) + I_syn + I_inj) / C
 * - When V >= V_threshold, neuron fires and V is reset
 * - During refractory period, V is clamped to V_reset
 * 
 * **Input Integration**:
 * Neurons accumulate inputs from:
 * - Synaptic inputs (excitatory/inhibitory/modulatory)
 * - External current injection
 * - Background noise (optional)
 * 
 * **Usage Example**:
 * @code
 * nlm::Neuron neuron(0); // Create neuron with ID 0
 * neuron.setThreshold(-50.0f);
 * neuron.setLeakConductance(12.0f);
 * neuron.initializeRandom(rng); // Set random parameters
 * 
 * // Simulation loop
 * while (simulation_steps-- > 0) {
 *     neuron.step(current_time);
 *     // Process neuron output if firing
 * }
 * @endcode
 * 
 * **Network Integration**:
 * Neurons are organized into regions and populations for scalable network
 * construction. Each neuron maintains references to its connected synapses
 * and can participate in plasticity learning rules.
 * 
 * @see NeuronState for detailed state representation
 * @see Synapse for synaptic connections
 * @see Brain for population management
 * @see RandomGenerator for random initialization
 * @see PlasticityRule for learning mechanisms
 * @see NLMException for error conditions
 * 
 * @warning Neuron parameters must be consistent with simulation time step
 * (dt). Incompatible parameters may cause numerical instability or unrealistic
 * behavior. Always validate parameters before use.
 * 
 * @note Neurons use the pImpl idiom for efficient memory management and
 * forward declaration of the implementation class.
 * 
 * @ingroup NeuralComponents
 */
class Neuron {
public:
    /**
     * @brief Construct a neuron with specified identifier
     * 
     * Creates a new neuron with a unique identifier. The neuron is initialized
     * to a resting state with default parameters. Additional parameters must be
     * set before use in simulation.
     * 
     * @param id Unique identifier for this neuron
     * 
     * @see setType() for setting neuron type
     * @see initializeRandom() for random parameter initialization
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42); // Create neuron with ID 42
     * neuron.setType(nlm::NeuronType::Excitatory);
     * neuron.initializeRandom(rng);
     * @endcode
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor
     * 
     * Cleans up neuron resources and performs any necessary cleanup operations.
     * Memory management is handled via the pImpl pattern.
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor is deleted (use move semantics instead)
     * 
     * Neurons cannot be copied due to their complex internal state and
     * implementation details. Use move semantics for efficient transfer.
     * 
     * @param other Source neuron to copy (deleted)
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted
     * 
     * Neurons cannot be assigned due to their complex internal state and
     * implementation details. Use move assignment for efficient transfer.
     * 
     * @param other Source neuron to assign (deleted)
     * @return Neuron& Reference to assigned neuron (never returned)
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor (efficient)
     * 
     * Transfers ownership of neuron implementation from source to new neuron
     * without copying complex internal state. This provides efficient neuron
     * transfer and reassignment.
     * 
     * @param other Source neuron to move from
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator (efficient)
     * 
     * Transfers ownership of neuron implementation from source to existing neuron
     * without copying complex internal state. This provides efficient neuron
     * reassignment and transfer.
     * 
     * @param other Source neuron to move from
     * @return Neuron& Reference to this neuron
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the neuron's unique identifier
     * 
     * @return NeuronId Unique identifier for this neuron
     * 
     * @see setRegionId() for setting region membership
     * @see setPopulationId() for setting population membership
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(123);
     * NeuronId id = neuron.getId();
     * std::cout << "Neuron ID: " << id << std::endl;
     * @endcode
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's type
     * 
     * @return NeuronType Neuron type (Excitatory, Inhibitory, or Custom)
     * 
     * @see setType() for setting neuron type
     * @see NeuronType for type enumeration
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * nlm::NeuronType type = neuron.getType();
     * if (type == nlm::NeuronType::Excitatory) {
     *     std::cout << "Excitatory neuron" << std::endl;
     * }
     * @endcode
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's type
     * 
     * Configures the neuron's functional type, which may affect its behavior
     * and interaction with plasticity rules. Common types include Excitatory,
     * Inhibitory, and Custom types.
     * 
     * @param type Neuron type to set
     * 
     * @see getType() for retrieving neuron type
     * @see NeuronType for available types
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * neuron.setType(nlm::NeuronType::Inhibitory);
     * @endcode
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the neuron's complete state
     * 
     * Returns a constant reference to the neuron's current state. This provides
     * read-only access to all neuron properties including membrane potentials,
     * firing state, and connectivity information.
     * 
     * @return const NeuronState& Constant reference to neuron state
     * 
     * @note This is thread-safe for read-only access. For modifications,
     * use the mutable state accessor or individual setter methods.
     * 
     * @see getState() for mutable access
     * @see NeuronState for state structure
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * const nlm::NeuronState& state = neuron.getState();
     * std::cout << "Membrane potential: " << state.membranePotential << " mV" << std::endl;
     * std::cout << "Firing state: " << static_cast<int>(state.firingState) << std::endl;
     * @endcode
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to neuron's state
     * 
     * Returns a mutable reference to the neuron's current state. This allows
     * direct modification of neuron state for advanced use cases and optimization.
     * Use with caution as improper state modification may cause simulation errors.
     * 
     * @return NeuronState& Mutable reference to neuron state
     * 
     * @warning Modifying state directly may bypass important validation and
     * consistency checks. Prefer using individual setter methods for normal
     * operations. Direct state access is useful for performance-critical code
     * and custom simulation logic.
     * 
     * @see getState() for constant access
     * @see NeuronState for state structure
     
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * NeuronState& state = neuron.getState();
     * // Fast modification for performance-critical code
     * state.membranePotential = -65.0f;
     * state.firingState = nlm::FiringState::Firing;
     * @endcode
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * 
     * @return MembranePotential Current membrane potential (mV)
     * 
     * @see setMembranePotential() for setting membrane potential
     * @see addToMembranePotential() for incremental changes
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * double v = neuron.getMembranePotential();
     * std::cout << "V_m = " << v << " mV" << std::endl;
     * @endcode
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential
     * 
     * @param potential New membrane potential (mV)
     * 
     * @see getMembranePotential() for retrieving membrane potential
     * @see addToMembranePotential() for incremental changes
     * 
     * @warning Setting membrane potential directly may cause numerical
     * instability if done frequently. Use incremental updates for smooth
     * dynamics.
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * neuron.setMembranePotential(-60.0f);
     * @endcode
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add delta to membrane potential
     * 
     * Performs incremental update to membrane potential, useful for adding
     * synaptic inputs or current injections. This is more numerically stable
     * than setting potential directly.
     * 
     * @param delta Change in membrane potential (mV)
     * 
     * @see getMembranePotential() for retrieving membrane potential
     * @see setMembranePotential() for setting absolute value
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * // Add excitatory synaptic input
     * neuron.addToMembranePotential(2.5f);
     * // Add inhibitory input
     * neuron.addToMembranePotential(-1.0f);
     * @endcode
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold
     * 
     * @return MembranePotential Current firing threshold (mV)
     * 
     * @see setThreshold() for setting threshold
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * double threshold = neuron.getThreshold();
     * std::cout << "Threshold = " << threshold << " mV" << std::endl;
     * @endcode
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold
     * 
     * @param threshold New firing threshold (mV)
     * 
     * @see getThreshold() for retrieving threshold
     * 
     * @warning Threshold must be greater than resting potential and less
     * than reset potential for proper neuron operation.
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * neuron.setThreshold(-45.0f);
     * @endcode
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * 
     * @return bool true if neuron is in firing state, false otherwise
     * 
     * @see isRefractory() for checking refractory state
     * @see setFiringState() for setting firing state
     * @see stepLIF() for spike detection during simulation
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return bool true if neuron is in refractory period, false otherwise
     * 
     * @see isFiring() for checking firing state
     * @see setRefractoryPeriod() for setting refractory period
     * @see decrementRefractory() for advancing refractory time
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the neuron's firing state
     * 
     * @param state New firing state (Resting, Firing, or Refractory)
     * 
     * @see getFiringState() for retrieving firing state
     * @see isFiring() for checking firing state
     * @see isRefractory() for checking refractory state
     * 
     * @warning Changing firing state manually may cause simulation
     * inconsistencies. Use stepLIF() for automatic state transitions.
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * neuron.setFiringState(nlm::FiringState::Firing);
     * @endcode
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period duration
     * 
     * @param steps Number of simulation steps for refractory period
     * 
     * @see getRefractoryPeriod() for retrieving refractory period
     * @see isRefractory() for checking refractory state
     * @see decrementRefractory() for advancing refractory time
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory period counter
     * 
     * Advances the neuron's refractory period counter by one step. If the
     * counter reaches zero, the neuron exits the refractory state.
     * 
     * @see isRefractory() for checking refractory state
     * @see setRefractoryPeriod() for setting refractory period
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * // Simulate refractory period
     * while (neuron.isRefractory()) {
     *     neuron.decrementRefractory();
     * }
     * @endcode
     */
    void decrementRefractory();
    
    /**
     * @brief Get the neuron's firing rate
     * 
     * @return FiringRate Current firing rate (Hz)
     * 
     * @see setFiringRate() for setting firing rate
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * double rate = neuron.getFiringRate();
     * std::cout << "Firing rate: " << rate << " Hz" << std::endl;
     * @endcode
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the neuron's firing rate
     * 
     * @param rate New firing rate (Hz)
     * 
     * @see getFiringRate() for retrieving firing rate
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * neuron.setFiringRate(50.0f);
     * @endcode
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance
     * 
     * @param conductance Leak conductance (nS)
     * 
     * @see getLeakConductance() for retrieving leak conductance
     * 
     * @warning Leak conductance affects membrane time constant and
     * stability. Typical values range from 1-100 nS.
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * neuron.setLeakConductance(15.0f);
     * @endcode
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the leak conductance
     * 
     * @return MembranePotential Leak conductance (nS)
     * 
     * @see setLeakConductance() for setting leak conductance
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period duration
     * 
     * @return uint32_t Refractory period in simulation steps
     * 
     * @see setRefractoryPeriod() for setting refractory period
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential
     * 
     * @param potential Resting potential (mV)
     * 
     * @see getRestingPotential() for retrieving resting potential
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential
     * 
     * @return MembranePotential Resting potential (mV)
     * 
     * @see setRestingPotential() for setting resting potential
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential
     * 
     * @param potential Reset potential (mV)
     * 
     * @see getResetPotential() for retrieving reset potential
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get the reset potential
     * 
     * @return MembranePotential Reset potential (mV)
     * 
     * @see setResetPotential() for setting reset potential
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Check if neuron has reached firing threshold
     * 
     * @return bool true if membrane potential >= threshold, false otherwise
     * 
     * @see stepLIF() for spike detection and state transition
     * @see setThreshold() for setting threshold value
     * @see getMembranePotential() for current membrane potential
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * if (neuron.checkThreshold()) {
     *     std::cout << "Neuron ready to fire!" << std::endl;
     * }
     * @endcode
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of last spike
     * 
     * @return float Timestamp of last spike (-1.0f if no spikes yet)
     * 
     * @see recordSpike() for recording spike events
     * @see clearSpikeHistory() for clearing history
     * @see getSpikeHistory() for retrieving all spike times
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform one step of LIF neuron dynamics
     * 
     * Implements the Leaky Integrate-and-Fire model for a single simulation
     * step. This is the core neuron computation that:
     * 1. Updates membrane potential based on currents
     * 2. Checks for threshold crossing and spike generation
     * 3. Manages refractory period
     * 4. Applies reset after spiking
     * 
     * @param currentTime Current simulation time (ms)
     * @param dt Time step duration (ms)
     * @return bool true if neuron fired during this step, false otherwise
     * 
     * @note This method implements the core LIF dynamics:
     * - dV/dt = (-g_leak*(V-V_rest) + I_syn + I_inj) / C
     * - When V >= V_threshold, neuron fires
     * - During refractory period, V is clamped to V_reset
     * - Spike resets V to V_reset and sets refractory counter
     * 
     * @see getMembranePotential() for current membrane potential
     * @see isFiring() for firing state
     * @see isRefractory() for refractory state
     * @see recordSpike() for spike recording
     * @see receiveExcitatoryInput() for adding excitatory inputs
     * @see receiveInhibitoryInput() for adding inhibitory inputs
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * neuron.setThreshold(-50.0f);
     * neuron.setLeakConductance(10.0f);
     * 
     * double currentTime = 0.0;
     * double dt = 0.1; // 0.1 ms time step
     * 
     * for (int step = 0; step < 1000; ++step) {
     *     neuron.injectCurrent(0.5); // External current
     *     neuron.receiveExcitatoryInput(2.0); // Synaptic input
     *     
     *     if (neuron.stepLIF(currentTime, dt)) {
     *         std::cout << "Spike at time " << currentTime << " ms" << std::endl;
     *         currentTime += dt;
     *     }
     * }
     * @endcode
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * Adds excitatory postsynaptic potential (EPSP) to membrane potential.
     * Excitatory inputs typically increase membrane potential toward threshold.
     * 
     * @param amplitude EPSP amplitude (mV)
     * 
     * @see receiveInhibitoryInput() for inhibitory inputs
     * @see receiveModulatoryInput() for modulatory inputs
     * @see stepLIF() for synaptic integration
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * neuron.receiveExcitatoryInput(2.5f); // Strong excitatory input
     * neuron.receiveExcitatoryInput(0.5f); // Weak excitatory input
     * @endcode
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * Adds inhibitory postsynaptic potential (IPSP) to membrane potential.
     * Inhibitory inputs typically decrease membrane potential away from threshold.
     * 
     * @param amplitude IPSP amplitude (mV, positive value)
     * 
     * @see receiveExcitatoryInput() for excitatory inputs
     * @see receiveModulatoryInput() for modulatory inputs
     * @see stepLIF() for synaptic integration
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * neuron.receiveInhibitoryInput(1.5f); // Inhibitory input
     * @endcode
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * Adds modulatory postsynaptic potential (mEPSP) to membrane potential.
     * Modulatory inputs typically have weaker effects but can influence
     * neuronal excitability and plasticity.
     * 
     * @param amplitude mEPSP amplitude (mV)
     * 
     * @see receiveExcitatoryInput() for excitatory inputs
     * @see receiveInhibitoryInput() for inhibitory inputs
     * @see stepLIF() for synaptic integration
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current
     * 
     * Adds external current to the neuron's input current for the next step.
     * External current can represent stimuli from sensory inputs or other
     * external sources.
     * 
     * @param current External current amplitude (nA)
     * 
     * @see getTotalCurrent() for retrieving total current
     * @see clearTotalCurrent() for clearing current
     * @see stepLIF() for current integration
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * neuron.injectCurrent(0.1); // 0.1 nA external current
     * // Current will be integrated in next stepLIF() call
     * @endcode
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total current for this step
     * 
     * @return MembranePotential Total current (nA) for current step
     * 
     * @see injectCurrent() for adding external current
     * @see clearTotalCurrent() for clearing current
     * @see stepLIF() for current usage in dynamics
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear the total current
     * 
     * Resets the external current accumulator to zero. This is typically
     * called at the beginning of each simulation step to ensure currents
     * are properly integrated for each time step.
     * 
     * @see injectCurrent() for adding external current
     * @see getTotalCurrent() for retrieving current
     * @see stepLIF() for current usage
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike event
     * 
     * Records the current time as a spike event in the neuron's history.
     * Spike history is used for STDP (Spike-Timing-Dependent Plasticity) and
     * other timing-dependent plasticity mechanisms.
     * 
     * @param timestamp Time of spike event (ms)
     * 
     * @see getLastSpikeTime() for retrieving last spike time
     * @see getSpikeHistory() for retrieving all spike times
     * @see clearSpikeHistory() for clearing spike history
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * neuron.recordSpike(current_time);
     * @endcode
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the neuron's spike history
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps (ms)
     * 
     * @see recordSpike() for recording spike events
     * @see clearSpikeHistory() for clearing history
     * @see getLastSpikeTime() for last spike only
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear the spike history
     * 
     * Removes all recorded spike events from the neuron's history. This may
     * be useful for resetting the neuron or changing simulation modes.
     * 
     * @see recordSpike() for recording spike events
     * @see getSpikeHistory() for retrieving spike history
     * @see getLastSpikeTime() for last spike time
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add an incoming synapse to this neuron
     * 
     * Registers a synapse as an input to this neuron (post-synaptic). The
     * synapse will be used for synaptic input during neuron simulation.
     * 
     * @param handle Synapse handle (identifier)
     * 
     * @see addOutgoingSynapse() for adding outgoing synapses
     * @see getIncomingSynapses() for retrieving incoming synapses
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse from this neuron
     * 
     * Registers a synapse as an output from this neuron (pre-synaptic). The
     * synapse will be used for spike transmission during simulation.
     * 
     * @param handle Synapse handle (identifier)
     * 
     * @see addIncomingSynapse() for adding incoming synapses
     * @see getOutgoingSynapses() for retrieving outgoing synapses
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get the list of incoming synapses
     * 
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     * 
     * @see addIncomingSynapse() for adding incoming synapses
     * @see getOutgoingSynapses() for retrieving outgoing synapses
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get the list of outgoing synapses
     * 
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     * 
     * @see addOutgoingSynapse() for adding outgoing synapses
     * @see getIncomingSynapses() for retrieving incoming synapses
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the neuron's plasticity flags
     * 
     * @return const PlasticityFlags& Constant reference to plasticity flags
     * 
     * @see getPlasticityFlags() for mutable access
     * @see enablePlasticity() for enabling plasticity
     * @see PlasticityFlags for flag structure
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to neuron's plasticity flags
     * 
     * @return PlasticityFlags& Mutable reference to plasticity flags
     * 
     * @see getPlasticityFlags() for constant access
     * @see enablePlasticity() for enabling plasticity
     * @see PlasticityFlags for flag structure
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity mechanisms
     * 
     * Configures which plasticity mechanisms are active for this neuron.
     * Plasticity allows synapses to strengthen or weaken based on activity.
     * 
     * @param hebbian Enable Hebbian plasticity (true) or disable (false)
     * @param stdp Enable STDP (true) or disable (false)
     * @param rewardModulated Enable reward-modulated plasticity (true) or disable (false)
     * 
     * @see getPlasticityFlags() for retrieving plasticity flags
     * @see PlasticityFlags for detailed flag structure
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the neuron's region identifier
     * 
     * @param region Region identifier for this neuron
     * 
     * @see getRegionId() for retrieving region identifier
     * @see setPopulationId() for setting population identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the neuron's region identifier
     * 
     * @return RegionId Region identifier for this neuron
     * 
     * @see setRegionId() for setting region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the neuron's population identifier
     * 
     * @param population Population identifier for this neuron
     * 
     * @see getPopulationId() for retrieving population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the neuron's population identifier
     * 
     * @return PopulationId Population identifier for this neuron
     * 
     * @see setPopulationId() for setting population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step
     * 
     * Performs one complete simulation step for the neuron, including:
     * 1. Processing incoming synaptic inputs
     * 2. Updating membrane potential dynamics
     * 3. Checking for spike generation
     * 4. Managing refractory period
     * 5. Recording spike events
     * 
     * @param currentTime Current simulation time (ms)
     * 
     * @note This is the main simulation method. It integrates all neuron
     * components and should be called at each simulation time step.
     * 
     * @see stepLIF() for LIF-specific dynamics
     * @see receiveExcitatoryInput() for excitatory input processing
     * @see receiveInhibitoryInput() for inhibitory input processing
     * @see recordSpike() for spike recording
     * @see isFiring() for firing state
     * @see isRefractory() for refractory state
     * 
     * @warning Current TODO comment indicates this method needs implementation.
     * It currently serves as a placeholder for future development.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     * 
     * Resets all neuron state variables to their initial values:
     * - Membrane potential to resting potential
     * - Firing rate to zero
     * - Firing state to resting
     * - Refractory counter to zero
     * - Spike history to empty
     * - Adaptation variable to zero
     * - Total current to zero
     * 
     * This method is useful for:
     * - Resetting neuron state between trials
     * - Initializing neuron for new simulation
     * - Clearing effects of previous simulation
     * 
     * @see getState() for current state
     * @see resetPotential for reset target
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random parameters
     * 
     * Sets neuron parameters to random values using the provided random number
     * generator. This ensures reproducible neuron initialization and helps
     * explore parameter space.
     * 
     * @param rng Reference to random number generator
     * 
     * @see RandomGenerator for random number generation
     * @see getId() for neuron identification
     * 
     * @par Example
     * @code
     * nlm::Neuron neuron(42);
     * nlm::RandomGenerator rng(12345); // Fixed seed for reproducibility
     * neuron.initializeRandom(rng);
     * 
     * // Verify initialization
     * std::cout << "Neuron initialized with ID: " << neuron.getId() << std::endl;
     * std::cout << "Threshold: " << neuron.getThreshold() << " mV" << std::endl;
     * @endcode
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
