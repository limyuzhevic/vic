#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <string>
#include "../core/Error/Error.hpp"

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
 * @class Neuron
 * @brief Represents a single biological neuron implementing the Leaky Integrate-and-Fire (LIF) model
 * @details This class models a biological neuron with realistic membrane dynamics, synaptic inputs, and spike generation.
 * The LIF model simulates how neurons integrate incoming currents and fire action potentials when membrane potential
 * reaches a threshold, making it suitable for biologically-plausible neural network simulations.
 * 
 * The neuron maintains comprehensive state including membrane potential, synaptic conductances, firing history,
 * and plasticity mechanisms. It supports both rate-based and spiking computation models with error recovery
 * and validation mechanisms for robust simulation.
 * 
 * Memory model: Uses pointer-to-implementation (PIMPL) for stable ABI and efficient memory management.
 * Thread safety: Methods are not thread-safe; external synchronization required for concurrent access.
 * 
 * @warning Default parameters and initial values are biologically inspired but may need tuning for specific applications
 * @note This implementation follows the standard LIF formulation: dV/dt = (V_rest - V)/tau + I/C
 */
class Neuron {
public:
    /**
     * @brief Constructor - creates neuron with unique identifier
     * @param id Unique identifier for this neuron (must be valid and non-conflicting)
     * @post Neuron is initialized with default biologically plausible parameters:
     *       - Membrane potential: -70mV (resting)
     *       - Threshold: -55mV
     *       - Refractory period: 5 steps
     *       - Firing state: Resting
     * @warning Constructor may throw if ID validation fails in implementation
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor - cleans up neuron resources
     * @details Ensures proper cleanup of dynamic memory and resets state
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor - deleted to prevent copying and ensure unique neuron identity
     * @details Neurons are identified by their unique ID and must not be duplicated
     * @warning Attempting to copy will result in compilation error
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor - enables efficient neuron transfer
     * @param other Neuron to move from (will be invalidated after move)
     * @post this->id = other->id, other->pImpl = nullptr (source is invalid)
     * @note Move semantics provide efficient transfer of ownership without deep copying
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator - enables efficient neuron reassignment
     * @param other Neuron to move from (will be invalidated after move)
     * @post this->id = other->id, other->pImpl = nullptr (source is invalid)
     * @return Reference to this neuron for chaining operations
     * @note Move assignment provides efficient transfer of ownership without deep copying
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * @return Neuron's unique identifier (guaranteed unique within the brain)
     * @post None
     * @note Neuron ID is immutable after creation and cannot be changed
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron type classification
     * @return Neuron type indicating functional role in the network
     * @post None
     * @note Common types include: Internal, Sensory, Motor, Excitatory, Inhibitory, Modulatory
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type classification
     * @param type New neuron type to assign
     * @post Neuron type changes and plasticity flags may be updated accordingly
     * @warning Type changes may affect synaptic dynamics and network behavior
     * @throws NLMException if type validation fails in implementation
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the complete neuron state structure
     * @return Constant reference to neuron state (contains all internal variables)
     * @post State remains unchanged
     * @note Thread-safe access to state when used with getState()
     * @warning State should only be modified through dedicated setter methods
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable access to neuron state structure
     * @return Non-constant reference to neuron state (requires careful management)
     * @post State may be modified by caller
     * @warning Direct state manipulation bypasses validation - use with caution
     * @note Use setState() or individual setters instead when possible
     */
    NeuronState& getState();
    
    /**
     * @brief Get current membrane potential (transmembrane voltage)
     * @return Membrane potential in millivolts (mV)
     * @post Value remains unchanged
     * @note Typical range: -100mV to 50mV. Resting potential ~ -70mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential to specific value
     * @param potential New membrane potential in mV
     * @post Membrane potential updated; health status invalid
     * @warning Bypasses biological dynamics - use with care
     * @throws NLMException if potential validation fails
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Increment membrane potential by delta value
     * @param delta Voltage change in mV (positive for depolarization, negative for hyperpolarization)
     * @post Membrane potential updated by delta; health status invalid
     * @note Biological and computational sources of membrane potential change
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get firing threshold potential
     * @return Threshold potential in mV (typical: -55mV)
     * @post Value remains unchanged
     * @note When membrane potential >= threshold, neuron fires action potential
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set firing threshold potential
     * @param threshold New threshold potential in mV
     * @post Threshold updated; health status invalid
     * @warning Threshold changes affect excitability and firing dynamics
     * @throws NLMException if threshold validation fails (should be above resting potential)
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing action potentials
     * @return true if neuron is in firing state, false otherwise
     * @post None
     * @note Returns true immediately after spike initiation until refractory period ends
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period (unable to fire)
     * @return true if neuron cannot fire due to refractory inhibition
     * @post None
     * @note During refractory period, membrane potential is clamped and neuron cannot spike
     */
    bool isRefractory() const;
    
    /**
     * @brief Set neuron firing state directly
     * @param state Desired firing state (Resting, Active, Refractory, Inhibited)
     * @post Firing state updated; health status invalid
     * @warning Direct state manipulation bypasses physiological dynamics
     * @throws NLMException if state validation fails
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period duration
     * @param steps Number of simulation steps the neuron should remain refractory
     * @post Refractory period duration updated; remaining steps reset if needed
     * @note Default: 5 steps (biological refractory period ~ 5ms)
     * @warning Long refractory periods reduce firing rate but improve stability
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter (called each simulation step)
     * @post Refractory counter decremented; may exit refractory state
     * @note Automatically called during neuron step processing
     */
    void decrementRefractory();
    
    /**
     * @brief Get current firing rate (spikes per second)
     * @return Firing rate in Hz (spikes per second)
     * @post Value remains unchanged
     * @note Rate-based computation model; distinct from spiking state for efficiency
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set firing rate directly (rate-based computation)
     * @param rate Firing rate in Hz
     * @post Firing rate updated; health status invalid
     * @warning Direct rate setting bypasses spike generation dynamics
     * @throws NLMException if rate validation fails
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set leak conductance for membrane current flow
     * @param conductance Leak conductance in nS (nanoSiemens)
     * @post Conductance updated; health status invalid
     * @warning Affects time constant and membrane potential dynamics
     * @note Controls how quickly membrane potential returns to resting potential
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get leak conductance value
     * @return Leak conductance in nS
     * @post None
     * @note Typical values: 10 nS. Higher values = faster membrane time constant
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get refractory period duration
     * @return Refractory period in simulation steps
     * @post None
     * @note Used for spike timing and recovery management
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set resting potential (baseline membrane potential)
     * @param potential Resting potential in mV
     * @post Resting potential updated; health status invalid
     * @warning Critical for membrane potential dynamics
     * @throws NLMException if potential validation fails
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get resting potential value
     * @return Resting potential in mV
     * @post None
     * @note Typical values: -70mV. Golden standard: -65mV to -75mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set post-spike reset potential
     * @param potential Reset potential in mV
     * @post Reset potential updated; health status invalid
     * @warning Often equals resting potential for stable firing
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if membrane potential has reached firing threshold
     * @return true if neuron should fire (potential >= threshold)
     * @post None
     * @note Used by spike detection logic; doesn't actually cause firing
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get timestamp of last spike
     * @return Last spike timestamp in simulation time steps (-1 if no spikes yet)
     * @post Value remains unchanged
     * @note Used for spike history and plasticity calculations
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Main LIF neuron integration step
     * @param currentTime Current simulation time step
     * @param dt Time step duration in milliseconds
     * @return true if neuron fired during this step, false otherwise
     * @post Updates membrane potential, firing state, and spike history if applicable
     * @note Core computational method implementing Leaky Integrate-and-Fire dynamics
     * @see step() for convenience method with fixed timestep
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Convenience step method (1ms timestep)
     * @param currentTime Current simulation time step
     * @post Updates neuron state using default 1ms timestep
     * @note Wrapper around stepLIF() for simpler usage
     * @see stepLIF() for full parameter control
     */
    void step(Timestamp currentTime);
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
    // Error handling and validation methods
    bool validateState() const;
    bool validateInput(MembranePotential input) const;
    bool validateThreshold() const;
    bool validateMembranePotential() const;
    bool validateRefractoryPeriod() const;
    std::vector<std::string> getValidationIssues() const;
    
    // Error recovery methods
    void attemptRecovery();
    bool isRecoverable() const;
    
    // Health checking
    bool isHealthy() const;
    std::vector<std::string> getHealthIssues() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
