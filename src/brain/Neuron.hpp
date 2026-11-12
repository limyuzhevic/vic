#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
// Contains all state variables needed to simulate Leaky Integrate-and-Fire (LIF) dynamics
struct NeuronState {
    // Membrane potential state variables
    MembranePotential membranePotential;    // Current membrane potential (mV). Typical values: -70mV (resting), -55mV (threshold)
    MembranePotential restingPotential;    // Resting potential where neuron returns after spike (mV)
    MembranePotential threshold;           // Firing threshold potential (mV). When crossed, neuron fires
    MembranePotential resetPotential;       // Post-spike reset potential (mV). Usually equal to resting potential
    MembranePotential leakConductance;      // Membrane leak conductance (nS). Controls decay rate toward resting potential
    MembranePotential synapseConductance;   // Total synaptic conductance (nS). Sum of all incoming synaptic weights
    
    // Firing dynamics variables
    FiringRate firingRate;                 // Instantaneous firing rate (Hz). Used for rate-based models
    FiringState firingState;               // Current firing state (Resting, Firing, or Refractory)
    uint32_t refractoryRemaining;          // Steps remaining in refractory period (0 = not refractory)
    uint32_t refractoryPeriod;             // Total refractory period in simulation steps
    float adaptationVariable;              // Spike-frequency adaptation variable. Increases with each spike
    float lastSpikeTime;                   // Timestamp of last spike (-1.0f if no spikes yet). In seconds
    
    // Constructor initializes neuron to resting state with typical LIF parameters
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
        // Phase 6: 9 steps, not 5. One action presentation is 10
        // brain steps, so a 5-step refractory let a neuron fire
        // twice inside a single burst; 9 steps caps it at about
        // one spike per presentation, which is the population's
        // natural burst rhythm anyway. This lowers the reported
        // rate without changing any drive constant (drive
        // scaling was tried and reverted: -30% drive silenced
        // the network entirely, 0 Hz, because this is a
        // threshold system with no gentle middle ground).
        , refractoryPeriod(9)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
class Neuron {
public:
    /**
     * @brief Create neuron with unique ID
     * @param id Unique identifier for this neuron
     * 
     * @post Neuron initialized with default LIF parameters and resting state.
     *       Memory allocated for implementation details (Pimpl pattern).
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy neuron and cleanup all resources
     * Automatically releases all synapse connections and
     * implementation-specific memory allocations
     */
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    /**
     * @brief Get the unique identifier of this neuron
     * @return NeuronId Unique identifier for this neuron
     * 
     * @post Id remains constant throughout neuron's lifetime.
     */
    NeuronId getId() const;
    
    // Type
    /**
     * @brief Get the functional type of this neuron
     * @return NeuronType Type classification (Sensory, Motor, or Interneuron)
     * 
     * @post Type determines neuron's role in information flow and processing.
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the functional type of this neuron
     * @param type Desired neuron type
     * 
     * @post Changes neuron type, which may affect how it's used in network
     *       simulations and which input sources are appropriate.
     */
    void setType(NeuronType type);
    
    // State access
    /**
     * @brief Get constant reference to neuron state
     * @return const NeuronState& Const reference to complete neuron state
     * 
     * @post Read-only access to all neuron state variables. Useful for
     *       observation and debugging.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to neuron state
     * @return NeuronState& Mutable reference to complete neuron, allows modification
     * 
     * @post Enables direct modification of neuron state for testing and
     *       advanced control scenarios.
     */
    NeuronState& getState();
    
    // Membrane potential
    /**
     * @brief Get current membrane potential
     * @return MembranePotential Current membrane potential (mV)
     * 
     * @post Typical values: -70mV (resting), -55mV (threshold for firing).
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential directly
     * @param potential New membrane potential value (mV)
     * 
     * @post Direct assignment, useful for initialization or testing specific conditions.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add delta to membrane potential (more efficient than set)
     * @param delta Change in membrane potential (mV)
     * 
     * @post Used for simulating synaptic inputs and external current injection.
     *       Positive delta = depolarization (moves toward threshold).
     */
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    /**
     * @brief Get firing threshold potential
     * @return MembranePotential Threshold voltage (mV) at which neuron fires
     * 
     * @post Typical values: -55mV. When membrane potential exceeds this value,
     *       neuron enters firing state.
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set firing threshold potential
     * @param threshold New threshold voltage (mV)
     * 
     * @post Useful for studying different threshold behaviors and for testing
     *       neuron responsiveness under varying conditions.
     */
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    /**
     * @brief Check if neuron is currently in firing state
     * @return bool true if neuron is firing, false otherwise
     * 
     * @post Determines if neuron can transmit signals to postsynaptic neurons.
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * @return bool true if neuron cannot fire, false otherwise
     * 
     * @post During refractory period, neuron cannot fire regardless of input,
     *       preventing pathological bursting.
     */
    bool isRefractory() const;
    
    /**
     * @brief Set firing state directly
     * @param state Desired firing state
     * 
     * @post For testing and advanced control scenarios. Note that normal
     *       operation uses automatic state transitions based on membrane dynamics.
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period duration
     * @param steps Number of simulation steps neuron must remain refractory
     * 
     * @post Controls how long neuron cannot fire after a spike. Critical for
     *       balancing network activity and preventing runaway excitation.
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter (called by brain simulation)
     * @post Automatically called each simulation step by brain implementation.
     *       When counter reaches 0, neuron can fire again.
     */
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
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
    
// Firing rate (for rate-based computation)
    /**
     * @brief Get instantaneous firing rate
     * @return FiringRate Current firing rate (Hz)
     * 
     * @post Represents number of spikes per second. Useful for rate-based
     *       models and population coding.
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set firing rate directly
     * @param rate New firing rate (Hz)
     * 
     * @post For rate-based control scenarios. Normal operation uses
     *       membrane potential dynamics to determine firing.
     */
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    /**
     * @brief Set leak conductance controlling membrane time constant
     * @param conductance Leak conductance (nS)
     * 
     * @post Higher conductance = faster decay toward resting potential,
     *       lower time constant = quicker response to inputs.
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get leak conductance
     * @return MembranePotential Leak conductance (nS)
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get refractory period duration
     * @return uint32_t Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set resting potential
     * @param potential Resting potential (mV)
     * 
     * @post Determines equilibrium point of neuron dynamics.
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get resting potential
     * @return MembranePotential Resting potential (mV)
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set reset potential (post-spike reset value)
     * @param potential Reset potential (mV)
     * 
     * @post Typically equals resting potential but can be different for
     *       spike frequency adaptation scenarios.
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get reset potential
     * @return MembranePotential Reset potential (mV)
     */
    MembranePotential getResetPotential() const;
    
    // Spike detection
    /**
     * @brief Check if neuron should fire based on current membrane potential
     * @return bool true if neuron crosses threshold, false otherwise
     * 
     * @post Synchronous threshold check. Used in both manual and automatic
     *       spike detection scenarios.
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get timestamp of last spike
     * @return float Last spike time in seconds (-1.0f if no spikes yet)
     * 
     * @post Used by STDP and other spike-timing dependent mechanisms.
     */
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    /**
     * @brief Execute one step of Leaky Integrate-and-Fire dynamics
     * @param currentTime Current simulation timestamp (seconds)
     * @param dt Time step duration (seconds)
     * @return bool true if neuron fired during this step
     * 
     * @post Main LIF computation: dV/dt = (-gL*(V-EL) + I_syn + I_ext)/C
     *       where V is membrane potential, gL is leak conductance,
     *       EL is resting potential, I_syn is synaptic current, I_ext is external current.
     *       If V exceeds threshold, neuron fires, potential resets to resetPotential,
     *       and refractory counter starts.
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    /**
     * @brief Receive excitatory synaptic input
     * @param amplitude Synaptic current amplitude (nA)
     * 
     * @post Increases membrane potential proportionally to amplitude.
     *       Positive contribution toward firing threshold.
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * @param amplitude Synaptic current amplitude (nA)
     * 
     * @post Decreases membrane potential proportionally to amplitude.
     *       Negative contribution toward firing threshold (hyperpolarization).
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * @param amplitude Synaptic current amplitude (nA)
     * 
     * @post Modulatory inputs (like neuromodulators) can change neuron
     *       properties like threshold, conductance, or gain.
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    /**
     * @brief Inject external current into neuron
     * @param current External current (nA)
     * 
     * @post Adds to total synaptic current for membrane potential computation.
     *       Used for stimulation protocols and experimental protocols.
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total current affecting neuron
     * @return MembranePotential Total current (nA) from all sources
     * 
     * @post Sum of all synaptic currents plus external injected current.
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear accumulated external current
     * @post Resets external current injection to zero. Useful for
     *       alternating between different stimulation protocols.
     */
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    /**
     * @brief Record spike occurrence at specific time
     * @param timestamp Time of spike (seconds)
     * 
     * @post Maintains spike history for spike-timing dependent plasticity
     *       (STDP). History is used to compute eligibility traces.
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get recent spike history
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     * 
     * @post Spike history used by STDP and other temporal learning rules.
     *       Typically recent spikes (within eligibility trace window).
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * @post Used when resetting neuron state or starting new learning epoch.
     */
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    /**
     * @brief Add incoming synapse connection
     * @param handle Handle to incoming synapse
     * 
     * @post Connects presynaptic neuron to this neuron, this neuron will
     *       receive input from that synapse.
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse connection
     * @param handle Handle to outgoing synapse
     * 
     * @post Connects this neuron to postsynaptic neuron, this neuron will
     *       send signals to that synapse's target.
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get list of incoming synapses
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get list of outgoing synapses
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    /**
     * @brief Get plasticity configuration flags
     * @return const PlasticityFlags& Reference to plasticity flags
     * 
     * @post Determines whether each plasticity rule (Hebbian, STDP, reward-modulated)
     *       is enabled for this neuron.
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity configuration flags
     * @return PlasticityFlags& Mutable reference to plasticity flags
     * 
     * @post Allows direct modification of plasticity settings for testing.
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Configure plasticity rules for this neuron
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * @post Sets plasticity flags for learning mechanisms. Hebbian and STDP
     *       are the main local plasticity rules in NLM Phase 2.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    /**
     * @brief Set region ID for this neuron
     * @param region Region identifier
     * 
     * @post Neurons belong to regions for organizational purposes. Region
     *       typically contains functionally related neurons.
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region ID of this neuron
     * @return RegionId Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population ID for this neuron
     * @param population Population identifier
     * 
     * @post More fine-grained than region. Populations may contain neurons
     *       with specific functional characteristics.
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population ID of this neuron
     * @return PopulationId Population identifier
     */
###################
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population ID of this neuron
     * @return PopulationId Population identifier
     */
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    /**
     * @brief Execute one simulation step (wrapper for stepLIF)
     * @param currentTime Current simulation timestamp (seconds)
     * 
     * @post Legacy method from Phase 2. Calls stepLIF with appropriate timestep.
     *       TODO PHASE 2: Implement real integrate-and-fire dynamics
     */
    void step(Timestamp currentTime);
    
    // Reset to initial state
    /**
     * @brief Reset neuron to initial state
     * 
     * @post Restores all state variables to constructor-initialized values.
     *       Clears spike history and resets membrane potential to resting.
     */
    void reset();
    
    // Initialize with random parameters
    /**
     * @brief Initialize neuron with random parameters from random generator
     * @param rng Random number generator reference
     * 
     * @post Sets random values for neuron parameters like threshold,
     *       conductance, refractory period, etc. For creating diverse initial
     *       network configurations.
     */
    void initializeRandom(RandomGenerator& rng);
    
    private:
        struct Impl;
        Impl* pImpl;
    };
    
} // namespace nlm
