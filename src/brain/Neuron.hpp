#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// NeuronState: Complete neuron state representation for efficient storage
// Contains all dynamic variables needed to simulate neuron behavior
// Memory layout optimized for cache efficiency and SIMD operations

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
     * Sets all state variables to biologically plausible initial values
     * following typical neuron properties (resting potential ~ -70mV,
     * threshold ~ -55mV, etc.)
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

// Neuron: Individual neuron implementation using Leaky Integrate-and-Fire (LIF) dynamics
// Implements real spiking neuron computation with refractory periods, adaptation,
// and synaptic integration capabilities
// Based on classical LIF neuron models with biological plausibility

class Neuron {
public:
    /**
     * @brief Constructs a neuron with unique identifier
     * 
     * @param id Unique identifier for this neuron (must be unique across all neurons)
     * 
     * @details
     * Creates a neuron with default parameters following typical biophysical properties:
     * - Resting potential: -70mV
     * - Firing threshold: -55mV  
     * - Refractory period: 5 simulation steps
     * - Membrane capacitance: 1nF (implicit in LIF dynamics)
     * - Time constant: 20ms (from Dynamics::IntegrateAndFireDynamics)
     * 
     * The neuron is initialized but not yet integrated into any region or population.
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Virtual destructor for proper cleanup of neuron resources
     * 
     * Ensures proper destruction of the pImpl object and cleanup of any
     * allocated memory or resources.
     */
    ~Neuron();
    
    /**
     * @brief Move constructor - transfers ownership of implementation
     * 
     * @param other Source neuron to move from
     * 
     * @details
     * Efficient transfer of the pImpl pointer without deep copying.
     * Source neuron is left in a valid, empty state.
     */
    Neuron(Neuron&& other) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership and cleans up
     * 
     * @param other Source neuron to move from
     * @return Neuron& Reference to this neuron for chaining
     * 
     * @details
     * Proper cleanup of current implementation before transferring ownership.
     * Prevents resource leaks during self-assignment.
     */
    Neuron& operator=(Neuron&& other) noexcept;
    
    /**
     * @brief Explicitly delete copy constructor - neurons cannot be copied
     * 
     * @details
     * Neurons use pImpl idiom with internal dynamic state. Direct copying
     * would require complex deep copying of state and relationships.
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Explicitly delete copy assignment operator - neurons cannot be copied
     * 
     * @details
     * See copy constructor rationale. This prevents accidental neuron duplication.
     */
    Neuron& operator=(const Neuron&) = delete;
    
/**
     * @brief Gets the neuron's unique identifier
     * 
     * @return NeuronId The neuron's ID
     */
    NeuronId getId() const;
    
    /**
     * @brief Gets the neuron's type classification
     * 
     * @return NeuronType The neuron type (excitatory, inhibitory, etc.)
     */
    NeuronType getType() const;
    
    /**
     * @brief Sets the neuron type classification
     * 
     * @param type The neuron type to assign
     * 
     * @details
     * Neuron type affects synaptic interactions and plasticity rules.
     * Common types include excitatory, inhibitory, and modulatory neurons.
     */
    void setType(NeuronType type);
    
    /**
     * @brief Gets the complete neuron state (const accessor)
     * 
     * @return const NeuronState& Reference to neuron state
     * 
     * @details
     * Provides read-only access to all neuron state variables. Essential for
     * monitoring and analysis of neural dynamics.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Gets the complete neuron state (mutable accessor)
     * 
     * @return NeuronState& Reference to neuron state
     * 
     * @details
     * Provides mutable access to neuron state for direct modification.
     * Use with caution as this bypasses normal neuron dynamics.
     */
    NeuronState& getState();
    
    /**
     * @brief Gets the current membrane potential
     * 
     * @return MembranePotential Current membrane potential in millivolts (mV)
     * 
     * @details
     * The membrane potential represents the electrical state of the neuron.
     * Values below threshold indicate resting state, above threshold indicate
     * spiking readiness.
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Sets the membrane potential directly
     * 
     * @param potential Membrane potential in mV
     * 
     * @details
     * Direct membrane potential manipulation. Useful for testing, debugging,
     * or implementing controlled stimulation protocols.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Increments the membrane potential by a delta value
     * 
     * @param delta Change in membrane potential in mV
     * 
     * @details
     * This is the primary method for synaptic input integration. The delta
     * can be positive (excitatory) or negative (inhibitory).
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Gets the firing threshold potential
     * 
     * @return MembranePotential Threshold in mV
     * 
     * @details
     * When membrane potential reaches or exceeds this value, the neuron
     * will spike. Typical value: -55mV for standard LIF neurons.
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Sets the firing threshold
     * 
     * @param threshold Threshold potential in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Checks if neuron is currently firing
     * 
     * @return bool True if neuron is in firing state
     */
    bool isFiring() const;
    
    /**
     * @brief Checks if neuron is in refractory period
     * 
     * @return bool True if neuron cannot fire due to recent activity
     */
    bool isRefractory() const;
    
    /**
     * @brief Sets the neuron's firing state
     * 
     * @param state New firing state
     * 
     * @details
     * Direct firing state manipulation. Prefer natural dynamics through
     * spike detection methods instead of direct state setting.
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Sets the refractory period duration
     * 
     * @param steps Number of simulation steps to remain refractory
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrements remaining refractory time by one step
     * 
     * @details
     * Called automatically during simulation step. When refractory time
     * reaches zero, neuron becomes eligible to fire again.
     */
    void decrementRefractory();
    
    /**
     * @brief Gets the current firing rate
     * 
     * @return FiringRate Current firing rate in Hertz (Hz)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Sets the firing rate
     * 
     * @param rate Firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Sets the leak conductance parameter
     * 
     * @param conductance Leak conductance in nS
     * 
     * @details
     * Controls the rate at which membrane potential decays toward resting
     * potential. Higher values = faster decay, lower values = slower decay.
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Gets the leak conductance
     * 
     * @return MembranePotential Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Gets the refractory period duration
     * 
     * @return uint32_t Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Sets the resting potential
     * 
     * @param potential Resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Gets the resting potential
     * 
     * @return MembranePotential Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Sets the reset potential (post-spike value)
     * 
     * @param potential Reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Gets the reset potential
     * 
     * @return MembranePotential Reset potential in mV
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Checks if neuron should fire based on threshold crossing
     * 
     * @return bool True if membrane potential >= threshold
     * 
     * @details
     * This method is called by dynamics classes to determine if a neuron
     * should fire. Does not change neuron state - only detects firing condition.
     */
    bool checkThreshold() const;
    
    /**
     * @brief Gets the timestamp of the last spike
     * 
     * @return float Timestamp of last spike, or -1.0f if no spikes occurred
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Performs Leaky Integrate-and-Fire dynamics for one timestep
     * 
     * @param currentTime Current simulation timestamp
     * @param dt Timestep duration in seconds
     * @return bool True if neuron fired during this timestep
     * 
     * @details
     * Core LIF dynamics implementation:
     * 1. Integrate synaptic inputs: dV/dt = (I - g_leak*(V - V_rest))/C
     * 2. Check for threshold crossing
     * 3. If threshold crossed: spike, reset potential, enter refractory period
     * 4. Return spike status for spike propagation
     * 
     * This is the main neuron dynamics function called during simulation steps.
     * 
     * References:
     * - Dayan & Abbott (2001) "Theoretical Neuroscience"
     * - Gerstner et al. (2014) "Neuronal Dynamics"
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receives excitatory synaptic input
     * 
     * @param amplitude Synaptic current amplitude in nA
     * 
     * @details
     * Postsynaptic potential generated by excitatory synapses.
     * Increases membrane potential toward threshold.
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives inhibitory synaptic input
     * 
     * @param amplitude Synaptic current amplitude in nA
     * 
     * @details
     * Postsynaptic potential generated by inhibitory synapses.
     * Decreases membrane potential away from threshold.
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives modulatory synaptic input
     * 
     * @param amplitude Modulatory synaptic current in nA
     * 
     * @details
     * Modulatory inputs affect neuron excitability and plasticity rules.
     * Effects depend on receptor types and intracellular mechanisms.
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Injects current directly into the neuron (external input)
     * 
     * @param current Current to inject in nA
     * 
     * @details
     * External current injection. Useful for testing, stimulation protocols,
     * or implementing external drives. Bypasses synaptic mechanisms.
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Gets the total current input this timestep
     * 
     * @return MembranePotential Total current in nA
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clears accumulated synaptic input
     * 
     * @details
     * Resets total current to zero. Called at the end of each timestep
     * to prepare for the next integration cycle.
     */
    void clearTotalCurrent();
    
    /**
     * @brief Records a spike event with timestamp
     * 
     * @param timestamp Time when spike occurred
     * 
     * @details
     * Maintains spike history for plasticity calculations (STDP, etc.).
     * Spike history is used by learning algorithms to compute timing-dependent
     * plasticity effects.
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Gets the spike history (recent spikes)
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clears the spike history
     * 
     * @details
     * Resets spike history to prepare for next learning period.
     * Called during reset or at end of learning window.
     */
    void clearSpikeHistory();
    
    /**
     * @brief Adds an incoming synapse connection
     * 
     * @param handle Synapse handle for incoming connection
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Adds an outgoing synapse connection
     * 
     * @param handle Synapse handle for outgoing connection
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Gets all incoming synapse connections
     * 
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Gets all outgoing synapse connections
     * 
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Gets the plasticity flags for this neuron
     * 
     * @return const PlasticityFlags& Reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Gets mutable access to plasticity flags
     * 
     * @return PlasticityFlags& Reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enables specific plasticity rules for this neuron
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * @details
     * Controls which plasticity rules apply to synapses connected to this neuron.
     * Rules can be enabled/disabled independently.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Sets the neuron's region membership
     * 
     * @param region Region ID
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Gets the neuron's region ID
     * 
     * @return RegionId Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Sets the neuron's population ID
     * 
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Gets the neuron's population ID
     * 
     * @return PopulationId Population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Updates the neuron for one simulation step
     * 
     * @param currentTime Current simulation timestamp
     * 
     * @details
     * Main neuron update function. Handles LIF dynamics, spike detection,
     * refractory period management, and state updates. Coordinates with
     * plasticity system for learning.
     * 
     * TODO PHASE 2: Implement real integrate-and-fire dynamics
     * Current implementation: stepLIF() with proper membrane integration,
     * spike detection, and refractory handling
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets the neuron to its initial state
     * 
     * @details
     * Resets all neuron state variables to initial values from NeuronState
     * constructor. Clears spike history, resets membrane potential, etc.
     */
    void reset();
    
    /**
     * @brief Initializes neuron with random parameters
     * 
     * @param rng Random number generator for stochastic initialization
     * 
     * @details
     * Applies random variations to neuron parameters based on distribution
     * defined in configuration. Used to create diverse neural populations.
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
