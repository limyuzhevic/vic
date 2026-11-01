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

// Neuron class representing a single neuron
/**
 * @class Neuron
 * @brief Leaky Integrate-and-Fire (LIF) neuron implementation
 * 
 * The Neuron class implements a biologically-inspired Leaky Integrate-and-Fire
 * neuron model with real synaptic transmission, spike-frequency adaptation,
 * and refractory period dynamics. This is a core component of the NLM framework
 * for neural simulation and computation.
 * 
 * Key features:
 * - LIF dynamics with exponential Euler integration
 * - Synaptic input integration (excitatory, inhibitory, modulatory)
 * - Spike-frequency adaptation via adaptation variable
 * - Absolute refractory period after spiking
 * - Spike history tracking for STDP and other plasticity rules
 * - Random initialization with biological constraints
 * 
 * The neuron operates in discrete time steps, updating its membrane potential
 * based on synaptic inputs, leak currents, and internal dynamics.
 * 
 * @note Uses Pimpl pattern for efficient implementation hiding
 * @note Thread-safe for individual neuron operations
 * @see Synapse, PlasticityRule
 */
class Neuron {
public:
    /**
     * @brief Constructor
     * @param id Unique identifier for this neuron
     * @details Creates a new neuron with specified ID and initializes default state.
     * Default type is NeuronType::Internal, other parameters are set to physiological defaults.
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor
     * @details Cleans up neuron resources
     */
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get neuron identifier
     * @return NeuronId Unique identifier for this neuron
     */
    NeuronId getId() const;
    
    /**
     * @brief Get neuron type
     * @return NeuronType Type classification of this neuron (Internal, Sensory, Motor, etc.)
     */
    NeuronType getType() const;
    
    /**
     * @brief Set neuron type
     * @param type New type classification for this neuron
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get current neuron state
     * @return const NeuronState& Complete state snapshot including membrane potential,
     * firing rate, refractory status, etc.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable neuron state reference
     * @return NeuronState& Mutable state reference for direct modification
     * @warning Direct modification should be used with caution
     */
    NeuronState& getState();
    
    /**
     * @brief Get membrane potential
     * @return MembranePotential Current membrane potential in millivolts (mV)
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential
     * @param potential New membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to membrane potential
     * @param delta Change in membrane potential in mV
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get firing threshold
     * @return MembranePotential Voltage threshold for spike initiation in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set firing threshold
     * @param threshold New threshold voltage in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * @return bool true if neuron has crossed threshold and is in Active state
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * @return bool true if neuron cannot spike due to recent activity
     */
    bool isRefractory() const;
    
    /**
     * @brief Set firing state
     * @param state New firing state (Resting, Active, Refractory)
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period
     * @param steps Number of simulation steps to spend in refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter
     * @details Called by simulation each step to track remaining refractory time
     */
    void decrementRefractory();
    
    /**
     * @brief Get current firing rate
     * @return FiringRate Current firing rate in Hertz (spikes per second)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set firing rate
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set leak conductance
     * @param conductance Leak conductance in nanoSiemens (nS)
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get leak conductance
     * @return MembranePotential Current leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get refractory period
     * @return uint32_t Total refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set resting potential
     * @param potential Resting membrane potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get resting potential
     * @return MembranePotential Resting membrane potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set reset potential
     * @param potential Post-spike reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if membrane potential crossed threshold
     * @return bool true if threshold would be crossed (pre-spike)
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get last spike time
     * @return float Timestamp of last spike in simulation time, -1.0 if never spiked
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform LIF integration step
     * @details Main simulation step implementing Leaky Integrate-and-Fire dynamics.
     * Updates membrane potential based on synaptic inputs, leak currents, and
     * adaptation mechanisms. Detects spikes and applies reset/refractory dynamics.
     * 
     * @param currentTime Current simulation time in milliseconds
     * @param dt Time step duration in milliseconds
     * @return bool true if neuron spiked during this step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * @param amplitude Synaptic current amplitude in nS*mV
     * @details Adds to membrane potential via Ohmic integration
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * @param amplitude Synaptic current amplitude in nS*mV
     * @details Subtracts from membrane potential, causing hyperpolarization
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * @param amplitude Modulatory input amplitude
     * @details Affects plasticity and adaptation variables without direct integration
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current
     * @param current External current in nS*mV
     * @details Simulates input from sensory receptors or external sources
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total current
     * @return MembranePotential Total current input this simulation step
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear total current accumulator
     * @details Resets synaptic input for next simulation step
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record spike for plasticity and STDP
     * @param timestamp Simulation time when spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get spike history
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     * @see recordSpike, clearSpikeHistory
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * @details Removes all spike records, typically called during reset
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse connection
     * @param handle Synapse handle for incoming connection
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse connection
     * @param handle Synapse handle for outgoing connection
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get incoming synapse handles
     * @return const std::vector<SynapseHandle>& Handles of all incoming synapses
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get outgoing synapse handles
     * @return const std::vector<SynapseHandle>& Handles of all outgoing synapses
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity flags
     * @return const PlasticityFlags& Current plasticity configuration
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity flags
     * @return PlasticityFlags& Mutable plasticity configuration
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity mechanisms
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set region membership
     * @param region Region identifier this neuron belongs to
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region identifier
     * @return RegionId Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population identifier
     * @param population Population identifier within region
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population identifier
     * @return PopulationId Population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step
     * @details Main simulation step. Default implementation uses stepLIF with
     * 1ms timestep. Should be overridden for variable timestep simulations.
     * @param currentTime Current simulation time
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     * @details Resets all state variables to physiological defaults and clears
     * spike history and synaptic input. Does not reset synaptic weights or plasticity.
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random parameters
     * @param rng Random number generator for biologically-constrained initialization
     * @details Sets up neuron parameters with realistic variations around typical values.
     * Membrane potential near resting, threshold with small variation, refractory period
     * in typical range (2-10ms), etc.
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
