#ifndef NEURON_HPP
#define NEURON_HPP

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

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
class Neuron {
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    /**
     * 
     * **Neuron Constructor**
     * =======================
     * 
     * Creates a new LIF neuron with the specified identifier.
     * 
     * **Parameters**
     * -------------
     * id: Unique identifier for the neuron (typically sequential)
     * 
     * **Initialization**
     * -----------------
     * The neuron is initialized with:
     * - Type: Internal (default)
     * - Region: INVALID_REGION_ID (until assigned)
     * - Population: INVALID_POPULATION_ID (until assigned)
     * - Membrane potential: -70.0 mV (resting)
     * - Threshold: -55.0 mV (standard cortical neuron)
     * - Refractory period: 5 ms (default)
     * - Leak conductance: 10 nS (standard)
     * 
     * **Memory Allocation**
     * ---------------------
     * Uses pImpl idiom for efficient memory management:
     * - Implementation details hidden in private Impl struct
     * - Enables copy construction/assignment deletion
     * - Supports move semantics for efficient transfer
     */
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    /**
     * 
     * **Neuron Identity**
     * ===================
     * 
     * **getId()**
     * ----------
     * 
     * Returns the unique identifier of this neuron.
     * 
     * **Returns**
     * -------
     * NeuronId: The neuron's identifier (immutable)
     * 
     * **Usage**
     * -------
     * Used for: Neuron identification in connectivity, memory systems,
     * and debugging. IDs are stable throughout neuron lifetime.
     */
    NeuronId getId() const;
    
    // Type
    /**
     * 
     * **Neuron Type**
     * =============
     * 
     * Neurons are functionally classified into types that determine their role
     * in the network:
     * 
     * - **Sensory**: Encodes external/internal stimuli
     * - **Internal**: Processes and integrates information
     * - **Motor**: Generates motor commands
     * - **Internal**: Default type for most neurons
     * 
     * **getType()**
     * ----------
     * 
     * Gets the functional type of this neuron.
     * 
     * **Returns**
     * -------
     * NeuronType: Current neuron type
     * 
     * **setType(NeuronType type)**
     * ---------------------------
     * 
     * Sets the functional type of this neuron. Type changes affect:
     * - Integration rules for synaptic inputs
     * - Output encoding behavior
     * - Memory system interactions
     * - Development trajectory
     * 
     * **Parameters**
     * -------------
     * type: New neuron type (must be valid NeuronType enum value)
     */
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State access
    /**
     * 
     * **Neuron State**
     * ===============
     * 
     * **getState()** / **getState()**
     * -------------------------------
     * 
     * Provides access to the complete neuron state for inspection and modification.
     * 
     * **Returns**
     * -------
     * const NeuronState&: Immutable state reference (inspection)
     * NeuronState&: Mutable state reference (modification)
     * 
     * **NeuronState Structure**
     * -------------------------
     * Contains all dynamic parameters governing neuron behavior:
     * 
     * membranePotential: Current membrane potential (mV)
     * restingPotential: Leak reversal potential (-70mV)
     * threshold: Firing threshold (-55mV)
     * resetPotential: Post-spike reset (-70mV)
     * leakConductance: Membrane conductance (nS)
     * synapseConductance: Synaptic conductance (nS)
     * firingRate: Current firing rate (Hz)
     * firingState: Current state (Resting, Active, Refractory)
     * refractoryRemaining: Steps remaining in refractory period
     * refractoryPeriod: Total refractory period duration
     * adaptationVariable: Spike-frequency adaptation variable
     * lastSpikeTime: Timestamp of last spike (-1 if none)
     */
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane potential
    /**
     * 
     * **Membrane Potential Control**
     * =============================
     * 
     * **getMembranePotential()**
     * ------------------------
     * 
     * Gets the current membrane potential.
     * 
     * **Returns**
     * -------
     * MembranePotential: Current membrane potential in mV
     * 
     * **setMembranePotential(MembranePotential potential)**
     * ------------------------------------------------------
     * 
     * Sets the membrane potential directly (bypassing integration).
     * 
     * **Warning**
     * -------
     * This is a direct override and should be used carefully:
     * - May cause instability if not respecting biophysical constraints
     * - Recommended for initialization only
     * - Integrates with adaptation mechanisms
     * 
     * **Parameters**
     * -------------
     * potential: Membrane potential in mV (-100 to 50 mV recommended)
     * 
     * **addToMembranePotential(MembranePotential delta)**
     * -----------------------------------------------------
     * 
     * Adds a delta to the current membrane potential.
     * 
     * **Use Case**
     * ---------:
     * Manual current injection, sensory input encoding, or testing.
     */
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    /**
     * 
     * **Firing Threshold**
     * ===================
     * 
     * **getThreshold()**
     * -----------------
     * 
     * Gets the current firing threshold.
     * 
     * **Returns**
     * -------
     * MembranePotential: Firing threshold in mV
     * 
     * **setThreshold(MembranePotential threshold)**
     * -------------------------------------------
     * 
     * Sets the firing threshold.
     * 
     * **Biological Significance**
     * ---------------------------
     * Threshold determines neuronal excitability:
     * - Lower threshold = more excitable
     * - Higher threshold = more selective
     * - Modulation affects learning and adaptation
     * 
     * **Parameters**
     * -------------
     * threshold: Firing threshold in mV (typically -55 to -40 mV)
     */
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    /**
     * 
     * **Firing State**
     * ===============
     * 
     * **isFiring()** / **isRefractory()**
     * --------------------------------
     * 
     * Query the current firing state.
     * 
     * **FiringState Enum**
     * --------------------
     * RESTING: Not actively processing inputs
     * ACTIVE: Processing inputs, may fire soon
     * REFRACTORY: Recently fired, cannot fire
     * 
     * **getState()**
     * ----------
     * 
     * Sets the firing state directly.
     * 
     * **Parameters**
     * -------------
     * state: Desired firing state (must be valid FiringState)
     */
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    /**
     * 
     * **Firing Rate**
     * =============
     * 
     * **getFiringRate()** / **setFiringRate(FiringRate rate)**
     * ----------------------------------------------------------
     * 
     * Gets/sets the firing rate in Hz.
     * 
     * **Usage**
     * -------
     * For rate-based neural models and population coding. The LIF model
     * computes instantaneous firing rates from spike timing, which can be
     * exposed through this interface.
     */
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    /**
     * 
     * **LIF Neuron Parameters**
     * =========================
     * 
     * These parameters govern the basic LIF dynamics:
     * 
     * **Leak Conductance**
     * --------------------
     * 
     * **getLeakConductance()** / **setLeakConductance(MembranePotential conductance)**
     * -----------------------------------------------------------------------------
     * 
     * Gets/sets the leak conductance (inverse of membrane resistance).
     * 
     * **Biological Role**
     * -----------------
     * Controls membrane time constant (tau = C/g) and integration speed:
     * - Higher conductance = faster integration
     * - Lower conductance = slower, more precise integration
     * - Default: 10 nS (typical cortical neuron)
     * 
     * **Parameters**
     * -------------
     * conductance: Leak conductance in nS (positive value)
     * 
     * **Refractory Period**
     * ---------------------
     * 
     * **getRefractoryPeriod()**
     * ------------------------
     * 
     * Gets the refractory period duration.
     * 
     * **setRefractoryPeriod(uint32_t steps)**
     * --------------------------------------
     * 
     * Sets the refractory period duration in simulation steps.
     * 
     * **Biological Meaning**
     * ---------------------
     * Models absolute and relative refractory periods:
     * - Prevents immediate re-firing after spike
     * - Enables spike-timing dependent effects
     * - Typical: 2-10 ms (2-50 steps at 1ms timestep)
     */
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    /**
     * 
     * **Spike Detection**
     * ==================
     * 
     * **checkThreshold()**
     * --------------------
     * 
     * Checks if neuron has reached firing threshold.
     * 
     * **Returns**
     * -------
     * bool: true if membrane potential >= threshold
     * 
     * **getLastSpikeTime()**
     * -----------------------
     * 
     * Gets the timestamp of the last spike.
     * 
     * **Returns**
     * -------
     * float: Last spike timestamp, -1.0 if no spikes yet
     * 
     * **Usage**
     * -------
     * For plasticity rules and timing-based learning. Critical for
     * STDP (Spike-Timing-Dependent Plasticity) and other timing-dependent
     * processes.
     */
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    /**
     * 
     * **LIF Integration Step**
     * ========================
     * 
     * **stepLIF(Timestamp currentTime, TimestepDuration dt)**
     * --------------------------------------------------------
     * 
     * Integrates the Leaky Integrate-and-Fire neuron dynamics for one time step.
     * 
     * **Mathematical Implementation**
     * -------------------------------
     * Implements the LIF differential equation:
     * 
     *     dV/dt = (V_rest - V)/tau + I/C
     *     
     * where:
     * - V = membrane potential
     * - V_rest = resting potential (-70 mV)
     * - tau = membrane time constant (20 ms)
     * - I = total synaptic current
     * - C = membrane capacitance (1 nF)
     * 
     * **Integration Method**
     * -----------------------
     * Uses exponential Euler for stability and efficiency:
     * 
     *     V_new = V_old + dt × 1000 × [(V_rest - V_old)/tau + I/C]
     *     
     * The multiplication by 1000 converts seconds to milliseconds.
     * 
     * **Returns**
     * -------
     * bool: true if neuron fired during this step
     * 
     * **Parameters**
     * -------------
     * currentTime: Current simulation timestamp (ms)
     * dt: Time step in seconds (typically 0.001 for 1ms)
     * 
     * **Side Effects**
     * --------------
     * - Updates membrane potential
     * - Records spikes in history
     * - Manages refractory period
     * - Updates adaptation variable
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    /**
     * 
     * **Synaptic Input**
     * =================
     * 
     * **receiveExcitatoryInput(MembranePotential amplitude)**
     * ----------------------------------------------------------
     * 
     * Receives excitatory post-synaptic potential (PSP).
     * 
     * **Biological Mechanism**
     * ------------------------
     * Models AMPA-type synaptic currents that depolarize the neuron:
     * - Increases membrane potential toward positive reversal
     * - Typically lasts 1-5 ms
     * - Contributes to temporal summation
     * 
     * **Parameters**
     * -------------
     * amplitude: PSP magnitude in mV (positive value)
     * 
     * **receiveInhibitoryInput(MembranePotential amplitude)**
     * --------------------------------------------------------
     * 
     * Receives inhibitory post-synaptic potential (PSP).
     * 
     * **Biological Mechanism**
     * ------------------------
     * Models GABA-type synaptic currents that hyperpolarize the neuron:
     * - Decreases membrane potential toward negative reversal
     * - Typically lasts 5-20 ms
     * - Provides lateral inhibition and gain control
     * 
     * **Parameters**
     * -------------
     * amplitude: PSP magnitude in mV (positive value, effect is subtractive)
     * 
     * **receiveModulatoryInput(MembranePotential amplitude)**
     * ----------------------------------------------------------
     * 
     * Receives modulatory input that affects plasticity, not immediate potential.
     * 
     * **Neuromodulatory Systems**
     * ---------------------------
     * Models dopamine, acetylcholine, serotonin effects:
     * - Modulate synaptic strength indirectly
     * - Gate learning processes
     * - Adjust neuronal excitability over longer timescales
     * 
     * **Parameters**
     * -------------
     * amplitude: Modulation strength (can be positive or negative)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    /**
     * 
     * **Current Injection**
     * ====================
     * 
     * **injectCurrent(MembranePotential current)**
     * --------------------------------------------
     * 
     * Injects current directly into the neuron (bypasses synapse modeling).
     * 
     * **Use Cases**
     * -----------
     * - Sensory input encoding
     * - Motor command generation
     * - Experimental stimulation
     * - External drive signals
     * 
     * **Parameters**
     * -------------
     * current: Current in nA (converted to mV via membrane properties)
     */
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    /**
     * 
     * **Spike History**
     * ================
     * 
     * **recordSpike(Timestamp timestamp)**
     * ------------------------------------
     * 
     * Records a spike for plasticity and analysis.
     * 
     * **Parameters**
     * -------------
     * timestamp: Time when spike occurred
     * 
     * **getSpikeHistory()**
     * ------------------------
     * 
     * Gets the spike history vector (recent spikes only).
     * 
     * **Returns**
     * -------
     * const std::vector<Timestamp>&: Timestamps of recent spikes
     * 
     * **clearSpikeHistory()**
     * -------------------------
     * 
     * Clears the spike history (typically on neuron reset).
     * 
     * **STDP Relevance**
     * -----------------
     * Essential for Spike-Timing-Dependent Plasticity:
     * - Pre-synaptic spikes recorded in synapses
     * - Post-synaptic spikes recorded in neurons
     * - Time differences determine weight changes
     */
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    /**
     * 
     * **Synapse Management**
     * =====================
     * 
     * **addIncomingSynapse(SynapseHandle handle)**
     * ------------------------------------------------
     * 
     * Adds an incoming synapse to this neuron.
     * 
     * **Parameters**
     * -------------
     * handle: Synapse handle for the incoming connection
     * 
     * **addOutgoingSynapse(SynapseHandle handle)**
     * --------------------------------------------
     * 
     * Adds an outgoing synapse from this neuron.
     * 
     * **Parameters**
     * -------------
     * handle: Synapse handle for the outgoing connection
     * 
     * **getIncomingSynapses()** / **getOutgoingSynapses()**
     * -----------------------------------------------------
     * 
     * Gets the lists of synaptic connections.
     * 
     * **Returns**
     * -------
     * const std::vector<SynapseHandle>&: Lists of synapse handles
     * 
     * **Use Cases**
     * -----------
     * - Plasticity rule application
     * - Synaptic efficacy analysis
     * - Network topology queries
     */
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    /**
     * 
     * **Plasticity State**
     * ===================
     * 
     * **getPlasticityFlags()** / **getPlasticityFlags()**
     * ----------------------------------------------------
     * 
     * Access plasticity configuration for this neuron.
     * 
     * **PlasticityFlags Structure**
     * ----------------------------
     * hebbian: Enable Hebbian co-activation learning
     * stdp: Enable Spike-Timing-Dependent Plasticity
     * rewardModulated: Enable reward-modulated plasticity
     * 
     * **enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)**
     * -----------------------------------------------------------------
     * 
     * Configures plasticity mechanisms for this neuron.
     * 
     * **Parameters**
     * -------------
     * hebbian: Enable/Disable Hebbian learning
     * stdp: Enable/Disable STDP
     * rewardModulated: Enable/Disable reward modulation
     */
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    /**
     * 
     * **Network Topology**
     * ===================
     * 
     * **setRegionId(RegionId region)**
     * --------------------------------
     * 
     * Assigns this neuron to a brain region.
     * 
     * **Parameters**
     * -------------
     * region: Region identifier
     * 
     * **getRegionId()**
     * -----------------
     * 
     * Gets the neuron's region identifier.
     * 
     * **setPopulationId(PopulationId population)**
     * -------------------------------------------
     * 
     * Assigns this neuron to a population within its region.
     * 
     * **Parameters**
     * -------------
     * population: Population identifier
     * 
     * **getPopulationId()**
     * --------------------
     * 
     * Gets the neuron's population identifier.
     * 
     * **Organization Benefits**
     * ------------------------
     * - Enables region-specific processing
     * - Supports population coding schemes
     * - Facilitates hierarchical organization
     * - Enables efficient connectivity routing
     */
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    /**
     * 
     * **Simulation Step**
     * ===================
     * 
     * **step(Timestamp currentTime)**
     * -------------------------------
     * 
     * Performs a simulation step using standard LIF dynamics.
     * 
     * **Simplified Integration**
     * --------------------------
     * This is a convenience method that uses:
     * - Standard timestep: 1ms (0.001 seconds)
     * - Default LIF integration via stepLIF()
     * 
     * **Parameters**
     * -------------
     * currentTime: Current simulation time in milliseconds
     * 
     * **Note**
     * -----
     * For advanced applications, use stepLIF() directly with custom
     * timesteps or integration schemes.
     */
    void step(Timestamp currentTime);
    
    // Reset to initial state
    /**
     * 
     * **Neuron Reset**
     * ================
     * 
     * **reset()**
     * ----------
     * 
     * Resets the neuron to its initial state.
     * 
     * **Reset Actions**
     * -----------------
     * - Clears membrane potential to resting state
     * - Resets synaptic input accumulators
     * - Clears spike history
     * - Restores default firing state (Resting)
     * - Resets adaptation variable
     * - Resets last spike time to -1.0
     * 
     * **Use Cases**
     * -----------
     * - Episode boundary reset
     * - Experiment restart
     * - Error recovery
     * - Memory consolidation trigger
     */
    void reset();
    
    // Initialize with random parameters
    /**
     * 
     * **Random Initialization**
     * =========================
     * 
     * **initializeRandom(RandomGenerator& rng)**
     * ------------------------------------------
     * 
     * Initializes neuron parameters with biologically realistic values.
     * 
     * **Biological Ranges**
     * --------------------
     * - Membrane potential: Resting ±3mV (-73 to -67 mV)
     * - Threshold: -55 ±2mV (-57 to -53 mV)
     * - Resting potential: -70 ±2mV (-72 to -68 mV)
     * - Reset potential: Resting +0 to +5mV (-70 to -65 mV)
     * - Refractory period: 2-10 ms (random uniform)
     * 
     * **Parameters**
     * -------------
     * rng: Random number generator for reproducible initialization
     * 
     * **Initialization Sequence**
     * ---------------------------
     * 1. Set membrane potential near resting
     * 2. Set threshold with small variation
     * 3. Set resting potential with small variation
     * 4. Set reset potential near resting
     * 5. Set refractory period randomly
     * 6. Reset state variables
     * 
     * **Purpose**
     * ---------
     * Creates diverse initial conditions that reflect biological variability
     * while maintaining functional constraints for stable network dynamics.
     */
    void initializeRandom(RandomGenerator& rng);
    
    private:
    struct Impl;
    neuron_impl_ptr pImpl;
};

} // namespace nlm

#endif // NEURON_HPP
