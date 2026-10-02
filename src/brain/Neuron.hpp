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
// Implements Leaky Integrate-and-Fire (LIF) dynamics
// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics with support for
// real synaptic inputs, neuromodulation, and synaptic plasticity

class Neuron {
public:
    /**
     * @brief Create a new neuron with the specified ID
     * 
     * Initializes a neuron with default parameters:
     * - Type: Internal
     * - Membrane potential: -70 mV (resting potential)
     * - Threshold: -55 mV
     * - Refractory period: 5 simulation steps
     * 
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy the neuron and clean up resources
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor - explicitly deleted to prevent copying
     * 
     * Neurons contain complex internal state (spike history, synaptic inputs)
     * that should not be copied. Use move semantics instead.
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator - explicitly deleted to prevent copying
     * 
     * Neurons contain complex internal state (spike history, synaptic inputs)
     * that should not be copied. Use move semantics instead.
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor - transfer ownership of internal implementation
     * 
     * Safely moves the internal implementation from another neuron,
     * leaving the source neuron in a valid but empty state.
     * 
     * @param other Neuron to move from
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfer ownership of internal implementation
     * 
     * Safely moves the internal implementation from another neuron,
     * cleaning up the current implementation first.
     * 
     * @param other Neuron to move from
     * @return Reference to this neuron after the move
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the neuron's unique identifier
     * 
     * @return Neuron ID
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's type
     * 
     * @return Neuron type (Excitatory, Inhibitory, Sensory, Motor, etc.)
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's type
     * 
     * @param type New neuron type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the neuron's complete state (const version)
     * 
     * @return Const reference to neuron state
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get the neuron's complete state (mutable version)
     * 
     * @return Mutable reference to neuron state
     */
    NeuronState& getState();
    
    /**
     * @brief Get the neuron's current membrane potential
     * 
     * @return Membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the neuron's membrane potential directly
     * 
     * Use this for direct control or testing. In normal operation, potential
     * changes through synaptic inputs and intrinsic dynamics.
     * 
     * @param potential Membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to the neuron's membrane potential
     * 
     * @param delta Change in membrane potential in mV
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the neuron's firing threshold
     * 
     * @return Threshold voltage in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the neuron's firing threshold
     * 
     * @param threshold Threshold voltage in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if the neuron is currently firing
     * 
     * @return true if neuron has fired in this timestep
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * During refractory period, the neuron cannot fire and synaptic inputs
     * are not integrated.
     * 
     * @return true if neuron is in refractory period
     */
    bool isRefractory() const;
    
    /**
     * @brief Set firing state directly
     * 
     * @param state New firing state (Resting, Active, Refractory, Inhibited)
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period duration
     * 
     * @param steps Number of simulation steps in refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter (called each timestep)
     * 
     * This method is called automatically during simulation steps to track
     * progress through the refractory period.
     */
    void decrementRefractory();
    
    /**
     * @brief Get current firing rate
     * 
     * @return Firing rate in Hz
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set firing rate
     * 
     * @param rate Firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set leak conductance (LIF parameter)
     * 
     * Controls how quickly the membrane potential decays to resting potential
     * 
     * @param conductance Leak conductance in nS
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get leak conductance
     * 
     * @return Leak conductance in nS
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
     * @param potential Resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get resting potential
     * 
     * @return Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set reset potential (post-spike reset value)
     * 
     * After a spike, the membrane potential is reset to this value
     * 
     * @param potential Reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get reset potential
     * 
     * @return Reset potential in mV
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Check if neuron has reached firing threshold
     * 
     * @return true if membrane potential >= threshold
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get time of last spike
     * 
     * @return Time of last spike in simulation steps, or -1 if never spiked
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform one LIF integration step
     * 
     * Implements the Leaky Integrate-and-Fire dynamics for one timestep:
     * 1. Update membrane potential based on leak and synaptic currents
     * 2. Apply spike-frequency adaptation
     * 3. Check for threshold crossing and spike if needed
     * 4. Reset membrane potential if spiked
     * 5. Enter refractory period if spiked
     * 
     * @param currentTime Current simulation time in ms
     * @param dt Timestep duration in ms
     * @return true if neuron fired during this timestep
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Process excitatory synaptic input
     * 
     * Excitatory inputs increase membrane potential (depolarize neuron)
     * 
     * @param amplitude Synaptic current amplitude in nA
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Process inhibitory synaptic input
     * 
     * Inhibitory inputs decrease membrane potential (hyperpolarize neuron)
     * 
     * @param amplitude Synaptic current amplitude in nA
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Process modulatory synaptic input
     * 
     * Modulatory inputs affect plasticity but not membrane potential directly
     * (e.g., neuromodulators like dopamine, acetylcholine)
     * 
     * @param amplitude Modulatory input amplitude
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject current directly into neuron (from external sources)
     * 
     * Direct current injection from sensory inputs or external stimulation
     * 
     * @param current Current to inject in nA
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total current accumulated since last clear
     * 
     * @return Total current in nA
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear accumulated synaptic current
     * 
     * Called at the end of each timestep to prepare for next timestep's inputs
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike timestamp for plasticity
     * 
     * Called when neuron fires to record post-synaptic spike for STDP
     * 
     * @param timestamp Time of spike
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get spike history (recent spikes)
     * 
     * @return Const reference to spike history vector
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse (post-synaptic connection)
     * 
     * @param handle Synapse handle
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse (pre-synaptic connection)
     * 
     * @param handle Synapse handle
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapses
     * 
     * @return Const reference to incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapses
     * 
     * @return Const reference to outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity flags
     * 
     * @return Const reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get plasticity flags (mutable version)
     * 
     * @return Mutable reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity mechanisms
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set neuron region membership
     * 
     * @param region Region ID
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get neuron region membership
     * 
     * @return Region ID
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set neuron population membership
     * 
     * @param population Population ID
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get neuron population membership
     * 
     * @return Population ID
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step (wrapper)
     * 
     * Convenience method that calls stepLIF with default timestep (1ms)
     * 
     * @param currentTime Current simulation time in ms
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     * 
     * Clears all state: membrane potential, firing state, refractory state,
     * adaptation variable, spike history, and synaptic inputs
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random biological parameters
     * 
     * Sets biologically plausible random values for all neuron parameters:
     * - Membrane potential: resting potential ±3 mV
     * - Threshold: -55 mV ±2 mV
     * - Resting potential: -70 mV ±2 mV
     * - Refractory period: 2-10 ms
     * - Leak conductance: ~10 nS
     * 
     * @param rng Random number generator
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
