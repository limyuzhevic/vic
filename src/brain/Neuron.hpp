#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @brief Stores the complete dynamical state of a single neuron
 * 
 * This structure encapsulates all variables needed to simulate Leaky Integrate-and-Fire (LIF) neuron dynamics.
 * It is designed for efficient memory access and cache locality during large-scale network simulations.
 * 
 * The state variables include membrane potential dynamics, firing state, refractory period tracking,
 * synaptic inputs, and plasticity-related variables.
 * 
 * @note This struct is public to allow direct access for performance-critical code paths.
 *       For scientific reproducibility, ensure the exact same state management is used.
 */
struct NeuronState {
    /**
     * @brief Current membrane potential (mV)
     * 
     * This is the instantaneous voltage across the neuron's membrane.
     * When this value crosses the threshold, the neuron generates a spike.
     * 
     * Typical resting values range from -70 to -60 mV for most neuron types.
     */
    MembranePotential membranePotential;
    
    /**
     * @brief Resting potential (mV)
     * 
     * The equilibrium voltage to which the membrane potential tends in the absence of inputs.
     * Determined by the neuron's ion channel composition and active transport mechanisms.
     * 
     * Default: -70.0 mV (typical for many neuron types)
     */
    MembranePotential restingPotential;
    
    /**
     * @brief Firing threshold (mV)
     * 
     * The membrane potential at which the neuron becomes sufficiently depolarized
     * to generate an action potential (spike). When membranePotential >= threshold,
     * the neuron enters the firing state and produces a spike.
     * 
     * Typical values range from -50 to -40 mV for fast-spiking neurons.
     * 
     * @note In some models, threshold can be dynamic (adaptation-dependent).
     */
    MembranePotential threshold;
    
    /**
     * @brief Post-spike reset value (mV)
     * 
     * The membrane potential value to which the neuron is reset immediately after
     * generating a spike. This represents the after-hyperpolarization phase.
     * 
     * Typically lower than restingPotential to ensure proper refractory dynamics.
     * Default: -70.0 mV (same as resting potential in basic LIF)
     */
    MembranePotential resetPotential;
    
    /**
     * @brief Leak conductance (nS)
     * 
     * Represents the membrane's passive conductance to ions, allowing the membrane
     * potential to decay exponentially toward the resting potential.
     * 
     * Higher values cause faster decay (shorter membrane time constant).
     * Default: 10.0 nS (typical for neuron with ~10 ms membrane time constant)
     */
    MembranePotential leakConductance;
    
    /**
     * @brief Synaptic conductance (nS)
     * 
     * The total conductance of all active synaptic inputs (excitatory + inhibitory + modulatory)
     * at the current simulation step. This represents the combined effect of synaptic currents.
     * 
     * @note This is the sum of all synaptic conductances, not individual conductances.
     */
    MembranePotential synapseConductance;
    
    /**
     * @brief Current firing rate (Hz)
     * 
     * The instantaneous firing rate calculated from recent spike history.
     * Useful for rate-based plasticity rules and rate-coded representations.
     * 
     * @note For spike-based models, this may be derived from spike count/timestamp.
     */
    FiringRate firingRate;
    
    /**
     * @brief Current firing state
     * 
     * Discrete state indicating whether the neuron is currently in its active firing state.
     * Can be Resting, Firing, or Refractory depending on the simulation phase.
     * 
     * @see FiringState enum for valid state values
     */
    FiringState firingState;
    
    /**
     * @brief Steps remaining in refractory period
     * 
     * Number of simulation steps remaining during which the neuron cannot generate spikes.
     * Used to enforce the biological refractory period after each spike.
     * 
     * When refractoryRemaining > 0, the neuron is in refractory state and cannot spike.
     * Decremented each simulation step via decrementRefractory().
     */
    uint32_t refractoryRemaining;
    
    /**
     * @brief Total refractory period duration (steps)
     * 
     * The number of simulation steps the neuron must remain refractory after spiking.
     * This determines the absolute refractory period length in simulation time units.
     * 
     * Typical values range from 1-10 steps depending on neuron type and simulation timestep.
     */
    uint32_t refractoryPeriod;
    
    /**
     * @brief Variable for spike-frequency adaptation (mV)
     * 
     * Adaptation variable that increases during spiking and reduces threshold/firing rate
     * over time, implementing short-term spike-frequency adaptation.
     
     * Often used in models of adaptation, where neurons reduce their firing rate
     * in response to sustained input, mimicking biological adaptation phenomena.
     */
    float adaptationVariable;
    
    /**
     * @brief Timestamp of last spike (simulation steps)
     * 
     * The simulation timestep at which the neuron last generated a spike.
     * Set to -1.0 when no spike has occurred yet.
     * 
     * Used for STDP (Spike-Timing-Dependent Plasticity) and other temporal computations.
     */
    float lastSpikeTime;
    
    /**
     * @brief Default constructor initializes neuron to resting state
     * 
     * Sets membrane potential to typical resting value (-70 mV).
     * All firing-related variables are initialized to zero or idle states.
     * No spike history is present initially.
     * 
     * This constructor is used when creating neurons from scratch or during reset operations.
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

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
class Neuron {
public:
    /**
     * @brief Constructs a neuron with a unique identifier
     * 
     * @param id Unique identifier for this neuron within its network
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroys the neuron and cleans up resources
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor is deleted (neurons cannot be copied)
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted (neurons cannot be assigned)
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor (efficient transfer of ownership)
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator (efficient transfer of ownership)
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Retrieves the neuron's unique identifier
     * 
     * @return The NeuronId assigned during construction
     */
    NeuronId getId() const;
    
    /**
     * @brief Retrieves the neuron's type classification
     * 
     * @return The NeuronType (e.g., Internal, External, Special)
     */
    NeuronType getType() const;
    
    /**
     * @brief Sets the neuron's type classification
     * 
     * @param type The NeuronType to assign to this neuron
     */
    void setType(NeuronType type);
    
    /**
     * @brief Provides read-only access to the neuron's complete state
     * 
     * @return Constant reference to the NeuronState structure
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Provides mutable access to the neuron's complete state
     * 
     * @return Reference to the NeuronState structure
     */
    NeuronState& getState();
    
    /**
     * @brief Retrieves the current membrane potential
     * 
     * @return Current membrane potential in millivolts (mV)
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Sets the membrane potential directly
     * 
     * @param potential New membrane potential in millivolts (mV)
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Adds a delta to the current membrane potential
     * 
     * @param delta Voltage change to add in millivolts (mV)
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Retrieves the firing threshold voltage
     * 
     * @return Threshold voltage in millivolts (mV)
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Sets the firing threshold voltage
     * 
     * @param threshold New threshold voltage in millivolts (mV)
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Checks if the neuron is currently firing (spiking)
     * 
     * @return true if the neuron has generated a spike in the current timestep
     */
    bool isFiring() const;
    
    /**
     * @brief Checks if the neuron is in refractory period
     * 
     * @return true if refractoryRemaining > 0, false otherwise
     */
    bool isRefractory() const;
    
    /**
     * @brief Sets the neuron's firing state directly
     * 
     * @param state The firing state to set (Resting, Firing, or Refractory)
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Sets the refractory period duration
     * 
     * @param steps Number of simulation steps for the refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrements the refractory period counter
     */
    void decrementRefractory();
    
    /**
     * @brief Retrieves the current firing rate
     * 
     * @return Firing rate in Hertz (spikes per second)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Sets the firing rate directly
     * 
     * @param rate Firing rate in Hertz (Hz)
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Sets the membrane leak conductance
     * 
     * @param conductance Conductance in nanosiemens (nS)
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Retrieves the membrane leak conductance
     * 
     * @return Leak conductance in nanosiemens (nS)
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Retrieves the refractory period duration
     * 
     * @return Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Sets the resting potential
     * 
     * @param potential Resting potential in millivolts (mV)
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Retrieves the resting potential
     * 
     * @return Resting potential in millivolts (mV)
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Sets the reset potential after spiking
     * 
     * @param potential Reset potential in millivolts (mV)
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Retrieves the reset potential after spiking
     * 
     * @return Reset potential in millivolts (mV)
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Checks if neuron has crossed firing threshold
     * 
     * @return true if membrane potential >= threshold, false otherwise
     */
    bool checkThreshold() const;
    
    /**
     * @brief Retrieves the timestamp of the last spike
     * 
     * @return Timestamp of last spike in simulation steps, or -1.0 if no spike occurred
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Executes one LIF integration step
     * 
     * This is the core simulation method that advances the neuron through one
     * time step according to Leaky Integrate-and-Fire dynamics.
     * 
     * @param currentTime Current simulation time in steps
     * @param dt Simulation timestep in milliseconds
     * 
     * @return true if neuron fired (spike generated) during this step, false otherwise
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receives excitatory synaptic input
     * 
     * @param amplitude Synaptic current amplitude in nanoamps (nA)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives inhibitory synaptic input
     * 
     * @param amplitude Synaptic current amplitude in nanoamps (nA)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives modulatory synaptic input
     * 
     * @param amplitude Synaptic current amplitude in nanoamps (nA)
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Injects external current into the neuron
     * 
     * @param current External current injection in nanoamps (nA)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Retrieves the total current affecting the neuron
     * 
     * @return Total current in nanoamps (nA)
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clears all accumulated current inputs
     */
    void clearTotalCurrent();
    
    /**
     * @brief Records a spike occurrence with timestamp
     * 
     * @param timestamp Simulation timestep when the spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Retrieves the neuron's spike history
     * 
     * @return Vector of spike timestamps in chronological order
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clears the neuron's spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Adds an incoming synapse to this neuron
     * 
     * @param handle Unique identifier for the incoming synapse
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Adds an outgoing synapse from this neuron
     * 
     * @param handle Unique identifier for the outgoing synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Retrieves all incoming synapse handles
     * 
     * @return Vector of handles to all synapses that deliver input to this neuron
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Retrieves all outgoing synapse handles
     * 
     * @return Vector of handles to all synapses that deliver output from this neuron
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Retrieves the neuron's plasticity flags
     * 
     * @return Constant reference to the PlasticityFlags structure
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Provides mutable access to plasticity flags
     * 
     * @return Reference to the PlasticityFlags structure
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enables specific plasticity mechanisms
     * 
     * @param hebbian Enable Hebbian STDP plasticity if true
     * @param stdp Enable standard STDP if true
     * @param rewardModulated Enable reward-modulated plasticity if true
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Sets the neuron's region identifier
     * 
     * @param region RegionId of the parent neural region
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Retrieves the neuron's region identifier
     * 
     * @return The RegionId assigned to this neuron
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Sets the neuron's population identifier
     * 
     * @param population PopulationId of the parent neural population
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Retrieves the neuron's population identifier
     * 
     * @return The PopulationId assigned to this neuron
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Advances the neuron through one complete simulation step
     * 
     * This method performs a comprehensive update of neuron state including:
     * 1. Integration of incoming currents and inputs
     * 2. Membrane potential dynamics
     * 3. Spike detection and generation
     * 4. Refractory period management
     * 5. State reset and adaptation
     * 
     * @param currentTime Current simulation time in steps
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets the neuron to its initial state
     * 
     * This method restores the neuron to its constructor-initialized state.
     */
    void reset();
    
    /**
     * @brief Initializes neuron parameters with random values
     * 
     * @param rng Reference to random number generator
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Opaque implementation structure for the pImpl idiom
     */
    struct Impl;
    
    /**
     * @brief Pointer to the implementation structure
     */
    Impl* pImpl;
};

} // namespace nlm
