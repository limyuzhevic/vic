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
 * This structure contains all state variables needed to represent a single
 * Leaky Integrate-and-Fire (LIF) neuron. The structure is designed for
 * cache efficiency and minimal padding.
 */
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
    float adaptationVariable;              // For spike-frequency adaptation (mV)
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    // Constructor initializes neuron to physiological resting state
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
 * This class implements a biologically-inspired Leaky Integrate-and-Fire (LIF)
 * neuron model with the following features:
 * 
 * - **Subthreshold dynamics**: Exponential integration of membrane potential
 * - **Spike generation**: Threshold crossing and refractory period
 * - **Synaptic integration**: Excitatory and inhibitory inputs
 * - **Plasticity flags**: Support for STDP and Hebbian learning
 * - **Activity tracking**: Spike history and refractory periods
 * - **Neuromodulation**: Adaptation variables for spike-frequency adaptation
 * - **Structural information**: Region and population membership
 * 
 * The neuron operates in discrete time steps with a fixed timestep (typically
 * 1ms). Each step updates the membrane potential based on LIF dynamics and
 * processes any incoming synaptic inputs.
 * 
 * @note Uses PIMPL pattern to hide implementation details and reduce compile
 *       times. All member functions are in Neuron.cpp.
 */
class Neuron {
public:
    /**
     * @brief Create neuron with unique identifier
     * 
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    /**
     * @brief Disable copying (PIMPL pattern)
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Enable move operations (PIMPL pattern)
     */
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the neuron's unique identifier
     * 
     * @return NeuronId Unique identifier
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's type (sensory, internal, motor, etc.)
     * 
     * @return NeuronType Type of neuron
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's type
     * 
     * @param type New neuron type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get a const reference to the neuron's state
     * 
     * @return const NeuronState& Complete neuron state
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get a mutable reference to the neuron's state
     * 
     * @return NeuronState& Complete neuron state (for internal modification)
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * 
     * @return MembranePotential Current membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential directly
     * 
     * @param potential New membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to the membrane potential (for synaptic input)
     * 
     * @param delta Change in membrane potential in mV
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold
     * 
     * @return MembranePotential Threshold voltage in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold
     * 
     * @param threshold New threshold voltage in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * 
     * @return bool True if neuron has just crossed threshold
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return bool True if neuron cannot fire due to refractory period
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the firing state of the neuron
     * 
     * @param state New firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period duration
     * 
     * @param steps Number of simulation steps in refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory period counter
     * 
     * @note Called automatically by stepLIF() each simulation step
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate
     * 
     * @return FiringRate Current firing rate in Hz
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate (for rate-based modeling)
     * 
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Get the leak conductance
     * 
     * @return MembranePotential Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Set the leak conductance
     * 
     * @param conductance Leak conductance in nS
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the refractory period duration
     * 
     * @return uint32_t Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Get the resting potential
     * 
     * @return MembranePotential Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the resting potential
     * 
     * @param potential Resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the reset potential (post-spike value)
     * 
     * @return MembranePotential Reset potential in mV
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Set the reset potential
     * 
     * @param potential Reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if neuron has reached firing threshold
     * 
     * @return bool True if membrane potential >= threshold
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the time of last spike
     * 
     * @return float Time of last spike, or -1 if never spiked
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Apply LIF neuron dynamics for one simulation step
     * 
     * This is the core neuron computation function that implements:
     * 1. Refractory period handling
     * 2. Membrane potential integration (LIF dynamics)
     * 3. Spike detection and reset
     * 4. Spike-frequency adaptation
     * 5. Return whether neuron fired
     * 
     * @param currentTime Current simulation time in ms
     * @param dt Time step in ms
     * @return bool True if neuron fired during this step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Apply standard LIF step with default timestep (1ms)
     * 
     * Convenience wrapper around stepLIF() with dt=0.001ms (1ms).
     * 
     * @param currentTime Current simulation time in ms
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * This adds current to the neuron's synaptic input accumulator, which
     * is integrated into membrane potential during the next LIF update.
     * 
     * @param amplitude Synaptic current amplitude (nS * mV)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * Inhibitory inputs hyperpolarize the neuron by subtracting from the
     * total synaptic current accumulator.
     * 
     * @param amplitude Inhibitory current amplitude (nS * mV)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * Modulatory inputs (e.g., dopamine, acetylcholine) affect plasticity
     * rather than directly changing membrane potential. They accumulate
     * to influence learning rules.
     * 
     * @param amplitude Modulatory input amplitude
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject current directly (e.g., from sensory input)
     * 
     * Direct current injection bypasses synaptic mechanisms and directly
     * contributes to membrane potential change. Used for external inputs
     * or sensory-driven activity.
     * 
     * @param current Current to inject (nS * mV)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total accumulated synaptic input
     * 
     * @return MembranePotential Total synaptic current
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear the synaptic input accumulator
     * 
     * This is called at the end of each LIF step to prepare for the
     * next step's inputs.
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike time in the spike history
     * 
     * Used by STDP and other spike-timing dependent plasticity rules.
     * Maintains a rolling history of up to MAX_SPIKE_HISTORY spikes.
     * 
     * @param timestamp Time at which spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the spike history (all recorded spike times)
     * 
     * @return const std::vector<Timestamp>& Vector of spike times
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear the spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add an incoming synapse to this neuron
     * 
     * @param handle Handle to the synapse
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse from this neuron
     * 
     * @param handle Handle to the synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapses
     * 
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapses
     * 
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the plasticity flags for this neuron
     * 
     * @return const PlasticityFlags& Plasticity configuration (Hebbian, STDP, etc.)
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to plasticity flags
     * 
     * @return PlasticityFlags& Mutable plasticity configuration
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Configure which plasticity rules apply to this neuron
     * 
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable spike-timing dependent plasticity
     * @param rewardModulated Enable reward-modulated learning
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the neuron's region identifier
     * 
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the neuron's region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the neuron's population identifier
     * 
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the neuron's population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Reset neuron to initial state (for restarting simulation)
     */
    void reset();
    
    /**
     * @brief Initialize neuron with biologically realistic random parameters
     * 
     * This function sets neuron parameters (membrane potential, threshold,
     * refractory period, etc.) based on realistic biological distributions
     * rather than uniform random values.
     * 
     * @param rng Random number generator for parameter sampling
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
