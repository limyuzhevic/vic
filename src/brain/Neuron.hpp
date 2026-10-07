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
 * @brief Represents a single spiking neuron implementing Leaky Integrate-and-Fire (LIF) dynamics
 * 
 * The Neuron class is the fundamental computational unit of the NLM (Neural Learning Machine).
 * Each neuron integrates incoming synaptic inputs, maintains membrane potential dynamics,
 * and generates spikes according to biologically-inspired LIF equations. Neurons participate
 * in plasticity mechanisms (STDP, Hebbian learning, structural plasticity) and neuromodulation
 * processes that modify their synaptic connections based on experience.
 * 
 * @details The LIF model implemented follows:
 *   dV/dt = (V_rest - V)/τ + I/C
 * Where:
 *   - V is membrane potential
 *   - V_rest is resting potential
 *   - τ is membrane time constant
 *   - I is total synaptic current
 *   - C is membrane capacitance
 * 
 * The neuron includes:
 * - Spike-frequency adaptation via an adaptation variable
 * - Refractory periods to prevent unrealistic firing rates
 * - Plasticity flags controlling different learning mechanisms
 * - Synaptic input history for spike-timing dependent plasticity (STDP)
 * 
 * @note This class uses the Pimpl idiom for efficient memory management and forward declarations.
 * 
 * @see https://en.wikipedia.org/wiki/Leaky_integrate-and-fire
 * @see https://www.sciencedirect.com/science/article/abs/10.1162/089976602317411963
 */
class Neuron {
public:
    /**
     * @brief Constructs a new neuron with the specified identifier
     * 
     * @param id Unique identifier for this neuron
     * @note Neurons are initially created in the Resting state with default parameters
     *       and are ready to participate in simulation after calling initialize()
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroys the neuron and cleans up internal resources
     */
    ~Neuron();
    
    /**
     * @brief Disabled copy constructor - neurons cannot be copied
     * 
     * Neurons use the Pimpl idiom and manage internal memory through pointers,
     * so copying would require deep copy semantics that are not supported.
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Disabled copy assignment operator - neurons cannot be assigned
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership of internal implementation
     * 
     * @param other Neuron to move from (will be left in a valid but unspecified state)
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership of internal implementation
     * 
     * @param other Neuron to move from (will be left in a valid but unspecified state)
     * @return Reference to this neuron after the assignment
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Gets the unique identifier of this neuron
     * 
     * @return NeuronId The neuron's identifier
     */
    NeuronId getId() const;
    
    /**
     * @brief Gets the neuron type (excitatory, inhibitory, modulatory, etc.)
     * 
     * @return NeuronType The type of this neuron
     * @see NeuronType enumeration in ../core/Types/Types.hpp
     */
    NeuronType getType() const;
    
    /**
     * @brief Sets the neuron type
     * 
     * @param type The new neuron type
     * @note Changing neuron type may affect synaptic interactions and plasticity
     */
    void setType(NeuronType type);
    
    /**
     * @brief Gets the complete state structure of this neuron
     * 
     * @return const NeuronState& Reference to the neuron's state (read-only)
     * @note Includes membrane potential, firing state, refractory status, etc.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Gets the complete state structure of this neuron (mutable)
     * 
     * @return NeuronState& Reference to the neuron's state (read/write)
     * @warning Modifying the state directly may lead to inconsistent neuron behavior
     */
    NeuronState& getState();
    
    /**
     * @brief Gets the current membrane potential
     * 
     * @return MembranePotential Current membrane potential in millivolts (mV)
     * @note Normal values: -70mV (resting), -55mV (threshold), up to +50mV (spike peak)
     */
    MembranePotential getMembranePotential() const;
    \n    /**
     * @brief Sets the membrane potential directly
     * 
     * @param potential New membrane potential in millivolts (mV)
     * @warning Direct manipulation bypasses normal LIF dynamics and should be used sparingly
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Adds a delta value to the current membrane potential
     * 
     * @param delta Change to apply to membrane potential in millivolts (mV)
     * @note Equivalent to: V += delta
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Gets the firing threshold potential
     * 
     * @return MembranePotential The voltage at which the neuron will fire
     * @note Typical values: -55mV (standard LIF), more negative for inhibited neurons
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Sets the firing threshold potential
     * 
     * @param threshold New firing threshold in millivolts (mV)
     * @note Lowering threshold makes neuron more likely to fire; raising it makes it more selective
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Checks if the neuron is currently in an active firing state
     * 
     * @return true if neuron has reached threshold and is firing, false otherwise
     * @note A neuron can be active but not refractory, refractory, or in other states
     */
    bool isFiring() const;
    
    /**
     * @brief Checks if the neuron is currently in its refractory period
     * 
     * @return true if neuron is refractory (cannot fire), false otherwise
     * @note During refractory period, the neuron cannot generate new spikes
     */
    bool isRefractory() const;
    
    /**
     * @brief Sets the firing state of the neuron
     * 
     * @param state The new firing state (Resting, Active, Refractory, Inhibited)
     * @warning Direct state manipulation should be used with caution as it may
     *          lead to inconsistent neuron behavior with the LIF dynamics
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Sets the refractory period duration
     * 
     * @param steps Number of simulation steps the neuron will be refractory after firing
     * @note Typical values: 2-10 steps (2-10ms) depending on neuron type and species model
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrements the refractory counter by one simulation step
     * 
     * @note Called automatically by the simulation clock during neuron stepping
     * @post If refractory counter reaches zero, neuron returns to Resting state
     */
    void decrementRefractory();
    
    /**
     * @brief Gets the current firing rate of the neuron
     * 
     * @return FiringRate Current firing rate in Hertz (spikes per second)
     * @note Rate-based computations may use this value instead of spike times
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Sets the firing rate (primarily for rate-based models)
     * 
     * @param rate New firing rate in Hertz (spikes per second)
     * @note In spike-based LIF models, this is typically computed from actual spike activity
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Sets the leak conductance parameter
     * 
     * @param conductance Leak conductance in nanoSiemens (nS)
     * @note Higher conductance means faster return to resting potential
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Gets the leak conductance parameter
     * 
     * @return MembranePotential Leak conductance in nanoSiemens (nS)
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Gets the current refractory period duration
     * 
     * @return uint32_t Number of simulation steps in the refractory period
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Sets the resting potential (baseline membrane voltage)
     * 
     * @param potential Resting potential in millivolts (mV)
     * @note Typical value: -70mV for standard LIF neurons
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Gets the resting potential
     * 
     * @return MembranePotential Resting potential in millivolts (mV)
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Sets the reset potential (value after spike)
     * 
     * @param potential Reset potential in millivolts (mV)
     * @note Typically close to resting potential but slightly depolarized
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Gets the reset potential
     * 
     * @return MembranePotential Reset potential in millivolts (mV)
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Checks if the neuron's membrane potential has reached the firing threshold
     * 
     * @return true if membrane potential >= threshold, false otherwise
     * @note This is checked during the LIF integration step to determine if a spike occurs
     */
    bool checkThreshold() const;
    
    /**
     * @brief Gets the timestamp of the most recent spike
     * 
     * @return float Time of last spike in simulation time units, -1.0 if no spikes yet
     * @note Used for spike-timing dependent plasticity (STDP) calculations
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Executes the LIF integration step and returns whether a spike occurred
     * 
     * @param currentTime Current simulation time
     * @param dt Timestep duration for the integration
     * @return true if neuron crossed threshold and fired, false otherwise
     * @post Neuron state is updated, and if a spike occurred, spike history is recorded
     *       and refractory period is initiated
     * 
     * @details Implements the discrete-time LIF equation:
     *   V(t+dt) = V(t) + dt * ((V_rest - V(t))/τ + I(t)/C)
     * 
     * The step includes:
     * 1. Refractory period handling
     * 2. Membrane potential integration (leak + synaptic)
     * 3. Spike-frequency adaptation
     * 4. Threshold crossing and spike generation
     * 5. Spike recording and state reset
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Adds excitatory synaptic input to the neuron
     * 
     * @param amplitude Synaptic strength in millivolts (mV)
     * @post Total synaptic current is increased, affecting membrane potential in next step
     * @note Represents post-synaptic excitatory potentials (EPSPs)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Adds inhibitory synaptic input to the neuron
     * 
     * @param amplitude Synaptic strength in millivolts (mV)
     * @post Total synaptic current is decreased (hyperpolarization), affecting membrane potential
     * @note Represents post-synaptic inhibitory potentials (IPSPs)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Adds modulatory synaptic input (neuromodulation)
     * 
     * @param amplitude Modulatory strength in millivolts (mV)
     * @post Primarily affects plasticity variables rather than immediate membrane potential
     * @note Used for neuromodulation signals (dopamine, acetylcholine, etc.) that affect learning
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Injects direct current into the neuron (external input)
     * 
     * @param current Current strength in millivolts (mV)
     * @post Added to synaptic input for LIF integration
     * @note Used for sensory input or experimental stimulation of neurons
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Gets the total synaptic current (read-only)
     * 
     * @return MembranePotential Total synaptic current in millivolts (mV)
     * @note Represents the sum of all current inputs since last clear
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clears all accumulated synaptic current
     * 
     * @post Total synaptic current is reset to zero
     * @note Called automatically at the end of each LIF step
     */
    void clearTotalCurrent();
    
    /**
     * @brief Records a spike at the specified time
     * 
     * @param timestamp Time of the spike
     * @post Spike history is updated for STDP calculations
     * @note Only called when neuron actually fires during stepLIF()
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Gets the spike history for STDP calculations
     * 
     * @return const std::vector<Timestamp>& Vector of spike times
     * @note Maintains recent spikes (up to MAX_SPIKE_HISTORY) for timing-dependent plasticity
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clears the spike history
     * 
     * @post All recorded spike times are removed
     * @note Called during neuron reset or when history exceeds maximum size
     */
    void clearSpikeHistory();
    
    /**
     * @brief Adds an incoming synapse connection handle
     * 
     * @param handle Synapse handle for an incoming connection
     * @post Neuron's incoming synapse list is updated
     * @note Used during synapse creation to track connectivity
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Adds an outgoing synapse connection handle
     * 
     * @param handle Synapse handle for an outgoing connection
     * @post Neuron's outgoing synapse list is updated
     * @note Used during synapse creation to track connectivity
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Gets the list of incoming synapse handles
     * 
     * @return const std::vector<SynapseHandle>& List of incoming synapses
     * @note Used for synaptic connectivity analysis and plasticity calculations
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Gets the list of outgoing synapse handles
     * 
     * @return const std::vector<SynapseHandle>& List of outgoing synapses
     * @note Used for synaptic connectivity analysis and plasticity calculations
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Gets the plasticity flags structure (read-only)
     * 
     * @return const PlasticityFlags& Neuron's plasticity configuration
     * @note Contains flags for Hebbian, STDP, and reward-modulated plasticity
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Gets the plasticity flags structure (mutable)
     * 
     * @return PlasticityFlags& Neuron's plasticity configuration
     * @warning Modifying plasticity flags may lead to inconsistent behavior
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enables specific plasticity mechanisms for this neuron
     * 
     * @param hebbian Enable Hebbian (additive) learning
     * @param stdp Enable Spike-Timing Dependent Plasticity (STDP)
     * @param rewardModulated Enable reward-modulated plasticity
     * @post Neuron's plasticity flags are updated accordingly
     * @note Can be called dynamically during simulation to turn on/off learning
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Sets the region identifier this neuron belongs to
     * 
     * @param region Region identifier
     * @post Neuron's region membership is updated
     * @note Regions group neurons by function or anatomical location
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Gets the neuron's region identifier
     * 
     * @return RegionId The neuron's region identifier
     * @note Used for regional connectivity and analysis
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Sets the population identifier this neuron belongs to
     * 
     * @param population Population identifier
     * @post Neuron's population membership is updated
     * @note Populations group neurons by type or functional role
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Gets the neuron's population identifier
     * 
     * @return PopulationId The neuron's population identifier
     * @note Used for population-level analysis and control
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Executes one simulation step using the LIF dynamics
     * 
     * @param currentTime Current simulation time
     * @post Neuron state is updated according to LIF integration
     * @note This is the main simulation method for spiking neurons
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets the neuron to its initial state
     * 
     * @post All state variables are restored to initial values, spike history cleared
     * @note Used to return neuron to baseline for repeated experiments
     */
    void reset();
    
    /**
     * @brief Initializes neuron parameters with biologically realistic random values
     * 
     * @param rng Random number generator for reproducible initialization
     * @post Neuron parameters (threshold, resting potential, refractory period, etc.)
     *       are set to random values within biologically plausible ranges
     * 
     * @details Initializes parameters based on typical biological ranges:
     * - Membrane potential: -70mV ± 3mV
     * - Threshold: -55mV ± 2mV  
     * - Resting potential: -70mV ± 2mV
     * - Reset potential: resting + 0-5mV
     * - Refractory period: 2-10 simulation steps
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Internal implementation structure (Pimpl idiom)
     * 
     * @details Separates the Neuron interface from its implementation to reduce
     * compilation dependencies and improve memory locality
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     */
    Impl* pImpl;
};

} // namespace nlm
