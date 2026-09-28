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
 * @brief Complete state representation of a neuron at a given timestep
 * 
 * This structure contains all state variables needed for LIF neuron dynamics.
 * Designed for cache efficiency with all related variables grouped together.
 * 
 * State variables include:
 * - Membrane potential dynamics (integration, threshold, reset)
 * - Synaptic inputs (excitatory, inhibitory, modulatory)
 * - Firing state and history
 * - Refractory period management
 * - Plasticity and adaptation mechanisms
 * 
 * @note Memory layout optimized for single cache line access where possible
 */
struct NeuronState {
    // Membrane potential dynamics
    MembranePotential membranePotential;    ///< Current membrane potential (mV)
    MembranePotential restingPotential;    ///< Resting potential (mV) - baseline when no input
    MembranePotential threshold;           ///< Firing threshold (mV) - spike when exceeded
    MembranePotential resetPotential;       ///< Post-spike reset value (mV) - clamped after spike
    MembranePotential leakConductance;      ///< Leak conductance (nS) - exponential decay rate
    MembranePotential synapseConductance;   ///< Synaptic conductance (nS) - total synaptic input
    
    // Firing characteristics
    FiringRate firingRate;                 ///< Current firing rate (Hz) - derived from spike history
    FiringState firingState;               ///< Current firing state (Resting, Active, Refractory)
    
    // Refractory period management
    uint32_t refractoryRemaining;          ///< Steps remaining in refractory period
    uint32_t refractoryPeriod;             ///< Total refractory period (steps) - absolute refractory time
    
    // Plasticity and adaptation
    float adaptationVariable;              ///< For spike-frequency adaptation - increases with firing
    float lastSpikeTime;                   ///< Timestamp of last spike (-1 if none)
    
    /**
     * @brief Initialize neuron with biologically plausible default values
     * 
     * Sets up standard LIF parameters based on typical neuron characteristics:
     * - Resting potential: -70mV (typical for many neurons)
     * - Threshold: -55mV (relative to resting)
     * - Leak conductance: 10 nS (determines time constant)
     * - Refractory period: 5 steps (relative to timestep)
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
 * @brief Implements a Leaky Integrate-and-Fire (LIF) neuron model
 * 
 * The Neuron class provides a biologically-inspired single-neuron model that
 * forms the foundation of the NLM artificial brain. It implements real LIF dynamics
 * with spike propagation, synaptic integration, and plasticity mechanisms.
 * 
 * Key features:
 * - Real membrane potential dynamics with exponential decay
 * - Event-driven spike generation and propagation
 * - Multiple input types (excitatory, inhibitory, modulatory)
 * - Synaptic connectivity management
 * - Comprehensive plasticity support (Hebbian, STDP, reward-modulated)
 * - Refractory period handling
 * - Activity-dependent adaptation
 * 
 * Neuron types supported:
 * - EXCITATORY: Positive synaptic inputs, standard plasticity
 * - INHIBITORY: Negative synaptic inputs, suppresses firing
 * - SENSORY: External input receptor neurons
 * - INTERNEURON: Interconnected network neurons
 * 
 * The neuron operates on a discrete timestep basis, updating its state
 * according to LIF dynamics at each timestep.
 */
class Neuron {
public:
    /**
     * @brief Constructor - creates a new neuron with specified ID
     * 
     * @param id Unique identifier for this neuron (should be globally unique)
     * @throws std::invalid_argument if id is invalid
     * 
     * @note Memory allocation uses pImpl pattern for efficient implementation
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor - cleans up neuron resources
     */
    ~Neuron();
    
    /**
     * @brief Disable copying - neurons are not copyable
     * 
     * Neurons manage dynamic resources and should not be copied.
     * Use move semantics instead when transferring neuron ownership.
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Enable moving - neurons can be moved efficiently
     * 
     * Move constructor and assignment allow efficient transfer of neuron ownership
     * without deep copying of internal state.
     */
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * 
     * @return NeuronId Unique identifier for this neuron
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron type classification
     * 
     * @return NeuronType The functional type of this neuron
     * 
     * @note Type influences synaptic interactions and plasticity rules
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type classification
     * 
     * @param type New neuron type
     * 
     * @note Changing neuron type may require reconfiguration of plasticity rules
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the current complete state of the neuron
     * 
     * @return const NeuronState& Reference to the neuron's current state
     * 
     * @note Returns a reference to internal state for efficiency
     *       Modifications through this reference affect neuron behavior
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to the neuron's state
     * 
     * @return NeuronState& Mutable reference to the neuron's current state
     * 
     * @note Use with caution - direct state modification bypasses validation
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * 
     * @return MembranePotential Current membrane potential in mV
     * @see setMembranePotential()
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential directly
     * 
     * @param potential New membrane potential in mV
     * 
     * @warning Direct potential manipulation should generally be avoided.
     *          Use injectCurrent() or synaptic input methods instead.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Inject current into the neuron (adds to membrane potential)
     * 
     * @param delta Membrane potential change in mV (positive = excitation)
     * 
     * @note Used for external input stimulation and current injection experiments
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold voltage
     * 
     * @return MembranePotential Firing threshold in mV
     * @see setThreshold()
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold voltage
     * 
     * @param threshold New firing threshold in mV
     * 
     * @note Lowering threshold increases neuronal excitability
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently in firing state
     * 
     * @return bool true if neuron is actively firing, false otherwise
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return bool true if neuron cannot fire (refractory), false otherwise
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the firing state directly
     * 
     * @param state New firing state
     * 
     * @warning Direct state manipulation should generally be avoided.
     *          Use LIF dynamics or public methods instead.
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period duration
     * 
     * @param steps Number of simulation steps in refractory period
     * 
     * @note Affects how long neuron cannot fire after a spike
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter by one timestep
     * 
     * @post If refractoryRemaining becomes 0, neuron may fire again
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate
     * 
     * @return FiringRate Current firing rate in Hz
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set firing rate (rate-based computation mode)
     * 
     * @param rate New firing rate in Hz
     * 
     * @note Used in hybrid rate-LIF models for computational efficiency
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance (determines membrane time constant)
     * 
     * @param conductance Leak conductance in nS
     * 
     * @note Higher conductance = faster membrane potential decay
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
     * @return uint32_t Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential
     * 
     * @param potential Resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential
     * 
     * @return MembranePotential Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential (post-spike reset value)
     * 
     * @param potential Reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get the reset potential
     * 
     * @return MembranePotential Reset potential in mV
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Check if membrane potential exceeds firing threshold
     * 
     * @return bool true if threshold crossed, false otherwise
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the last spike
     * 
     * @return float Time since last spike, or -1.0 if no spikes occurred
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform one timestep of LIF dynamics
     * 
     * @param currentTime Current simulation timestamp
     * @param dt Timestep duration in seconds
     * @return bool true if neuron fired during this timestep, false otherwise
     * 
     * @note Core LIF integration: V(t+dt) = V(t)*exp(-dt/τ) + I*τ
     *       where τ = 1/(gL/C) is the membrane time constant
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input (positive postsynaptic potential)
     * 
     * @param amplitude Membrane potential change in mV (positive)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input (negative postsynaptic potential)
     * 
     * @param amplitude Membrane potential change in mV (negative)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input (neuromodulatory effect)
     * 
     * @param amplitude Modulatory effect magnitude (can be positive or negative)
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current (from outside the network)
     * 
     * @param current External current input in mV
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total current affecting this neuron
     * 
     * @return MembranePotential Sum of all current sources
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear all external injected current
     * 
     * @post getTotalCurrent() returns 0 (synaptic inputs remain)
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike timestamp for plasticity calculations
     * 
     * @param timestamp Time of spike occurrence
     * 
     * @note Used by STDP and other spike-timing dependent plasticity rules
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the complete spike history for this neuron
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear the spike history
     * 
     * @post getSpikeHistory() returns empty vector
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse connection
     * 
     * @param handle Handle to the incoming synapse
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse connection
     * 
     * @param handle Handle to the outgoing synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapse connections
     * 
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapse connections
     * 
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the plasticity flags for this neuron
     * 
     * @return const PlasticityFlags& Current plasticity settings
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to plasticity flags
     * 
     * @return PlasticityFlags& Mutable reference to plasticity settings
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Configure plasticity mechanisms for this neuron
     * 
     * @param hebbian Enable Hebbian synaptic strengthening
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * @note Multiple plasticity mechanisms can be enabled simultaneously
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the region ID this neuron belongs to
     * 
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the region ID this neuron belongs to
     * 
     * @return RegionId Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the population ID this neuron belongs to
     * 
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the population ID this neuron belongs to
     * 
     * @return PopulationId Population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Perform one complete simulation timestep
     * 
     * @param currentTime Current simulation timestamp
     * 
     * @note Currently calls stepLIF() - placeholder for future enhanced dynamics
     *       TODO PHASE 2: Implement real integrate-and-fire dynamics
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     * 
     * @post All state variables restored to initial values
     *       membrane potential = resting potential
     *       firing rate = 0, no spike history
     *       not in firing or refractory state
     */
    void reset();
    
    /**
     * @brief Initialize neuron with biologically plausible random parameters
     * 
     * @param rng Random number generator reference
     * 
     * @note Sets random values for neuron parameters while maintaining
     *       biologically plausible ranges and correlations
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Opaque implementation pointer for pImpl pattern
     * 
     * @note All actual implementation details hidden in private struct
     *       Provides efficient memory usage and implementation flexibility
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm