#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @brief Neuron state structure for efficient storage
 * 
 * This structure holds all the state variables needed to simulate a single
 * Leaky Integrate-and-Fire (LIF) neuron. The structure is designed for
 * memory efficiency with fixed-size arrays and minimal padding.
 * 
 * The LIF neuron model is a simplified but biologically plausible model
 * of neuronal dynamics. It captures the essential features of how neurons
 * integrate synaptic inputs and generate action potentials (spikes).
 * 
 * @note All electrical potentials are in millivolts (mV)
 * @note Time is in milliseconds (ms)
 * @note Conductances are in nanosiemens (nS)
 * @note Firing rates are in hertz (Hz)
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
     * @brief Default constructor initializes neuron with biological parameters
     * 
     * Sets typical resting state values for a cortical neuron:
     * - Resting potential: -70 mV (typical for cortical neurons)
     * - Threshold: -55 mV (typical action potential threshold)
     * - Reset potential: -70 mV (return to resting after spike)
     * - Refractory period: 5 steps (1ms at 1kHz timestep)
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
 * @brief Neuron class representing a single neuron
 * 
 * This class implements a Leaky Integrate-and-Fire (LIF) neuron, which is
 * the fundamental computational unit of the NLM system. LIF neurons are
 * biologically inspired models that capture the essential dynamics of
 * how real neurons integrate synaptic inputs and generate spikes.
 * 
 * The LIF model is characterized by:
 * - Continuous membrane potential that integrates synaptic currents
 * - Exponential decay towards resting potential
 * - Threshold-based spike generation
 * - Absolute refractory period after spikes
 * - Spike-frequency adaptation
 * 
 * This implementation supports:
 * - Multiple neuron types (excitatory, inhibitory, modulatory, etc.)
 * - Synapse integration (excitatory, inhibitory, modulatory inputs)
 * - Plasticity mechanisms (STDP, Hebbian, reward-modulated)
 * - Development stages (plasticity changes over time)
 * - Regional organization (neurons belong to brain regions)
 * 
 * The neuron is implemented using the Pimpl idiom for performance,
 * containing a private implementation structure that manages all internal
 * state and computations.
 * 
 * @see See "src/brain/Neuron.cpp" for detailed implementation and
 *      mathematical formulations.
 */
class Neuron {
public:
    /**
     * @brief Create neuron with unique identifier
     * 
     * @param id Unique identifier for this neuron
     * @note The neuron starts with default type (Internal) and invalid
     *       region/population IDs. Call initializeRandom() or set appropriate
     *       parameters before use.
     */
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    /**
     * @brief Disable copying for performance reasons
     * 
     * Neurons should be created using move operations or the factory functions
     * provided by the NLM system.
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor for efficient neuron transfer
     * 
     * Transfer ownership of the implementation structure from another neuron
     * to this neuron. The source neuron is left in a valid but empty state.
     * 
     * @param other Neuron to move from (will be in valid empty state after)
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator for efficient neuron transfer
     * 
     * Transfer ownership of the implementation structure from another neuron
     * to this neuron. The source neuron is left in a valid but empty state.
     * 
     * @param other Neuron to move from (will be in valid empty state after)
     * @return Reference to this neuron for method chaining
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * 
     * @return NeuronId This neuron's identifier
     * @note The identifier remains constant for the lifetime of the neuron
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the current neuron type
     * 
     * @return NeuronType This neuron's type (Excitatory, Inhibitory, etc.)
     * @note Neuron type determines the sign of synaptic effects
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type
     * 
     * @param type New neuron type
     * @note Changing neuron type affects how synaptic inputs are processed
     *       and may affect plasticity rules
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the complete neuron state structure
     * 
     * @return const NeuronState& Reference to the neuron's state
     * @note This provides access to all internal state variables for
     *       analysis and debugging purposes
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to neuron state
     * 
     * @return NeuronState& Mutable reference to the neuron's state
     * @note This allows direct modification of neuron state - use with caution
     *       as it may violate the neuron's internal invariants
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * 
     * @return MembranePotential Current membrane potential in mV
     * @note Range: Typically -100mV to 50mV (clamped for stability)
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential directly
     * 
     * @param potential New membrane potential in mV
     * @note This is a low-level operation that bypasses integration.
     *       Prefer using synaptic inputs or injectCurrent() for normal operation.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to membrane potential
     * 
     * @param delta Change in membrane potential in mV
     * @note Convenience method for small potential adjustments
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold
     * 
     * @return MembranePotential Threshold voltage in mV
     * @note When membrane potential reaches this value, neuron fires
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold
     * 
     * @param threshold New threshold voltage in mV
     * @note Lower threshold values make neuron more likely to fire
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * 
     * @return bool True if neuron is in Active firing state
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return bool True if neuron cannot fire this step
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the firing state
     * 
     * @param state New firing state (Resting, Active, Refractory, Inhibited)
     * @note This is a low-level method. Use synaptic inputs for normal control.
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period duration
     * 
     * @param steps Number of simulation steps the neuron must wait before firing
     * @note Typical values: 2-10 steps (2-10ms at 1kHz timestep)
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter (called automatically)
     * 
     * @note This method is called automatically during simulation steps.
     *       Users typically don't need to call this directly.
     */
    void decrementRefractory();
    
    /**
     * @brief Get the firing rate
     * 
     * @return FiringRate Current firing rate in Hz
     * @note This is a rate-based measure derived from spike history
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate
     * 
     * @param rate New firing rate in Hz
     * @note This is a high-level parameter for rate-based control
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance
     * 
     * @param conductance Leak conductance in nS
     * @note Higher values make neuron return to resting potential faster
     *       Controls the time constant tau = C/g
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the leak conductance
     * 
     * @return MembranePotential Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period duration
     * 
     * @return uint32_t Total refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential
     * 
     * @param potential Resting potential in mV
     * @note Also known as the leak reversal potential or Nernst potential
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential
     * 
     * @return MembranePotential Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential
     * 
     * @param potential Post-spike reset potential in mV
     * @note Typically equal to or close to resting potential
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get the reset potential
     * 
     * @return MembranePotential Reset potential in mV
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Check if neuron has exceeded firing threshold
     * 
     * @return bool True if membrane potential >= threshold
     * @note This is the main spike detection mechanism
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the last spike
     * 
     * @return float Timestamp of last spike in ms, or -1 if never spiked
     * @note Used for spike timing-dependent plasticity (STDP)
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform one LIF simulation step
     * 
     * @param currentTime Current simulation time in ms
     * @param dt Time step duration in ms
     * @return bool True if neuron fired (spiked) this step
     * @note This is the core computation method that implements the
     *       Leaky Integrate-and-Fire dynamics.
     *       
     * @note See Neuron.cpp stepLIF() for detailed implementation of the
     *       mathematical formulation:
     *       - 
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * @param amplitude Synaptic input amplitude in mV
     * @note Excitatory inputs increase membrane potential towards threshold
     *       Represents AMPA-type synapses or similar excitatory mechanisms
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * @param amplitude Synaptic input amplitude in mV
     * @note Inhibitory inputs decrease membrane potential away from threshold
     *       Represents GABA-type synapses or similar inhibitory mechanisms
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * @param amplitude Synaptic input amplitude in mV
     * @note Modulatory inputs affect plasticity but not directly integrated
     *       Represents neuromodulators like dopamine, acetylcholine, etc.
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject current from external source
     * 
     * @param current Current amplitude in nA
     * @note Used for sensory inputs or external driving signals
     *       Current is converted to membrane potential change via C (capacitance)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total accumulated synaptic current
     * 
     * @return MembranePotential Total synaptic current in mV
     * @note This is the sum of all recent synaptic inputs
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear all accumulated synaptic current
     * 
     * @note Called automatically at the end of each simulation step
     *       User typically doesn't need to call this directly
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike event
     * 
     * @param timestamp Time when spike occurred
     * @note Records spike for STDP learning and spike history analysis
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the complete spike history
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     * @note Spike history is used for spike-timing-dependent plasticity (STDP)
     *       and rate-based computations
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * 
     * @note Used to reset learning history or change neuron type
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse handle
     * 
     * @param handle Handle to incoming synapse
     * @note Used for building synaptic connectivity
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse handle
     * 
     * @param handle Handle to outgoing synapse
     * @note Used for building synaptic connectivity
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapse handles
     * 
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapse handles
     * 
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the plasticity flags
     * 
     * @return const PlasticityFlags& Reference to plasticity flags
     * @note Flags determine which plasticity mechanisms are enabled
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to plasticity flags
     * 
     * @return PlasticityFlags& Mutable reference to plasticity flags
     * @note Allows modification of plasticity mechanisms
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable plasticity mechanisms
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * @note Typical configuration: hebbian=true, stdp=true, rewardModulated=false
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the region ID
     * 
     * @param region Region identifier
     * @note Neurons belong to brain regions for organizational purposes
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the region ID
     * 
     * @return RegionId Region identifier, or INVALID_REGION_ID if not set
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the population ID
     * 
     * @param population Population identifier
     * @note Populations group neurons with similar properties or functions
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the population ID
     * 
     * @return PopulationId Population identifier, or INVALID_POPULATION_ID if not set
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step
     * 
     * @param currentTime Current simulation time in ms
     * @note Default implementation with 1ms timestep. For custom timesteps,
     *       call stepLIF() directly or override this method in a derived class.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     * 
     * @note Resets membrane potential, spike history, and internal counters.
     *       Does not reset neuron ID, type, or connectivity.
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random parameters
     * 
     * @param rng Random number generator reference
     * @note Sets biologically plausible random parameters for neuron properties.
     *       Use this after creating neuron but before simulation.
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm