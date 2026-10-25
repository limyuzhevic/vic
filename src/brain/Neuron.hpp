#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// ============================================================================
// NEURON CLASS DOCUMENTATION
// ============================================================================

/**
 * @class NeuronState
 * @brief Stores the complete state of a single neuron for efficient access
 * 
 * This structure contains all dynamic and static parameters of a neuron
 * in a single contiguous memory block for optimal performance.
 * 
 * All values are in SI units (mV, nS, Hz, steps) except where noted.
 * 
 * @note This is a POD (Plain Old Data) structure for memory efficiency.
 * Use NeuronState& getState() to modify state, const NeuronState& getState() for read-only access.
 */
struct NeuronState {
    MembranePotential membranePotential;    /**< @brief Current membrane potential (mV) */
    MembranePotential restingPotential;    /**< @brief Resting potential (mV) - equilibrium state */
    MembranePotential threshold;           /**< @brief Firing threshold (mV) - spike initiation point */
    MembranePotential resetPotential;       /**< @brief Post-spike reset value (mV) */
    MembranePotential leakConductance;      /**< @brief Leak conductance (nS) - natural decay rate */
    MembranePotential synapseConductance;     /**< @brief Synaptic conductance (nS) - total synaptic input */
    FiringRate firingRate;                  /**< @brief Current firing rate (Hz) - derived from spike history */
    FiringState firingState;                /**< @brief Current firing state (Resting, Active, Refractory, Inhibited) */
    uint32_t refractoryRemaining;          /**< @brief Steps remaining in refractory period */
    uint32_t refractoryPeriod;             /**< @brief Total refractory period (steps) - minimum interspike interval */
    float adaptationVariable;                 /**< @brief For spike-frequency adaptation - calcium dynamics */
    float lastSpikeTime;                     /**< @brief Timestamp of last spike (-1 if none) */
    
    /**
struct Neuron constructor with standard initialization
     * 
     * Initializes neuron with biologically plausible values:
     * - Resting potential: -70mV (typical for cortical neurons)
     * - Threshold: -55mV (standard spike initiation)
     * - Reset potential: -70mV (back to resting)
     * - Leak conductance: 10nS (moderate decay)
     * - Refractory period: 5 steps (prevents immediate re-firing)
     */
     NeuronState()
         : membranePotential(nlm::NEURON_RESTING_POTENTIAL)
         , restingPotential(nlm::NEURON_RESTING_POTENTIAL)
         , threshold(nlm::NEURON_FIRING_THRESHOLD)
         , resetPotential(nlm::NEURON_RESTING_POTENTIAL)
         , leakConductance(nlm::NEURON_LEAK_CONDUCTANCE)
         , synapseConductance(0.0f)
         , firingRate(0.0f)
         , firingState(FiringState::Renting)
         , refractoryRemaining(0)
         , refractoryPeriod(nlm::NEURON_REFECRORY_PERID)
         , adaptationVariable(0.0f)
         , lastSpikeTime(-1.0f) {}
};
};

/**
 * @class Neuron
 * @brief Represents a single neuron implementing Leaky Integrate-and-Fire (LIF) dynamics
 * 
 * 
 * The Neuron class implements the classic LIFN model with spike detection and synaptic integration.
 * It serves as the fundamental computational unit of the NLM brain simulation.
 * 
 * Key features:
 * - Single-compartment model with exponential decay
 * - Spike generation when threshold is crossed
 * - Synaptic input integration (excitatory, inhibitory, modulatory)
 * - Event-driven spike propagation
 * - Plasticity state management for synaptic learning
 * - Refractory period to prevent unphysical firing rates
 * - Spike-frequency adaptation through adaptation variable
 * 
 * @note Uses pImpl (pointer to implementation) pattern for efficient memory management
 * and forward declaration of heavy dependencies.
 * 
 * @see https://en.wikipedia.org/wiki/Leaky_integrate-and-fires_action_potential_model
 * @see nlm::NeuronState - complete neuron state representation
 */
class Neuron {
public:
    /**
     * @brief Create neuron with unique identifier
     * 
     * @param[in] id Unique neuron identifier
     * @note Neuron takes ownership of its ID and manages it internally.
     */
    explicit Neuron(NeuronId id);
    
     ~Neuron();
     
     // Disable copying, enable moving
     Neuron(const Neuron&) = delete;
     Neuron& operator=(const Neuron&) = delete;
     Neuron(Neuron&&) noexcept;
     Neuron& operator=(Neuron&&) noexcept;
     
     // ============================================================================
     // IDENTITY AND METADATA
     * ============================================================================
     
     /**
     * @brief Get neuron's unique identifier
     * 
     * @return NeuronId Unique identifier for this neuron
     * @note IDs are guaranteed to be unique within a brain simulation.
     */
     NeuronId getId() const;
     
     /**
     * @brief Get neuron's type classification
     * 
     * @return NeuronType Type of neuron (excitatory, inhibitory, sensory, motor, etc.)
     * @see NeuronType enum in Types.hpp
     */
    NeuronType getType() const;
    void setType(NeuronType type);
     
     // ============================================================================
     // STATE ACCESS AND MODIFICATION
     * ============================================================================
     
     /**
     * @brief Get complete neuron state (read-only)
     * 
     * @return const NeuronState& Complete neuron state
     * @note Returns reference to internal state - do not copy.
     */
     const NeuronState& getState() const;
    NeuronState& getState();
    
     /**
     * @brief Get membrane potential
     * 
     * @return MembranePotential Current membrane potential (mV)
     */
     MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
     
     /**
     * @brief Get firing threshold
     * 
     * @return MembranePotential Firing threshold (mV)
     */
     MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
     /**
     * @brief Check if neuron has crossed firing threshold
     * 
     * @return bool true if neuron is firing, false otherwise
     */
     bool checkThreshold() const;
    float getLastSpikeTime() const;
    
     // ============================================================================
     // FIRING STATE MANAGEMENT
     * ============================================================================
     
     /**
     * @brief Check if neuron is currently firing
     * 
     * @return bool true if neuron has spiked in current timestep
     */
     bool isFiring() const;
    
     /**
     * @brief Check if neuron is in refactory period
     * 
     * @return bool true if neuron cannot spike due to refactory
     */
     bool isRefactory() const;
     
     /**
     * @brief Set firing state directly
     * 
     * @param[in] state New firing state
     * @note Typically managed internally by spike detection.
     */
    void setFiringState(FiringState state);
    void setRefactoryPeriod(uint32_t steps);
    void decrementRefactory();
    
    // ============================================================================
     // FIRING RATE AND DYNAMICS
     * ============================================================================
     
     /**
     * @brief Get current firing rate
     * 
     * @return FiringRate Firing rate in Hz
     * @note Derived from spike history - not actual instantaneous rate.
     */
     FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
     // ============================================================================
     // LIF NEURON PRAMETERS
     * ============================================================================
     
     /**
     * @brief Set leak conductance
     * 
     * @param[in] conductance Leak conductance (nS)
     * @note Controls exponential decay toward resting potential.
     */
     void setLeakConductance(MembranePotential conductance);
    
     /**
     * @brief Get leak conductance
     * 
     * @return MembranePotential Leak conductance (nS)
     */
     MembranePotential getLeakConductance() const;
    uint32_t getRefactoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // ============================================================================
     // SPIKE DETECTION AND DYNAMICS
     * ============================================================================
     
     /**
     * @brief Main LIFN step function - integrates dynamics and detects spikes
     * 
     * Performs one timestep of LIFN integration:
     * 1. Updates membrane potential based on incoming currents
     * 2. Applies exponential leak toward resting potential
     * 3. Checks for threshold crossing
     * 4. Generates spike if threshold crossed
     * 5. Manages refactory period and adaptation variable
     * 
     * @param[in] currentTime Current simulation time (seconds)
     * @param[in] dt Time step size (seconds)
     * @return bool true if neuron spiked this timestep
     * @note This is the core computation - most other functions are helpers.
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
     // ============================================================================
     // SYNAPTIC INPUT HANDLING
     * ============================================================================
     
     /**
     * @brief Inject excitatory post-synaptic potential
     * 
     * @param[in] amplitude Excitatory input strength (mV)
     * @note Adds to membrane potential - subtracts from inhibitory.
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
     // ============================================================================
     // EXTERNAL CURRENT INJECTION
     * ============================================================================
     
     /**
     * @brief Inject current from external sources (sensory, motor commands)
     * 
     * @param[in] current External current (mV)
     * @note External current adds to total current for integration.
     */
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
     // ============================================================================
     // SPIKE HISTORY AND PLASTICITY
     * ============================================================================
     
     /**
     * @brief Record spike occurrence
     * 
     * @param[in] timestamp Time of spike occurrence
     * @note Updates spike history for STDP and other temporal plasticities.
     */
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
     // ============================================================================
     // SYNAPSE MA NAGEMEN T
     * ============================================================================
     
     /**
     * @brief Add incoming synaptic connection
     * 
     * @param[in] handle Synaptic connection handle
     * @note Connection is added to incoming synapse list.
     */
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
     // ============================================================================
     // PLASTICITY STATE MANAGEMENT
     * ============================================================================
     
     /**
     * @brief Get plasticity flags
     * 
     * @return const PlasticityFlags& Plasticity configuration flags
     * @note Read-only access - use getPlasticityFlags() for modification.
     */
     const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
     /**
     * @brief Enable plasticity mechanisms
     * 
     * @param[in] hebbian Enable Hebbian plasticity
     * @param[in] stdp Enable STDP plasticity
     * @param[in] rewardModulated Enable reward-modulated plasticity
     * @note Each plasticity mechanism independently can be enabled/disabled.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
     // ============================================================================
     // REGION AND POPULATION MEMBERSHIP
     * ============================================================================
     
     /**
     * @brief Set neural region identifier
     * 
     * @param[in] region Region ID
     * @note Regions organize neurons for spatial topology.
     */
     void setRegionId(RegionId region);
    
     RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
     // ============================================================================
     // SIMULATION STEP FUNCTION
     * ============================================================================
     
     /**
     * @brief Update neuron for one complete simulation step
     * 
     * This is the main neuron update function called from Brain::step().
     * It currently delegates to stepLIFN() but can be expanded with
     * additional dynamics in future phases.
     * 
     * @param[in] currentTime Current simulation time (seconds)
     * @note TODO PHASE 2: Implement real integrate-and-fire dynamics.
     */
     void step(Timestamp currentTime);
    
     // ============================================================================
     // STATE RESET AND INITIALIZATION
     * ============================================================================
     
     /**
     * @brief Reset neuron to initial state
     * 
     * Restores all internal state variables to initial values:
     * - Membrane potential to resting
     * - Spike history cleared
     * - Refactory period reset
     * - Plasticity traces reset
     */
     void reset();
     
     /**
     * @brief Initialize neuron with biologically realistic random parameters
     * 
     * Sets neuron parameters using random number generator with
     * realistic biological ranges and correlations.
     * 
     * @param[in] rng Random number generator for parameter variation
     * @note Used during brain initialization for biological variability.
     */
     void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Private implementation structure for neuron
     * 
     * Contains all implementation details to keep header file small.
     * All actual data storage and computation happens here.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
