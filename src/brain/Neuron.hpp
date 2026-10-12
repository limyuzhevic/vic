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
 * @struct NeuronState
 * @brief Comprehensive neuron state storage for efficient access
 * @description Encapsulates all neuron parameters needed for Leaky Integrate-and-Fire (LIF)
 *              dynamics in a single structure for optimal performance. This struct
 *              contains membrane potential, synaptic inputs, firing state, and history.
 * @note Designed for cache efficiency - all related variables grouped together
 * @version 1.0.0
 */
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    /**
     * @brief Construct default neuron state
     * @description Initializes neuron with typical LIF parameters:
     *              - Membrane potential: -70 mV (resting)
     *              - Threshold: -55 mV
     *              - Refractory period: 5 simulation steps
     * @note Other parameters set to zero/ neutral values
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
 * @brief Single neuron implementing Leaky Integrate-and-Fire (LIF) dynamics
 * @description Represents an individual biological neuron with realistic electrical properties.
 *              The neuron integrates incoming synaptic currents, generates action potentials
 *              when threshold is reached, and exhibits refractory periods and adaptation.
 * @note This is the fundamental computational unit of the neural system. All neurons
 *       are organized within regions and participate in local and global dynamics.
 * @warning During development phase, some methods are PLACEHOLDERs (step(), stepLIF())
 * @version 1.0.0
 * @copyright Copyright (c) 2024 NLM Team
 */
class Neuron {
public:
    /**
     * @brief Construct neuron with unique identifier
     * @param id Unique neuron identifier for addressing and debugging
     * @description Creates a new neuron instance with the specified ID.
     *              The neuron is initialized with default LIF parameters.
     * @warning IDs must be unique across the entire brain system
     * @see NeuronId
     * @see reset
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @virtual Destructor
     * @description Cleans up neuron resources and performs any necessary cleanup.
     *              Handles synaptic connections and internal state management.
     */
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get neuron identifier
     * @return Const reference to neuron ID
     * @description Returns the unique identifier assigned at construction.
     *              Used for addressing and debugging.
     */
    NeuronId getId() const;
    
    /**
     * @brief Get neuron type classification
     * @return Neuron type (Excitatory, Inhibitory, etc.)
     * @description Returns the neuron's functional type, which determines its
     *              role in network dynamics and plasticity rules.
     * @see NeuronType
     */
    NeuronType getType() const;
    
    /**
     * @brief Set neuron type
     * @param type New neuron type
     * @description Changes the neuron's functional classification.
     *              This affects synaptic interactions and plasticity eligibility.
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get current neuron state
     * @return Const reference to neuron state struct
     * @description Provides read-only access to all neuron parameters including
     *              membrane potential, firing rate, and history.
     * @note State updates are performed by internal methods, not this getter
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get current neuron state (mutable)
     * @return Mutable reference to neuron state struct
     * @description Provides read-write access to neuron state for direct manipulation.
     *              Use with caution - most state should be modified through dedicated methods.
     */
    NeuronState& getState();
    
    /**
     * @brief Get current membrane potential
     * @return Membrane potential in mV (-100 to +50 typical range)
     * @description Returns the current electrical potential across the neuron's
     *              membrane. Values below -55 mV typically indicate subthreshold activity,
     *              while values crossing -55 mV trigger action potentials.
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential directly
     * @param potential New membrane potential in mV
     * @description Sets the membrane potential directly, bypassing synaptic integration.
     *              Use this for external current injection or testing.
     * @warning Can cause unrealistic neuron behavior if used incorrectly
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add current to membrane potential
     * @param delta Change in membrane potential (mV)
     * @description Increments the membrane potential by the specified amount.
     *              Positive delta excites the neuron, negative delta inhibits it.
     * @note This models the effect of synaptic inputs
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get firing threshold
     * @return Threshold voltage in mV (typically -55 mV)
     * @description Returns the voltage at which the neuron generates an action potential.
     *              Threshold determines the neuron's sensitivity to inputs.
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set firing threshold
     * @param threshold New threshold voltage in mV
     * @description Adjusts the voltage at which the neuron fires.
     *              Higher thresholds make neurons less excitable.
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * @return true if neuron is in Active firing state
     * @description Returns true when the neuron has generated an action potential
     *              and is in its refractory or post-firing period.
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * @return true if neuron cannot fire (refractory period active)
     * @description Returns true when the neuron is recovering from a spike
     *              and unable to generate additional action potentials.
     */
    bool isRefractory() const;
    
    /**
     * @brief Set firing state
     * @param state New firing state (Resting, Active, Refractory, Inhibited)
     * @description Manually sets the neuron's firing state, useful for testing
     *              or external control. Most state changes occur automatically.
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period duration
     * @param steps Number of simulation steps in refractory period
     * @description Configures how long the neuron remains unable to fire after
     *              generating a spike. Typical values range from 1-20 steps.
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory period counter
     * @description Called each simulation step to reduce the refractory period.
     *              When counter reaches zero, neuron can fire again.
     * @note This is an internal method called automatically by the brain system
     */
    void decrementRefractory();
    
    /**
     * @brief Get firing rate
     * @return Firing rate in Hz (spikes per second)
     * @description Returns the neuron's average firing rate, which can be used
     *              for rate-based computations or as feedback signal.
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set firing rate
     * @param rate New firing rate in Hz
     * @description Sets the neuron's firing rate directly, useful for rate-based
     *              models or testing. This affects the neuron's output signaling.
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * * @brief Get leak conductance parameter
     * @return Leak conductance in nS (typically 10-100 nS)
     * @description Returns the leak conductance that determines how quickly the
     *              membrane potential returns to resting level in the absence of inputs.
     * @note Higher leak conductance = faster membrane time constant
     */
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if neuron has reached firing threshold
     * @return true if membrane potential >= threshold
     * @description Tests whether the neuron is ready to fire based on its
     *              current membrane potential compared to threshold.
     * @note Used by spike detection and LIF simulation
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get timestamp of last spike
     * @return Last spike timestamp in seconds (-1 if never spiked)
     * @description Returns the simulation time when the neuron last fired,
     *              or -1 if it has never fired. Used for STDP and firing rate calculations.
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Execute one LIF neuron simulation step
     * @param currentTime Current simulation time in seconds
     * @param dt Timestep duration in seconds
     * @return true if neuron fired during this step, false otherwise
     * @description Implements the Leaky Integrate-and-Fire dynamics equation:
     *              V(t+dt) = V(t) * exp(-dt/tau) + (I*dt/C)
     * @note This is the core neuron computation method. Returns true when neuron
     *       generates an action potential.
     * @warning During development phase, this method is a placeholder
     * @see step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * @param amplitude Synaptic current amplitude (positive mV)
     * @description Processes an excitatory postsynaptic potential (EPSP) by adding
     *              current to the membrane potential. EPSPs depolarize the neuron.
     * @note Typically amplitudes range from 0.1 to 10.0 mV
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * @param amplitude Synaptic current amplitude (negative mV)
     * @description Processes an inhibitory postsynaptic potential (IPSP) by subtracting
     *              current from the membrane potential. IPSPs hyperpolarize the neuron.
     * @note Typically amplitudes range from -0.1 to -10.0 mV
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * @param amplitude Synaptic current amplitude (can be positive or negative)
     * @description Processes neuromodulatory synaptic input that can have long-lasting
     *              effects on neuron excitability and plasticity.
     * @note Modulatory inputs often affect intrinsic properties rather than immediate firing
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current
     * @param current Amount of current to inject (mV)
     * @description Injects current from external sources (sensory input, motor commands).
     *              This bypasses synapses and directly affects membrane potential.
     * @note Used for sensory input and motor output control
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total injected current
     * @return Sum of all injected current since last reset
     * @description Returns the accumulated external current input, which can be useful
     *              for monitoring and debugging.
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear injected current accumulator
     * @description Resets the total current counter to zero.
     *              Called by reset() method.
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record spike event for plasticity
     * @param timestamp Simulation time when spike occurred
     * @description Stores the spike timestamp for use by spike-timing-dependent
     *              plasticity (STDP) and other learning rules.
     * @note Spike history is used to compute synaptic eligibility traces
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get spike history for plasticity computation
     * @return Const reference to vector of spike timestamps
     * @description Returns timestamps of all spikes recorded since last clear.
     *              Used by STDP and other activity-dependent plasticity rules.
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * @description Removes all recorded spike timestamps.
     *              Called by reset() method.
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse to neuron
     * @param handle Synapse handle (pointer to synapse object)
     * @description Registers a synapse that sends signals to this neuron.
     *              Used for synaptic management and cleanup.
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse from neuron
     * @param handle Synapse handle (pointer to synapse object)
     * @description Registers a synapse that this neuron sends signals to.
     *              Used for synaptic management and cleanup.
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get incoming synapse handles
     * @return Const reference to vector of incoming synapse handles
     * @description Returns handles to all synapses that terminate at this neuron.
     *              Used for synaptic management and statistics.
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get outgoing synapse handles
     * @return Const reference to vector of outgoing synapse handles
     * @description Returns handles to all synapses that originate from this neuron.
     *              Used for synaptic management and statistics.
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity flags
     * @return Const reference to plasticity flags structure
     * @description Returns flags indicating which plasticity rules are enabled
     *              for this neuron (Hebbian, STDP, reward-modulated, etc.).
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get plasticity flags (mutable)
     * @return Mutable reference to plasticity flags structure
     * @description Returns mutable reference for directly modifying plasticity eligibility.
     * @note Typically modified through enablePlasticity() method
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity rules
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * @description Configures which plasticity rules are active for this neuron.
     *              Different neurons may have different plasticity profiles.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set region ID for neuron
     * @param region Region identifier
     * @description Assigns the neuron to a specific brain region for organization.
     *              Regions group neurons for local connectivity and management.
     * @see RegionId
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region ID
     * @return Region identifier
     * @description Returns the region to which the neuron belongs.
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population ID for neuron
     * @param population Population identifier
     * @description Assigns the neuron to a specific population within its region.
     *              Populations group neurons with similar properties or functions.
     * @see PopulationId
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population ID
     * @return Population identifier
     * @description Returns the population to which the neuron belongs.
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step
     * @param currentTime Current simulation time in seconds
     * @description Main neuron update method that integrates inputs and updates state.
     *              This is the primary method called by the brain system.
     * @note During development phase, this method is a placeholder
     * @see stepLIF
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     * @description Resets all neuron parameters to initial values as in construction.
     *              Clears spike history, resets membrane potential, and clears currents.
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random parameters
     * @param rng Random number generator reference
     * @description Randomizes neuron parameters such as threshold, resting potential,
     *              and conductance values. Used for initializing neural populations.
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
