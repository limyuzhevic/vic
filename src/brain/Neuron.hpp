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

// ============================================================================
// Neuron: Single Leaky Integrate-and-Fire (LIF) Neuron
class Neuron {
public:
    // ============================================================================
    // Lifecycle Management
    // ============================================================================

    /**
     * @brief Construct a neuron with unique identifier
     * 
     * Creates a neuron with the specified ID. The neuron is initialized with
     * biological-like default parameters and set to resting state.
     * 
     * @param id Unique identifier for this neuron (used for debugging and tracking)
     * @throws std::bad_alloc if memory allocation for implementation fails
     */
    explicit Neuron(NeuronId id);

    /**
     * @brief Destroy neuron and clean up resources
     */
    ~Neuron();

    // Disable copying, enable moving (Rule of Five compliance)
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;

    // ============================================================================
    // Neuron Identity and Classification
    // ============================================================================

    /**
     * @brief Get unique identifier for this neuron
     * 
     * @return NeuronId Unique identifier
     */
    NeuronId getId() const;

    /**
     * @brief Get neuron type classification
     * 
     * @return NeuronType Type of neuron (Excitatory, Inhibitory, Sensory, etc.)
     * 
     * @note Type affects synaptic behavior and plasticity eligibility
     */
    NeuronType getType() const;

    /**
     * @brief Set neuron type classification
     * 
     * @param type New neuron type
     * 
     * @note Type affects synaptic properties and learning rules
     */
    void setType(NeuronType type);

    // ============================================================================
    // State Access and Inspection
    // ============================================================================

    /**
     * @brief Get complete neuron state
     * 
     * @return const NeuronState& Constant reference to neuron state (for inspection)
     * 
     * @note State contains membrane potential, firing state, refractory period, etc.
     * @note Expensive to copy, use references for efficient access
     */
    const NeuronState& getState() const;

    /**
     * @brief Get mutable neuron state
     * 
     * @return NeuronState& Mutable reference to neuron state (for modification)
     * 
     * @warning Direct state modification bypasses biological constraints
     * @note Use provided methods instead when possible
     */
    NeuronState& getState();

    // ============================================================================
    // Membrane Potential Dynamics
    // ============================================================================

    /**
     * @brief Get current membrane potential
     * 
     * @return MembranePotential Membrane potential in millivolts (mV)
     * 
     * @note Normal range: -80mV (hyperpolarized) to +40mV (depolarized)
     * @note Threshold typically around -55mV for spike initiation
     */
    MembranePotential getMembranePotential() const;

    /**
     * @brief Set absolute membrane potential
     * 
     * @param potential Membrane potential in mV
     * 
     * @warning Direct setting may cause unbiological state changes
     * @note Prefer addToMembranePotential() for synaptic input
     */
    void setMembranePotential(MembranePotential potential);

    /**
     * @brief Add synaptic current to membrane potential
     * 
     * @param delta Change in membrane potential (mV) from synaptic input
     * 
     * @note Used for synaptic integration
     * @note Current is scaled by membrane capacitance
     */
    void addToMembranePotential(MembranePotential delta);

    // ============================================================================
    // Threshold and Firing Properties
    // ============================================================================

    /**
     * @brief Get firing threshold potential
     * 
     * @return MembranePotential Threshold potential in mV
     * @note Typical value: -55mV (neuron fires when crossed)
     */
    MembranePotential getThreshold() const;

    /**
     * @brief Set firing threshold
     * 
     * @param threshold New threshold potential in mV
     * @note Lower threshold = more excitable neuron
     */
    void setThreshold(MembranePotential threshold);

    // ============================================================================
    // Firing State Management
    // ============================================================================

    /**
     * @brief Check if neuron is currently in firing state
     * 
     * @return bool true if neuron has fired and is in spike response phase
     */
    bool isFiring() const;

    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return bool true if neuron cannot fire due to recent spike
     */
    bool isRefractory() const;

    /**
     * @brief Set firing state of neuron
     * 
     * @param state New firing state (Resting, Active, Refractory, etc.)
     * 
     * @warning Direct state modification may cause unbiological behavior
     * @note Prefer simulation-driven state changes via stepLIF()
     */
    void setFiringState(FiringState state);

    /**
     * @brief Set refractory period duration
     * 
     * @param steps Number of simulation steps neuron must wait before firing again
     */
    void setRefractoryPeriod(uint32_t steps);

    /**
     * @brief Decrease remaining refractory time by one step
     * 
     * @note Called automatically during simulation stepLIF()
     */
    void decrementRefractory();

    // ============================================================================
    // Firing Rate and Adaptation
    // ============================================================================

    /**
     * @brief Get current firing rate (approximate)
     * 
     * @return FiringRate Firing rate in Hz (spikes per second)
     */
    FiringRate getFiringRate() const;

    /**
     * @brief Set firing rate (rate-based control)
     * 
     * @param rate Desired firing rate in Hz
     * @note Used for rate-based neuron models
     */
    void setFiringRate(FiringRate rate);

    // ============================================================================
    // LIF Neuron Parameters
    // ============================================================================

    /**
     * @brief Set leak conductance (membrane time constant factor)
     * 
     * @param conductance Leak conductance in nS
     * @note Higher conductance = faster membrane potential decay
     */
    void setLeakConductance(MembranePotential conductance);

    /**
     * @brief Get leak conductance
     * 
     * @return MembranePotential Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;

    /**
     * @brief Get refractory period duration
     * 
     * @return uint32_t Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;

    /**
     * @brief Set resting membrane potential
     * 
     * @param potential Resting potential in mV
     * @note Typical value: -70mV for neurons
     */
    void setRestingPotential(MembranePotential potential);

    /**
     * @brief Get resting membrane potential
     * 
     * @return MembranePotential Resting potential in mV
     */
    MembranePotential getRestingPotential() const;

    /**
     * @brief Set reset potential after spike
     * 
     * @param potential Reset potential in mV
     * @note Typically close to resting potential
     */
    void setResetPotential(MembranePotential potential);

    // ============================================================================
    // Spike Detection and Generation
    // ============================================================================

    /**
     * @brief Check if membrane potential exceeds firing threshold
     * 
     * @return bool true if neuron should fire this timestep
     */
    bool checkThreshold() const;

    /**
     * @brief Get timestamp of last spike
     * 
     * @return float Timestamp of last spike in seconds (-1.0f if never spiked)
     */
    float getLastSpikeTime() const;

    /**
     * @brief Perform Leaky Integrate-and-Fire step for one simulation timestep
     * 
     * @param currentTime Current simulation time in seconds
     * @param dt Timestep duration in seconds
     * @return bool true if neuron fired during this timestep
     * 
     * @note Main LIF computation algorithm
     * @note Updates membrane potential, detects spikes, manages refractory period
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);

    // ============================================================================
    // Synaptic Input Processing
    // ============================================================================

    /**
     * @brief Receive excitatory synaptic input
     * 
     * @param amplitude Synaptic current amplitude in mV
     * @note Excitatory inputs depolarize membrane (make more positive)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);

    /**
     * @brief Receive inhibitory synaptic input
     * 
     * @param amplitude Synaptic current amplitude in mV
     * @note Inhibitory inputs hyperpolarize membrane (make more negative)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);

    /**
     * @brief Receive modulatory synaptic input
     * 
     * @param amplitude Modulatory input amplitude in mV
     * @note Modulatory inputs don't directly affect firing but affect plasticity
     */
    void receiveModulatoryInput(MembranePotential amplitude);

    // ============================================================================
    // External Current Injection
    // ============================================================================

    /**
     * @brief Inject external current (sensory or stimulation)
     * 
     * @param current External current amplitude in mV
     * @note Can simulate sensory input or artificial stimulation
     */
    void injectCurrent(MembranePotential current);

    /**
     * @brief Get total current affecting neuron this timestep
     * 
     * @return MembranePotential Total synaptic + external current
     */
    MembranePotential getTotalCurrent() const;

    /**
     * @brief Clear accumulated current for next timestep
     * 
     * @note Called automatically at end of LIF integration step
     */
    void clearTotalCurrent();

    // ============================================================================
    // Spike Recording and Learning
    // ============================================================================

    /**
     * @brief Record spike occurrence for learning rules
     * 
     * @param timestamp Time when spike occurred
     * @note Spike history used for STDP and other timing-dependent plasticity
     */
    void recordSpike(Timestamp timestamp);

    /**
     * @brief Get recent spike history
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     * @note History is limited to last MAX_SPIKE_HISTORY spikes for efficiency
     */
    const std::vector<Timestamp>& getSpikeHistory() const;

    /**
     * @brief Clear spike history
     * 
     * @note Called when neuron resets or is disconnected
     */
    void clearSpikeHistory();

    // ============================================================================
    // Synapse Management
    // ============================================================================

    /**
     * @brief Add incoming synapse (post-synaptic connection)
     * 
     * @param handle Synapse handle for incoming connection
     */
    void addIncomingSynapse(SynapseHandle handle);

    /**
     * @brief Add outgoing synapse (pre-synaptic connection)
     * 
     * @param handle Synapse handle for outgoing connection
     */
    void addOutgoingSynapse(SynapseHandle handle);

    /**
     * @brief Get list of incoming synapses
     * 
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;

    /**
     * @brief Get list of outgoing synapses
     * 
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;

    // ============================================================================
    // Plasticity Control
    // ============================================================================

    /**
     * @brief Get plasticity flags for this neuron
     * 
     * @return const PlasticityFlags& Plasticity eligibility flags
     */
    const PlasticityFlags& getPlasticityFlags() const;

    /**
     * @brief Get mutable plasticity flags
     * 
     * @return PlasticityFlags& Mutable reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();

    /**
     * @brief Enable specific plasticity mechanisms
     * 
     * @param hebbian Enable Hebbian learning (simultaneous activation)
     * @param stdp Enable STDP (Spike-Timing-Dependent Plasticity)
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * @note Plasticity flags determine when synaptic changes can occur
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);

    // ============================================================================
    // Network Organization
    // ============================================================================

    /**
     * @brief Set brain region membership
     * 
     * @param region Brain region ID
     */
    void setRegionId(RegionId region);

    /**
     * @brief Get brain region ID
     * 
     * @return RegionId Brain region ID
     */
    RegionId getRegionId() const;

    /**
     * @brief Set population membership
     * 
     * @param population Population ID (within region)
     */
    void setPopulationId(PopulationId population);

    /**
     * @brief Get population ID
     * 
     * @return PopulationId Population ID
     */
    PopulationId getPopulationId() const;

    // ============================================================================
    // Simulation and State Management
    // ============================================================================

    /**
     * @brief Update neuron for one simulation timestep
     * 
     * @param currentTime Current simulation time in seconds
     * 
     * @note Default implementation uses 1ms timestep
     * @note Calls stepLIF() for actual LIF computation
     */
    void step(Timestamp currentTime);

    /**
     * @brief Reset neuron to initial state
     * 
     * @note Clears membrane potential, spike history, and adaptation variables
     * @note Resets to biologically plausible initial conditions
     */
    void reset();

    /**
     * @brief Initialize neuron with random biological parameters
     * 
     * @param rng Random number generator for stochastic initialization
     * 
     * @note Creates biologically realistic neuron parameters:
     *       - Membrane potential near resting
     *       - Threshold with small variation
     *       - Refractory period of 2-10ms
     *       - Resting potential near -70mV
     */
    void initializeRandom(RandomGenerator& rng);

private:
    // ============================================================================
    // Implementation Details
    // ============================================================================

    /**
     * @brief Hidden implementation pointer (Pimpl pattern)
     * 
     * @details Neuron implementation details are hidden to reduce compilation dependencies
     *          and improve encapsulation. All operations are forwarded to the pImpl object.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
