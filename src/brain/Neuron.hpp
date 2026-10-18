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
 * @class Neuron
 * @brief Single neuron implementing Leaky Integrate-and-Fire (LIF) dynamics
 * 
 * This class represents a biological neuron with realistic biophysical properties:
 * - Membrane potential integration with exponential decay
 * - Spike generation when threshold is reached
 * - Refractory period after spiking
 * - Synaptic input integration (excitatory, inhibitory, modulatory)
 * - Spike-frequency adaptation through adaptation variable
 * - Multiple neuron types (excitatory, inhibitory, sensory, motor, modulatory)
 * 
 * @details The LIF model is the workhorse of computational neuroscience, providing
 * a balance of biological realism and computational efficiency. The mathematical
 * foundation is based on the Hodgkin-Huxley model but simplified for real-time
 * simulation with event-driven spike processing.
 * 
 * **LIF Mathematical Model**:
 * ```
 * τ · dV/dt = -g_L(V - E_L) + I_syn + I_ext
 * V(t) = V_reset when V(t) ≥ V_th
 * V(t+dt) = V_reset + (V(0) - V_reset)e^{-dt/τ}
 * ```
 * where:
 * - V(t): membrane potential at time t
 * - τ: membrane time constant (≈ 20ms)
 * - g_L: leak conductance
 * - E_L: resting potential (-70mV)
 * - I_syn: total synaptic current
 * - I_ext: external injected current
 * 
 * **Neuron Types**:
 * - Sensory: Receives environmental input, low threshold
 * - Internal: Integrates and processes signals, standard dynamics
 * - Motor: Transmits output signals, high firing rates
 * - Modulatory: Releases neuromodulators, slow adaptation
 * - Excitatory: Releases excitatory neurotransmitters
 * - Inhibitory: Releases inhibitory neurotransmitters
 * 
 * @see Brain.hpp - brain integration algorithm
 * @see Synapse.hpp - synaptic dynamics
 * @see ../plasticity/STDP.hpp - spike-timing dependent plasticity
 */

class Neuron {
public:
        /**
     * @brief Create neuron with unique identifier
     * 
     * @details Neuron constructor initializes neuron state with biologically
     * realistic parameters and sets up dynamic state management.
     * 
     * **Initial State Parameters**:
     * - Membrane potential: -70mV (resting)
     * - Threshold: -55mV (50mV above resting)
     * - Refractory period: 5 simulation steps
     * - Leak conductance: 10nS (standard)
     * - Firing state: Resting
     * 
     * @param id Unique identifier for this neuron (assigned by brain)
     * @pre Neuron ID must be unique within brain
     * @post Neuron state is initialized with default parameters
     */
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
        /**
     * @brief Get neuron identifier
     * 
     * @return Unique neuron ID
     * @note ID is assigned by brain during initialization and remains constant
     */
    NeuronId getId() const;
    
    /**
     * @brief Get neuron type
     * 
     * @return Neuron type (Sensory, Internal, Motor, etc.)
     * @note Type determines functional role and parameters
     */
    NeuronType getType() const;
    /**
     * @brief Set neuron type
     * 
     * @param type New neuron type
     * @post Updates type-specific parameters (threshold, adaptation, etc.)
     * @see NeuronType enum in Types.hpp
     */
    void setType(NeuronType type);
    
    // State access
    /**
     * @brief Get complete neuron state
     * 
     * @return Const reference to neuron state structure
     * @note State includes membrane potential, firing state, refractory period,
     * and adaptation variable
     */
    const NeuronState& getState() const;
    /**
     * @brief Get complete neuron state (non-const version)
     * 
     * @return Reference to neuron state structure
     * @warning Direct modification may lead to inconsistent state
     */
    NeuronState& getState();
    
        // Membrane potential
    /**
     * @brief Get current membrane potential
     * 
     * @return Membrane potential in mV (-100 to 60 typical range)
     * @note Integration of synaptic inputs and external currents
     */
    MembranePotential getMembranePotential() const;
    /**
     * @brief Set membrane potential directly
     * 
     * @details Direct setting is primarily used for testing and initialization.
     * For normal operation, use synaptic inputs or current injection.
     * 
     * @param potential New membrane potential (mV)
     * @warning Can create inconsistent state if called during simulation
     */
    void setMembranePotential(MembranePotential potential);
    /**
     * @brief Add to membrane potential (synaptic input)
     * 
     * @details This is the primary mechanism for synaptic input integration.
     * Synaptic currents are accumulated and applied during the next LIF step.
     * 
     * @param delta Change in membrane potential (mV)
     * @post Membrane potential is updated for next integration step
     */
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    /**
     * @brief Get firing threshold
     * 
     * @return Membrane potential threshold (mV) for spike generation
     * @note Typical values: -55mV (excitatory), -50mV (sensory)
     */
    MembranePotential getThreshold() const;
    /**
     * @brief Set firing threshold
     * 
     * @param threshold New firing threshold (mV)
     * @post Affects spike generation timing
     */
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    /**
     * @brief Check if neuron is currently firing
     * 
     * @return true if neuron has just crossed threshold and will spike
     * @note This is different from refractory state - firing is the instant
     * of spike generation
     */
    bool isFiring() const;
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return true if neuron cannot fire (recovery in progress)
     * @note Refractory period prevents excessive firing rates
     */
    bool isRefractory() const;
    /**
     * @brief Set firing state
     * 
     * @param state New firing state (Resting, Active, Refractory)
     */
    void setFiringState(FiringState state);
    /**
     * @brief Set refractory period duration
     * 
     * @param steps Number of simulation steps in refractory period
     * @post Affects recovery time after spiking
     */
    void setRefractoryPeriod(uint32_t steps);
    /**
     * @brief Decrement refractory period counter
     * 
     * @post Called automatically during simulation step processing
     * @note Returns neuron to resting state when period completes
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
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
        // Firing rate (for rate-based computation)
    /**
     * @brief Get current firing rate
     * 
     * @return Firing rate in Hz (spikes per second)
     * @note Used for rate-based neural computations
     */
    FiringRate getFiringRate() const;
    /**
     * @brief Set firing rate directly
     * 
     * @param rate Firing rate in Hz
     * @warning Can create inconsistent state with spike history
     */
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    /**
     * @brief Set leak conductance
     * 
     * @param conductance Leak conductance (nS)
     * @post Affects membrane time constant τ = C/g_L
     */
    void setLeakConductance(MembranePotential conductance);
    /**
     * @brief Get leak conductance
     * 
     * @return Leak conductance (nS)
     * @note Controls membrane time constant and decay rate
     */
    MembranePotential getLeakConductance() const;
    /**
     * @brief Get refractory period duration
     * 
     * @return Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    /**
     * @brief Set resting potential
     * 
     * @param potential Resting membrane potential (mV)
     * @post Affects input integration range
     */
    void setRestingPotential(MembranePotential potential);
    /**
     * @brief Get resting potential
     * 
     * @return Resting potential (mV)
     */
    MembranePotential getRestingPotential() const;
    /**
     * @brief Set reset potential
     * 
     * @param potential Post-spike reset potential (mV)
     * @post Typically slightly above resting potential
     */
    void setResetPotential(MembranePotential potential);
    /**
     * @brief Get reset potential
     * 
     * @return Reset potential (mV)
     */
    MembranePotential getResetPotential() const;
    
    // Spike detection
    /**
     * @brief Check if membrane potential crosses threshold
     * 
     * @return true if V_m ≥ V_th (spike would occur)
     * @note Does not change firing state, just indicates condition
     */
    bool checkThreshold() const;
    /**
     * @brief Get time of last spike
     * 
     * @return Timestamp of last spike, or -1.0f if no spikes yet
     * @note Used for STDP and spike history analysis
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief LIF neuron dynamics step function
     * 
     * @details Implements the complete Leaky Integrate-and-Fire dynamics
     * including membrane potential integration, spike detection, and
     * post-spike reset with refractory period.
     * 
     * **LIF Mathematical Formulas**:
     * ```
     * τ = C / g_L  (membrane time constant)
     * α = exp(-dt / τ)  (decay factor)
     * V_new = V * α + (V_rest + I/C) * (1 - α)  (membrane update)
     * spike = (V_new ≥ V_th) ? true : false  (threshold crossing)
     * V_reset = V_reset if spike else V_new  (post-spike reset)
     * ```
     * 
     * @param currentTime Current simulation time (ms)
     * @param dt Time step duration (seconds)
     * @return true if neuron fired (spike emitted), false otherwise
     * @post Updates membrane potential, firing state, and spike history
     * @see step() - convenience method with fixed dt
     * @see ../brain/Brain.cpp:306 - integration in brain simulation loop
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    /**
     * @brief Receive excitatory synaptic input
     * 
     * @param amplitude Synaptic current amplitude (mV)
     * @post Current is added to synaptic input buffer for next LIF step
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * @param amplitude Synaptic current amplitude (mV)
     * @post Current is subtracted from synaptic input buffer
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    /**
     * @brief Receive modulatory synaptic input
     * 
     * @param amplitude Neuromodulatory current (mV)
     * @post Updates adaptation variable for spike-frequency adaptation
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    /**
     * @brief Inject external current into neuron
     * 
     * @details External current can come from sensory input, neuromodulators,
     * or experimental stimulation. Bypasses synaptic transmission.
     * 
     * @param current Current amplitude (mV)
     * @post Added to synaptic input for immediate effect
     */
    void injectCurrent(MembranePotential current);
    /**
     * @brief Get total current affecting neuron
     * 
     * @return Total current (synaptic + external) in mV
     * @note Reset to zero after each LIF step
     */
    MembranePotential getTotalCurrent() const;
    /**
     * @brief Clear current buffer
     * 
     * @post Resets total current to zero for next step
     */
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    /**
     * @brief Record spike event with timestamp
     * 
     * @param timestamp Time of spike (ms)
     * @post Adds to spike history for plasticity computations
     */
    void recordSpike(Timestamp timestamp);
    /**
     * @brief Get recent spike history
     * 
     * @return Const reference to vector of spike timestamps
     * @note Used by STDP for spike-timing dependent plasticity
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    /**
     * @brief Clear spike history
     * 
     * @post Removes all spike timestamps
     */
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    /**
     * @brief Add incoming synapse connection
     * 
     * @param handle Handle to incoming synapse
     * @post Synapse tracks incoming connections for synaptic input
     */
    void addIncomingSynapse(SynapseHandle handle);
    /**
     * @brief Add outgoing synapse connection
     * 
     * @param handle Handle to outgoing synapse
     * @post Synapse tracks outgoing connections for spike propagation
     */
    void addOutgoingSynapse(SynapseHandle handle);
    /**
     * @brief Get incoming synapse connections
     * 
     * @return Const reference to vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    /**
     * @brief Get outgoing synapse connections
     * 
     * @return Const reference to vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    /**
     * @brief Get plasticity flags
     * 
     * @return Const reference to plasticity flags structure
     */
    const PlasticityFlags& getPlasticityFlags() const;
    /**
     * @brief Get plasticity flags (non-const version)
     * 
     * @return Reference to plasticity flags structure
     * @warning Direct modification may interfere with plasticity system
     */
    PlasticityFlags& getPlasticityFlags();
    /**
     * @brief Enable specific plasticity rules
     * 
     * @details Configures which plasticity mechanisms are active for this neuron:
     * - Hebbian: synchronous activity-based potentiation
     * - STDP: spike-timing dependent plasticity
     * - Reward-modulated: eligibility traces for reinforcement learning
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing dependent plasticity
     * @param rewardModulated Enable reward-modulated learning
     * @post Plasticity rules are applied in brain simulation loop
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    /**
     * @brief Set neuron region identifier
     * 
     * @param region Region ID where neuron resides
     * @post Affects brain-wide connectivity and plasticity
     */
    void setRegionId(RegionId region);
    /**
     * @brief Get neuron region identifier
     * 
     * @return Region ID where neuron resides
     */
    RegionId getRegionId() const;
    /**
     * @brief Set neuron population identifier
     * 
     * @param population Population ID within region
     * @post Populates population-based statistics and connectivity
     */
    void setPopulationId(PopulationId population);
    /**
     * @brief Get neuron population identifier
     * 
     * @return Population ID within region
     */
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    /**
     * @brief Complete neuron simulation step
     * 
     * @details High-level simulation step that delegates to stepLIF with
     * standard timestep. Handles both spiking and non-spiking neurons.
     * 
     * @param currentTime Current simulation time (ms)
     * @post Updates membrane potential, firing state, and spike history
     * @see stepLIF() - detailed LIF implementation
     * @see ../brain/Brain.cpp:339 - integration in brain step loop
     */
    void step(Timestamp currentTime);
    
    // Reset to initial state
    /**
     * @brief Reset neuron to initial state
     * 
     * @details Complete state reset including:
     * - Reset membrane potential to resting value
     * - Clear synaptic currents
     * - Reset firing state to resting
     * - Clear spike history
     * - Reset adaptation variable
     * 
     * @post Neuron is ready for new simulation
     */
    void reset();
    
    // Initialize with random parameters
    /**
     * @brief Initialize neuron with random biologically plausible parameters
     * 
     * @details Generates random neuron parameters within biological ranges:
     * - Membrane potential: resting ±3mV
     * - Threshold: -55mV ±2mV
     * - Resting potential: -70mV ±2mV
     * - Reset potential: resting + random offset
     * - Refractory period: 2-10 simulation steps
     * - Leak conductance: type-dependent range
     * 
     * @param rng Reference to random number generator
     * @post Neuron parameters are randomly initialized
     * @throws std::runtime_error if random generator fails
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm