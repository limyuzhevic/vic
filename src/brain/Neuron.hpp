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
 * @brief Complete state representation for a single neuron
 * 
 * This structure stores all state variables needed for Leaky Integrate-and-Fire (LIF)
 * neuron dynamics. Organized for cache efficiency with frequently accessed variables
 * near the beginning and less frequently accessed variables toward the end.
 * 
 * @details The LIF neuron model tracks membrane potential, synaptic inputs, and
 * spike history to determine when the neuron fires. State variables include:
 * - Membrane potentials (resting, threshold, reset)
 * - Synaptic conductances and total current
 * - Firing state and refractory period
 * - Adaptation variables for spike-frequency adaptation
 * - Spike timing history for spike-timing-dependent plasticity (STDP)
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
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    /**
     * @brief Default constructor initializes neuron to resting state
     * 
     * Sets all state variables to biologically plausible default values
     * that represent a typical cortical neuron in its resting state.
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
} __attribute__((aligned(16)));  // Cache alignment for performance

/**
 * @class Neuron
 * @brief Implements a single Leaky Integrate-and-Fire (LIF) neuron
 * 
 * The Neuron class represents a fundamental computational unit in the NLM brain.
 * Each neuron integrates incoming currents from synaptic inputs and produces
 * action potentials (spikes) when its membrane potential exceeds threshold.
 * 
 * @details Key features:
 * - Real biophysical dynamics using LIF model
 * - Event-driven spike propagation with precise timing
 * - Multiple input types (excitatory, inhibitory, modulatory)
 * - Spike history recording for STDP learning
 * - Plasticity flag management for learning rules
 * - Random parameter initialization for biological variability
 * 
 * The neuron operates in discrete time steps, updating its membrane potential
 * according to the LIF differential equation:
 * V(t+dt) = V(t) + dt*(I_leak + I_syn + I_injected)/C
 * 
 * When V reaches threshold, the neuron fires a spike and resets to resetPotential
 * for refractoryPeriod steps.
 */
class Neuron {
public:
    /**
     * @brief Construct a neuron with a unique identifier
     * 
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor releases neuron resources
     */
    ~Neuron();
    
    /**
     * @brief Disable copying (neurons must have unique IDs)
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Disable copy assignment
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor enables neuron transfer
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator enables neuron transfer
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the neuron's unique identifier
     * 
     * @return NeuronId Unique identifier for this neuron
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's type (excitatory, inhibitory, etc.)
     * 
     * @return NeuronType The neuron type classification
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's type
     * 
     * @param type New neuron type classification
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the neuron's complete state structure
     * 
     * @return const NeuronState& Reference to neuron state (const)
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get the neuron's complete state structure (mutable)
     * 
     * @return NeuronState& Reference to neuron state
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
     * @brief Add delta to membrane potential (for synaptic inputs)
     * 
     * @param delta Change in membrane potential in mV
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold potential
     * 
     * @return MembranePotential Firing threshold in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold
     * 
     * @param threshold New threshold in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * 
     * @return true if neuron is in firing state, false otherwise
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return true if neuron cannot fire, false otherwise
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the neuron's firing state
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
     * Called each simulation step while neuron is refractory
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate
     * 
     * @return FiringRate Current firing rate in Hz
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate (for rate-based computation)
     * 
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance parameter
     * 
     * @param conductance Leak conductance in nS
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     @brief Get the leak conductance parameter
     * 
     * @return MembranePotential Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period duration
     * 
     * @return uint32_t Refractory period in steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential
     * 
     * @param potential New resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential
     * 
     * @return MembranePotential Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential (post-spike)
     * 
     * @param potential New reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get the reset potential
     * 
     * @return MembranePotential Reset potential in mV
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Check if neuron has reached firing threshold
     * 
     * @return true if membrane potential >= threshold, false otherwise
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get timestamp of last spike
     * 
     * @return float Timestamp of last spike in ms, or -1 if never spiked
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform LIF neuron dynamics step
     * 
     * Updates neuron state according to integrate-and-fire model for one
     * simulation timestep. Returns true if neuron fired during this step.
     * 
     * @param currentTime Current simulation time in ms
     * @param dt Timestep duration in ms
     * @return bool true if neuron fired, false otherwise
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * Adds excitatory postsynaptic potential to membrane potential.
     * 
     * @param amplitude Input amplitude in mV
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * Adds inhibitory postsynaptic potential to membrane potential.
     * 
     * @param amplitude Input amplitude in mV (negative value)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * Adds modulatory input that can affect plasticity or other processes.
     * 
     * @param amplitude Input amplitude in mV
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current into neuron
     * 
     * Simulates external input like sensory stimulation or internal signals.
     * 
     * @param current Current injection in nA
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total injected current
     * 
     * @return MembranePotential Total injected current in nA
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear all injected current
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record spike timestamp for STDP learning
     * 
     * Maintains spike history for spike-timing-dependent plasticity calculations.
     * 
     * @param timestamp Time when spike occurred in ms
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get spike history for plasticity calculations
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse connection
     * 
     * @param handle Synapse handle for incoming connection
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse connection
     * 
     * @param handle Synapse handle for outgoing connection
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
     * @brief Get plasticity flags for learning rules
     * 
     * @return const PlasticityFlags& Reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get plasticity flags for learning rules (mutable)
     * 
     * @return PlasticityFlags& Reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity rules
     * 
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set region membership for the neuron
     * 
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region identifier
     * 
     * @return RegionId Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population membership for the neuron
     * 
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population identifier
     * 
     * @return PopulationId Population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step
     * 
     * Performs the complete neuron update including LIF dynamics,
     * synaptic integration, and state management.
     * 
     * @param currentTime Current simulation time in ms
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     * 
     * Restores all state variables to default values and clears history.
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random biological parameters
     * 
     * Sets neuron parameters (threshold, conductance, etc.) using random
     * values drawn from biologically plausible distributions.
     * 
     * @param rng Random number generator instance
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Pimpl idiom implementation details
     * 
     * Forward declaration to Neuron implementation for reduced compile times
     * and encapsulation of implementation details.
     */
    struct Impl;
    Impl* pImpl;  // Pointer to implementation
} __attribute__((aligned(64)));  // Cache line alignment for performance
