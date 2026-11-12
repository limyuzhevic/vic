#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

/**
 * @brief Neuron state structure for efficient storage
 * 
 * This structure contains all state variables for a single LIF neuron,
 * organized for cache efficiency and optimized memory layout. Each neuron
 * maintains its own state independently for parallel computation.
 */
struct NeuronState {
    MembranePotential membranePotential;    /**< @brief Current membrane potential (mV) */
    MembranePotential restingPotential;    /**< @brief Resting potential (mV) */
    MembranePotential threshold;           /**< @brief Firing threshold (mV) */
    MembranePotential resetPotential;       /**< @brief Post-spike reset value (mV) */
    MembranePotential leakConductance;      /**< @brief Leak conductance (nS) */
    MembranePotential synapseConductance;   /**< @brief Synaptic conductance (nS) */
    FiringRate firingRate;                 /**< @brief Current firing rate (Hz) */
    FiringState firingState;               /**< @brief Current firing state */
    uint32_t refractoryRemaining;          /**< @brief Steps remaining in refractory period */
    uint32_t refractoryPeriod;             /**< @brief Total refractory period (steps) */
    float adaptationVariable;              /**< @brief For spike-frequency adaptation */
    float lastSpikeTime;                   /**< @brief Timestamp of last spike (-1 if none) */
    
    /**
     * @brief Default constructor
     * Initializes neuron state with biologically realistic parameters.
     * The refractory period is set to 9 steps based on Phase 6 findings:
     * - Shorter refractory periods let neurons fire multiple times in a single
     *   action presentation (10 brain steps per action)
     * - 9 steps caps at about one spike per presentation, matching the
     *   population's natural burst rhythm
     * - This reduces reported firing rates without changing drive constants
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
        , refractoryPeriod(9)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

/**
 * @brief Neuron class representing a single spiking neuron
 * 
 * Implements the Leaky Integrate-and-Fire (LIF) model, the most widely used
 * biophysical neuron model in computational neuroscience. This class provides
 * the fundamental building block for neural computation in the NLM framework.
 * 
 * The neuron maintains detailed state including membrane potential, synaptic
 * inputs, firing history, and plasticity flags. It supports event-driven spike
 * generation with precise timing for spike-timing-dependent plasticity.
 */
class Neuron {
public:
    /**
     * @brief Create neuron with ID
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor
     */
    ~Neuron();
    
    /**
     * @brief Disable copying (efficient memory management)
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Enable move construction
     * Allows efficient transfer of neuron ownership.
     */
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get neuron's unique identifier
     * @return Neuron ID
     */
    NeuronId getId() const;
    
    /**
     * @brief Get neuron type
     * @return Neuron type (sensory, motor, inhibitory, etc.)
     */
    NeuronType getType() const;
    void setType(NeuronType type);
    
    /**
     * @brief Get complete neuron state
     * @return Constant reference to neuron state structure
     */
    const NeuronState& getState() const;
    NeuronState& getState();
    
    /**
     * @brief Get membrane potential
     * @return Current membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get firing threshold
     * @return Membrane potential threshold for spike generation
     */
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Get firing state
     * @return Current firing state (Resting, Active, Refractory, etc.)
     */
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    /**
     * @brief Get firing rate
     * @return Current firing rate in Hz
     */
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set leak conductance
     * @param conductance Leak conductance in nS
     */
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if neuron has reached firing threshold
     * @return true if membrane potential >= threshold
     */
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    /**
     * @brief LIF neuron step function
     * 
     * Implements the discrete-time Leaky Integrate-and-Fire dynamics:
     * dV/dt = (V_rest - V)/tau + I/C
     * 
     * @param currentTime Current simulation time in seconds
     * @param dt Timestep duration in seconds
     * @return true if neuron fired during this timestep
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * Adds to membrane potential through conductance-based transmission.
     * @param amplitude Synaptic current amplitude
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * Subtracts from membrane potential through hyperpolarization.
     * @param amplitude Synaptic current amplitude
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * Affects plasticity but not directly integrated into membrane potential.
     * @param amplitude Modulatory input amplitude
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current into neuron
     * Used for sensory input or external stimulation.
     * @param current External current in nA
     */
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    /**
     * @brief Record spike occurrence
     * Maintains spike history for plasticity computations.
     * @param timestamp Time when spike occurred
     */
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse reference
     * Maintains connectivity information for synaptic transmission.
     * @param handle Synapse handle
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse reference
     * Maintains connectivity information for synaptic transmission.
     * @param handle Synapse handle
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get incoming synapse handles
     * @return Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get outgoing synapse handles
     * @return Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity flags
     * @return Reference to plasticity state flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Configure plasticity mechanisms
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set neuron region membership
     * Regions group neurons for efficient computation and organization.
     * @param region Region ID
     */
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    
    /**
     * @brief Set neuron population membership
     * Populations group neurons by type for efficient processing.
     * @param population Population ID
     */
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step
     * Default LIF step with standard timestep (1ms).
     * TODO: Implement real integrate-and-fire dynamics with configurable timestep
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     * Used for simulation restart and initialization.
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random biological parameters
     * Sets realistic membrane properties based on physiological constraints.
     * @param rng Random number generator for parameter variation
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Private implementation (pImpl pattern)
     * Forward declaration for efficient memory management and encapsulation.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
