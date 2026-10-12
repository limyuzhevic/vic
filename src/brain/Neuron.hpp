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
 * @brief Represents a single neuron implementing Leaky Integrate-and-Fire (LIF) dynamics
 * 
 * This is a core component of the NLM (Neural Learning Machine) brain model,
 * simulating biological neurons with electrical properties and firing behavior.
 * 
 * The neuron model includes:
 * - Membrane potential integration using LIF dynamics
 * - Spike generation when threshold is reached
 * - Refractory periods after firing
 * - Synaptic input processing (excitatory, inhibitory, modulatory)
 * - Plasticity state management for learning
 * - Connectivity tracking via synapse handles
 * 
 * @details The neuron implements a simplified biophysical model that balances
 * computational efficiency with biological plausibility. Each neuron maintains
 * internal state including membrane potential, firing thresholds, and synaptic
 * inputs, and can change its synaptic connections through plasticity rules.
 * 
 * The neuron is part of a larger population and belongs to a specific region
 * and population, allowing for organized network structure.
 */
class Neuron {
public:
    /**
     * @brief Construct a new Neuron with a unique identifier
     * 
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy the Neuron and clean up allocated memory
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor deleted - neurons cannot be copied
     * @param other Neuron to copy (not used)
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator deleted - neurons cannot be copied
     * @param other Neuron to assign (not used)
     * @return Reference to this neuron (never used)
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership of implementation
     * 
     * @param other Neuron to move from
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership of implementation
     * 
     * @param other Neuron to move from
     * @return Reference to this neuron for chaining
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * 
     * @return NeuronId Unique identifier
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron type (e.g., excitatory, inhibitory, sensory)
     * 
     * @return NeuronType Type of this neuron
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type
     * 
     * @param type New neuron type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get a const reference to the neuron's current state
     * 
     * @return const NeuronState& Constant reference to neuron state
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get a mutable reference to the neuron's current state
     * 
     * @return NeuronState& Mutable reference to neuron state
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * 
     * @return MembranePotential Current membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential to an absolute value
     * 
     * @param potential New membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add a delta to the membrane potential (useful for synaptic inputs)
     * 
     * @param delta Change in membrane potential in mV
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold voltage
     * 
     * @return MembranePotential Firing threshold in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold voltage
     * 
     * @param threshold New firing threshold in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if the neuron is currently firing (in active state)
     * 
     * @return true if neuron is firing, false otherwise
     */
    bool isFiring() const;
    
    /**
     * @brief Check if the neuron is in refractory period
     * 
     * @return true if neuron is refractory, false otherwise
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the neuron's firing state (e.g., resting, active, refractory)
     * 
     * @param state New firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period duration
     * 
     * @param steps Number of simulation steps to remain refractory
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement the refractory period counter
     * 
     * Should be called each simulation step to countdown refractory period
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate (in Hz)
     * 
     * @return FiringRate Current firing rate
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate
     * 
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance (affects membrane time constant)
     * 
     * @param conductance Leak conductance in nS
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
     * @brief Set the resting membrane potential
     * 
     * @param potential Resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting membrane potential
     * 
     * @return MembranePotential Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential (membrane potential after spike)
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
     * @brief Check if the membrane potential has reached firing threshold
     * 
     * @return true if threshold reached, false otherwise
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the time when the last spike occurred
     * 
     * @return float Time of last spike in ms, or -1.0 if no spikes
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform one LIF integration step and check for spikes
     * 
     * @param currentTime Current simulation time in ms
     * @param dt Timestep duration in ms
     * @return true if neuron fired during this step, false otherwise
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input (adds to membrane potential)
     * 
     * @param amplitude Synaptic input amplitude in mV
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input (subtracts from membrane potential)
     * 
     * @param amplitude Synaptic input amplitude in mV
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input (affects plasticity)
     * 
     * @param amplitude Modulatory input amplitude
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current (e.g., from sensory input)
     * 
     * @param current External current in pA
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total synaptic current (deprecated, use synapticInput instead)
     * @deprecated Use internal synapticInput directly
     * @return MembranePotential Total current in pA
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear accumulated synaptic current
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike event with timestamp
     * 
     * @param timestamp Time when spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the history of recent spike timestamps
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear all spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add an incoming synapse to this neuron
     * 
     * @param handle Synapse handle for incoming connection
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse from this neuron
     * 
     * @param handle Synapse handle for outgoing connection
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get list of incoming synapse handles
     * 
     * @return const std::vector<SynapseHandle>& List of incoming synapses
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get list of outgoing synapse handles
     * 
     * @return const std::vector<SynapseHandle>& List of outgoing synapses
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the neuron's plasticity flags
     * 
     * @return const PlasticityFlags& Const reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity flags for modification
     * 
     * @return PlasticityFlags& Mutable reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable specific plasticity mechanisms
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable Spike-Timing Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
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
     * @brief Update neuron state for one simulation step
     * 
     * This is the main update function called each simulation step.
     * Currently a placeholder (Phase 2 TODO: implement real LIF dynamics).
     * 
     * @param currentTime Current simulation time in ms
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to its initial state
     * 
     * Resets membrane potential, clears spike history, and resets all internal state
     * to the values defined by the neuron's initialization function.
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random biologically plausible parameters
     * 
     * Sets resting potential, threshold, refractory period, and other parameters
     * to random values within realistic biological ranges.
     * 
     * @param rng Random number generator for parameter selection
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Pimpl idiom implementation details
     * 
     * This hides the implementation details and allows forward declaration
     * of internal types without exposing them in the header.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
