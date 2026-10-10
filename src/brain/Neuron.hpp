#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

/**
 * @file Neuron.hpp
 * 
 * Single neuron implementation for NLM
 * 
 * This file provides the fundamental computational unit of the NLM
 * system - the individual neuron. The Neuron class implements the
 * Leaky Integrate-and-Fire (LIF) model, which is the standard
 * model for spiking neural networks. Each neuron represents a basic
 * processing element that integrates inputs and produces outputs
 * through spike generation.
 * 
 * Neurons are the basic building blocks of the NLM neural network,
 * forming the foundation for complex neural computation. Each neuron
 * maintains its own state, processes incoming signals, and generates
 * spike outputs that drive learning and communication throughout
 * the neural system.
 * 
 * Key features:
 * - Leaky Integrate-and-Fire dynamics
 * - Multiple input types (excitatory, inhibitory, modulatory)
 * - Refractory period to prevent excessive firing
 * - Synaptic connectivity management
 * - Plasticity state management
 * - Spike history for learning (STDP)
 * - Current injection capabilities
 * - Rate-based and event-based computation modes
 * - Random initialization for reproducible experiments
 * 
 * @note Essential for understanding neural computation in NLM
 * @note Used by all neural regions and populations
 * @note Central to learning and adaptation processes
 * @see Brain.hpp - For brain-level neuron management
 * @see NeuralRegion.hpp - For region-level organization
 * @see Synapse.hpp - For synaptic connectivity
 * @see STDP.hpp - For spike-timing-dependent plasticity
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @brief Forward declarations for Neuron dependencies
 */
class Synapse;
class RandomGenerator;

/**
 * @struct NeuronState
 * 
 * Complete state representation for a single neuron.
 * 
 * This structure encapsulates the complete internal state of a
 * neuron, including both membrane potentials and dynamic variables
 * that determine neural behavior. The NeuronState structure is
 * used to store neuron state efficiently and is accessed through
 * the Neuron class via getState() and setState() methods.
 * 
 * The state includes:
 * - Membrane potentials (voltage levels)
 * - Firing state and rate
 * - Refractory period information
 * - Adaptation variables
 * - Spike history and timing
 * 
 * @note Used by Neuron for efficient state management
 * @note Critical for neural dynamics and learning
 * @see Neuron::getState() - For state access
 * @see Neuron::setState() - For state modification
 * @see Neuron::reset() - For state reset
 * 
 * @author NLM Development Team
 */
struct NeuronState {
    /** Current membrane potential relative to resting potential (mV) */
    MembranePotential membranePotential;
    
    /** Resting membrane potential (mV) - equilibrium voltage */
    MembranePotential restingPotential;
    
    /** Firing threshold potential (mV) - triggers spike when crossed */
    MembranePotential threshold;
    
    /** Reset potential after spike (mV) - returns to after firing */
    MembranePotential resetPotential;
    
    /** Leak conductance (nS) - determines membrane decay rate */
    MembranePotential leakConductance;
    
    /** Synaptic conductance (nS) - total incoming synaptic input */
    MembranePotential synapseConductance;
    
    /** Current firing rate (Hz) - for rate-based computation */
    FiringRate firingRate;
    
    /** Current firing state - determines neuron behavior */
    FiringState firingState;
    
    /** Steps remaining in refractory period */
    uint32_t refractoryRemaining;
    
    /** Total refractory period in simulation steps */
    uint32_t refractoryPeriod;
    
    /** Adaptation variable for spike-frequency adaptation */
    float adaptationVariable;
    
    /** Timestamp of last spike (-1 if none) */
    float lastSpikeTime;
    
    /**
     * @brief Default constructor
     * 
     * Creates a neuron in its initial resting state with typical
     * biophysical parameters. This represents a generic neuron
     * configuration that can be customized through initialization.
     * 
     * @post All membrane potentials initialized
     * @post Firing state set to resting
     * @post Refractory period zero
     * @post Spike timestamp set to -1
     * @see initializeRandom() - For custom initialization
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
 * 
 * Single spiking neuron implementing Leaky Integrate-and-Fire (LIF) dynamics.
 * 
 * The Neuron class represents the fundamental computational unit of the
 * NLM neural network. It implements the LIF model, which is the standard
 * model for spiking neural networks and biological neurons. Each neuron
 * integrates incoming synaptic inputs and generates outputs through spike
 * production when its membrane potential crosses a threshold.
 * 
 * The neuron model includes:
 * - **Membrane dynamics**: Integration of inputs and leaky decay
 * - **Threshold spiking**: Generation of discrete spikes
 * - **Refractory period**: Recovery period after spiking
 * - **Synaptic integration**: Summation of incoming signals
 * - **Plasticity management**: Synaptic weight modification
 * - **Learning support**: Spike history for STDP
 * 
 * Neuronal computation in NLM follows this basic cycle:
 * 1. Receive inputs (synaptic potentials, injected current)
 * 2. Integrate inputs (membrane potential changes)
 * 3. Apply leak decay (natural decay toward resting potential)
 * 4. Check threshold (detect spike condition)
 * 5. Generate spike (if threshold crossed)
 * 6. Update state (reset, refractory, adaptation)
 * 
 * @note Central to all neural computation in NLM
 * @note Used by all neural regions and populations
 * @note Essential for learning and adaptation
 * @see Brain.hpp - For brain-level neuron management
 * @see NeuralRegion.hpp - For region organization
 * @see Synapse.hpp - For synaptic connectivity
 * @see STDP.hpp - For spike-timing-dependent plasticity
 * 
 * @author NLM Development Team
 */
class Neuron {
public:
    /**
     * @brief Constructor with neuron ID
     * 
     * Creates a new neuron with the specified ID. The neuron is
     * initialized with default parameters and ready for synaptic
     * connections and input processing.
     * 
     * @param id Unique identifier for the neuron
     * 
     * @pre id must be valid (not zero or invalid)
     * @post Neuron created with specified ID
     * @post State initialized to resting
     * @post Ready for synaptic connections
     * @see getId() - For retrieving neuron ID
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor
     * 
     * Cleans up neuron resources and performs any necessary
     * cleanup operations. The neuron is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Neuron cannot be used after destruction
     */
    ~Neuron();
    
    // Disable copying - PImpl idiom
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get neuron identifier
     * 
     * Returns the unique identifier assigned to this neuron.
     * This ID is used for addressing neurons in the neural network
     * and is essential for synaptic connections and communication.
     * 
     * @return NeuronId Unique identifier
     * 
     * @post Return value is constant for neuron lifetime
     * @complexity O(1)
     * 
     * @see Neuron(NeuronId) - Constructor with ID
     */
    NeuronId getId() const;
    
    /**
     * @brief Get neuron type
     * 
     * Returns the neuron type, which determines its computational
     * properties and behavior. Different neuron types have different
     * biophysical parameters and computational roles.
     * 
     * @return NeuronType Neuron type
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setType() - For setting neuron type
     * @see NeuronType - Available neuron types
     */
    NeuronType getType() const;
    
    /**
     * @brief Set neuron type
     * 
     * Changes the neuron type, which affects its biophysical
     * parameters and computational properties. This is useful for:
     * - Creating specialized neuron populations
     * - Adjusting neuron properties for different roles
     * - Dynamic neuron type changes
     * - Experimental parameter studies
     * 
     * @param type New neuron type
     * 
     * @post Neuron type updated
     * @post Biophysical parameters adjusted
     * @post State updated for new type
     * 
     * @note Type changes affect membrane dynamics
     * @see getType() - For getting neuron type
     * @see NeuronType - Available neuron types
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get neuron state (const version)
     * 
     * Returns a const reference to the current neuron state.
     * This provides read-only access to the complete neuron state
     * without allowing modifications. Useful for inspection and
     * debugging.
     * 
     * @return Const reference to NeuronState
     * 
     * @post Return value valid as long as neuron exists
     * @complexity O(1)
     * 
     * @see getState() - Non-const version
     * @see NeuronState - State structure
     * @see reset() - For state reset
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get neuron state (non-const version)
     * 
     * Returns a non-const reference to the current neuron state.
     * This allows direct modification of neuron state, which is
     * useful for:
     * - Custom state initialization
     * - Debugging and analysis
     * - Advanced control algorithms
     * - State testing and verification
     * 
     * @return Reference to NeuronState
     * 
     * @post Return value valid as long as neuron exists
     * @complexity O(1)
     * 
     * @see getState() const - Const version
     * @see NeuronState - State structure
     * @see step() - For automatic state updates
     */
    NeuronState& getState();
    
    /**
     * @brief Get membrane potential
     * 
     * Returns the current membrane potential relative to the
     * resting potential. This is the primary variable that determines
     * neural activity and is the main output of integration.
     * 
     * @return MembranePotential Current membrane potential
     * 
     * @post Return value reflects current integration state
     * @complexity O(1)
     * 
     * @see setMembranePotential() - For setting potential
     * @see addToMembranePotential() - For modifying potential
     * @see checkThreshold() - For spike detection
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential
     * 
     * Directly sets the membrane potential to a specific value.
     * This is a low-level operation that bypasses the normal
     * integration process and is typically used for:
     * - State initialization
     * - Testing scenarios
     * - Debugging and analysis
     * - Custom control algorithms
     * 
     * @param potential New membrane potential
     * 
     * @pre potential must be physiologically reasonable
     * @post Membrane potential set directly
     * @post Integration state bypassed
     * 
     * @note Can interfere with normal neural dynamics
     * @see getMembranePotential() - For getting potential
     * @see addToMembranePotential() - For incremental changes
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to membrane potential
     * 
     * Incrementally adds a value to the current membrane potential.
     * This is the primary method for neural integration, representing
     * the sum of all synaptic inputs and injected currents.
     * 
     * @param delta Change in membrane potential (mV)
     * 
     * @pre delta should be reasonable for time step
     * @post Membrane potential increased by delta
     * @post Integration state updated
     * 
     * @note Used by synaptic inputs and current injection
     * @see setMembranePotential() - For direct setting
     * @see getMembranePotential() - For getting potential
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get firing threshold
     * 
     * Returns the membrane potential threshold that must be crossed
     * for the neuron to generate a spike. This determines the
     * excitability of the neuron.
     * 
     * @return MembranePotential Firing threshold
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setThreshold() - For setting threshold
     * @see checkThreshold() - For spike detection
     * @see getMembranePotential() - For comparison
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set firing threshold
     * 
     * Changes the firing threshold, affecting neuron excitability.
     * This is useful for:
     * - Adjusting neuron sensitivity
     * - Implementing learning rules
     * - Experimental parameter studies
     * - Dynamic threshold control
     * 
     * @param threshold New firing threshold
     * 
     * @post Firing threshold updated
     * @post Neuron excitability changed
     * @post Spike detection threshold adjusted
     * 
     * @note Threshold affects firing rate and dynamics
     * @see getThreshold() - For getting threshold
     * @see checkThreshold() - For spike detection
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is firing
     * 
     * Determines whether the neuron is currently in a firing state.
     * This is different from spike generation - it indicates whether
     * the neuron is actively producing spikes.
     * 
     * @return true if neuron is in firing state, false otherwise
     * 
     * @post Return value reflects current firing state
     * @complexity O(1)
     * 
     * @see isRefractory() - For refractory state
     * @see setFiringState() - For setting state
     * @see stepLIF() - For spike generation
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is refractory
     * 
     * Determines whether the neuron is in its refractory period,
     * during which it cannot generate spikes. This prevents excessive
     * firing rates and allows the neuron to recover after spiking.
     * 
     * @return true if neuron is in refractory period, false otherwise
     * 
     * @post Return value reflects refractory state
     * @complexity O(1)
     * 
     * @see isFiring() - For firing state
     * @see setRefractoryPeriod() - For setting refractory period
     * @see decrementRefractory() - For refractory decrement
     */
    bool isRefractory() const;
    
    /**
     * @brief Set firing state
     * 
     * Directly sets the firing state of the neuron. This is a
     * low-level operation that can be used for:
     * - State initialization
     * - Testing scenarios
     * - Custom control algorithms
     * - Debugging and analysis
     * 
     * @param state New firing state
     * 
     * @post Firing state updated
     * @post Spike generation affected
     * @post Neural dynamics changed
     * 
     * @note This bypasses normal state transitions
     * @see getFiringState() - For getting firing state
     * @see stepLIF() - For automatic state updates
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period
     * 
     * Changes the refractory period, which determines how long the
     * neuron must wait before it can fire again after a spike.
     * This affects the maximum firing rate of the neuron.
     * 
     * @param steps Number of simulation steps (must be positive)
     * 
     * @pre steps must be positive
     * @post Refractory period updated
     * @post Maximum firing rate affected
     * 
     * @note Longer refractory periods reduce firing rates
     * @see getRefractoryPeriod() - For getting refractory period
     * @see decrementRefractory() - For refractory decrement
     * @see isRefractory() - For refractory state check
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory period
     * 
     * Decrements the remaining refractory period by one step.
     * This is called automatically during simulation steps when
     * the neuron is in refractory mode.
     * 
     * @post Refractory period decreased by one
     * @post Neuron may become non-refractory
     * 
     * @note Called automatically by stepLIF()
     * @see isRefractory() - For refractory state
     * @see setRefractoryPeriod() - For setting period
     */
    void decrementRefractory();
    
    /**
     * @brief Get firing rate
     * 
     * Returns the current firing rate in Hertz (spikes per second).
     * This is used for rate-based computation and analysis of neural
     * activity patterns.
     * 
     * @return FiringRate Current firing rate
     * 
     * @post Return value reflects current firing activity
     * @complexity O(1)
     * 
     * @see setFiringRate() - For setting firing rate
     * @see getState() - For complete state
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set firing rate
     * 
     * Sets the firing rate directly, bypassing normal spike generation.
     * This is used for rate-based computation and is an alternative
     * to event-based spike generation.
     * 
     * @param rate Firing rate in Hertz (Hz)
     * 
     * @post Firing rate updated
     * @post Neural state affected
     * 
     * @note Used for rate-based neural computation
     * @see getFiringRate() - For getting firing rate
     * @see stepLIF() - For spike-based computation
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set leak conductance
     * 
     * Changes the leak conductance, which determines how quickly the
     * membrane potential decays toward the resting potential in the
     * absence of inputs. Higher conductance means faster decay.
     * 
     * @param conductance Leak conductance (nS)
     * 
     * @post Leak conductance updated
     * @post Membrane dynamics changed
     * 
     * @note Affects neuron membrane time constant
     * @see getLeakConductance() - For getting conductance
     * @see stepLIF() - For effect on dynamics
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get leak conductance
     * 
     * Returns the current leak conductance, which determines the
     * membrane time constant and decay rate.
     * 
     * @return Leak conductance (nS)
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setLeakConductance() - For setting conductance
     * @see stepLIF() - For effect on dynamics
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get refractory period
     * 
     * Returns the current refractory period in simulation steps.
     * 
     * @return Refractory period in steps
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setRefractoryPeriod() - For setting period
     * @see isRefractory() - For refractory state
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set resting potential
     * 
     * Changes the resting membrane potential, which is the equilibrium
     * voltage to which the neuron returns in the absence of inputs.
     * 
     * @param potential Resting potential (mV)
     * 
     * @post Resting potential updated
     * @post Neuron equilibrium changed
     * @post State updated
     * 
     * @note Affects neuron baseline
     * @see getRestingPotential() - For getting potential
     * @see setMembranePotential() - For potential manipulation
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get resting potential
     * 
     * Returns the resting membrane potential, which is the equilibrium
     * voltage to which the neuron returns in the absence of inputs.
     * 
     * @return Resting potential (mV)
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setRestingPotential() - For setting potential
     * @see getMembranePotential() - For current potential
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set reset potential
     * 
     * Changes the reset potential, which is the membrane potential
     * that the neuron resets to immediately after generating a spike.
     * This is typically the same as the resting potential.
     * 
     * @param potential Reset potential (mV)
     * 
     * @post Reset potential updated
     * @post Spike reset behavior changed
     * @post State updated
     * 
     * @note Typically equal to resting potential
     * @see getResetPotential() - For getting potential
     * @see stepLIF() - For reset effect
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get reset potential
     * 
     * Returns the reset potential, which is the membrane potential
     * that the neuron resets to immediately after generating a spike.
     * 
     * @return Reset potential (mV)
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setResetPotential() - For setting potential
     * @see stepLIF() - For reset behavior
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Check threshold for spike generation
     * 
     * Checks whether the current membrane potential has crossed the
     * firing threshold, indicating that the neuron should generate a
     * spike in the current timestep.
     * 
     * @return true if membrane potential >= threshold, false otherwise
     * 
     * @post Return value indicates spike condition
     * @complexity O(1)
     * 
     * @see getMembranePotential() - For membrane potential
     * @see getThreshold() - For threshold
     * @see stepLIF() - For spike generation
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get last spike time
     * 
     * Returns the timestamp of the last spike generated by this neuron.
     * If no spikes have been generated, returns -1.0.
     * 
     * @return Timestamp of last spike (-1.0 if no spikes)
     * 
     * @post Return value updates with each spike
     * @complexity O(1)
     * 
     * @see recordSpike() - For recording spikes
     * @see stepLIF() - For spike generation
     * @see getSpikeHistory() - For complete history
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief LIF step function
     * 
     * Implements one timestep of Leaky Integrate-and-Fire (LIF) dynamics.
     * This is the core computation function that:
     * 1. Applies membrane potential dynamics
     * 2. Checks for threshold crossing
     * 3. Generates spikes if needed
     * 4. Updates refractory state
     * 5. Applies adaptation
     * 6. Updates spike history
     * 
     * @param currentTime Current simulation time (ms)
     * @param dt Timestep duration (ms)
     * @return true if neuron fired this timestep, false otherwise
     * 
     * @pre currentTime must be non-negative
     * @pre dt must be positive
     * @post Membrane potential updated (with leak)
     * @post Spike generated if threshold crossed
     * @post Refractory state updated
     * @post Spike history updated
     * 
     * @note Primary computation function for neuron dynamics
     * @note Called once per timestep by simulation
     * @see getMembranePotential() - For membrane potential
     * @see checkThreshold() - For spike detection
     * @see step() - Alternative timestep function
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory input
     * 
     * Injects excitatory synaptic current into the neuron, causing
     * depolarization of the membrane potential. Excitatory inputs
     * typically increase the likelihood of spike generation.
     * 
     * @param amplitude Input amplitude (mV)
     * 
     * @pre amplitude should be positive
     * @post Membrane potential increased
     * @post Neuron excitability increased
     * 
     * @note Represents glutamatergic input
     * @see receiveInhibitoryInput() - For inhibitory input
     * @see receiveModulatoryInput() - For modulatory input
     * @see addToMembranePotential() - For direct potential change
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory input
     * 
     * Injects inhibitory synaptic current into the neuron, causing
     * hyperpolarization of the membrane potential. Inhibitory inputs
     * decrease the likelihood of spike generation and stabilize
     * neural activity.
     * 
     * @param amplitude Input amplitude (mV)
     * 
     * @pre amplitude should be positive
     * @post Membrane potential decreased
     * @post Neuron excitability decreased
     * 
     * @note Represents GABAergic input
     * @see receiveExcitatoryInput() - For excitatory input
     * @see receiveModulatoryInput() - For modulatory input
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory input
     * 
     * Injects modulatory current into the neuron, which can affect
     * neuronal excitability, gain, and plasticity. Modulatory inputs
     * are typically slower and more diffuse than excitatory/inhibitory
     * inputs.
     * 
     * @param amplitude Input amplitude (mV)
     * 
     * @pre amplitude can be positive or negative
     * @post Neuronal excitability modulated
     * @post Plasticity potentially affected
     * 
     * @note Represents neuromodulatory input (dopamine, acetylcholine, etc.)
     * @see receiveExcitatoryInput() - For excitatory input
     * @see receiveInhibitoryInput() - For inhibitory input
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current
     * 
     * Injects current directly into the neuron from an external source.
     * This is used for:
     * - Stimulation protocols
     * - Experimental control
     * - Artificial driving of neural activity
     * - Implementation of external inputs
     * 
     * @param current Current amount (mV)
     * 
     * @pre current should be physiologically reasonable
     * @post Membrane potential increased
     * @post Neuron driven by external input
     * 
     * @see getTotalCurrent() - For getting injected current
     * @see clearTotalCurrent() - For clearing current
     * @see receiveExcitatoryInput() - For synaptic input
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total injected current
     * 
     * Returns the total amount of current injected directly into the
     * neuron. This is the sum of all external current injections.
     * 
     * @return Total injected current (mV)
     * 
     * @post Return value accumulates with injections
     * @complexity O(1)
     * 
     * @see injectCurrent() - For injecting current
     * @see clearTotalCurrent() - For clearing current
     * @see receiveExcitatoryInput() - For synaptic input
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear total injected current
     * 
     * Clears the total injected current to zero. This is typically
     * called at the beginning of each timestep to reset accumulated
     * external current for the new timestep.
     * 
     * @post Total injected current set to 0
     * @post Future injections will accumulate anew
     * 
     * @see injectCurrent() - For injecting current
     * @see getTotalCurrent() - For getting current
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record spike timestamp
     * 
     * Records the current timestamp in the spike history, enabling
     * spike-timing-dependent plasticity (STDP) and other spike-timing
     * dependent computations. This is essential for learning.
     * 
     * @param timestamp Time when spike occurred (ms)
     * 
     * @post Spike timestamp added to history
     * @post STDP computations enabled
     * 
     * @note Spike history used by STDP and other learning rules
     * @see getSpikeHistory() - For accessing spike history
     * @see clearSpikeHistory() - For clearing history
     * @see stepLIF() - For automatic spike recording
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get spike history
     * 
     * Returns a reference to the spike history vector, which contains
     * timestamps of all spikes generated by this neuron. This is used
     * for spike-timing-dependent plasticity (STDP) and other
     * spike-timing dependent computations.
     * 
     * @return Const reference to spike history vector
     * 
     * @post Return value updates with each spike
     * @complexity O(1) for reference, O(n) for copying
     * 
     * @note Critical for STDP and learning
     * @see recordSpike() - For recording spikes
     * @see clearSpikeHistory() - For clearing history
     * @see STDP - For STDP implementation
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * 
     * Clears the spike history vector, removing all recorded spike
     * timestamps. This is used to:
     * - Reset STDP computations
     * - Reset learning processes
     * - Prepare for new learning epochs
     * - Debugging and testing
     * 
     * @post Spike history cleared
     * @post STDP computations reset
     * 
     * @see recordSpike() - For recording spikes
     * @see getSpikeHistory() - For accessing history
     * @see STDP - For STDP usage
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse
     * 
     * Registers an incoming synapse to this neuron, establishing
     * synaptic connectivity. This is called by synapses when they
     * are connected to this neuron.
     * 
     * @param handle Synapse handle identifying the connection
     * 
     * @post Synapse added to incoming list
     * @post Connectivity established
     * 
     * @note Maintains synaptic connectivity graph
     * @see addOutgoingSynapse() - For outgoing synapses
     * @see Synapse - For synapse class
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse
     * 
     * Registers an outgoing synapse from this neuron, establishing
     * synaptic connectivity. This is called by synapses when they
     * are connected from this neuron.
     * 
     * @param handle Synapse handle identifying the connection
     * 
     * @post Synapse added to outgoing list
     * @post Connectivity established
     * 
     * @note Maintains synaptic connectivity graph
     * @see addIncomingSynapse() - For incoming synapses
     * @see Synapse - For synapse class
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get incoming synapses
     * 
     * Returns a const reference to the vector of incoming synapse
     * handles. This provides access to all synapses that connect
     * to this neuron.
     * 
     * @return Const reference to incoming synapse handles
     * 
     * @post Return value stable during simulation
     * @complexity O(1) for reference, O(n) for copying
     * 
     * @note Essential for synaptic connectivity analysis
     * @see addIncomingSynapse() - For adding synapses
     * @see getOutgoingSynapses() - For outgoing synapses
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get outgoing synapses
     * 
     * Returns a const reference to the vector of outgoing synapse
     * handles. This provides access to all synapses that originate
     * from this neuron.
     * 
     * @return Const reference to outgoing synapse handles
     * 
     * @post Return value stable during simulation
     * @complexity O(1) for reference, O(n) for copying
     * 
     * @note Essential for synaptic connectivity analysis
     * @see addOutgoingSynapse() - For adding synapses
     * @see getIncomingSynapses() - For incoming synapses
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity flags
     * 
     * Returns a const reference to the plasticity flags for this neuron,
     * which determine which plasticity rules are enabled for this neuron.
     * This is essential for controlling learning processes.
     * 
     * @return Const reference to plasticity flags
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see PlasticityFlags - Flags structure
     * @see enablePlasticity() - For setting plasticity
     * @see getPlasticityFlags() - Non-const version
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get plasticity flags (non-const version)
     * 
     * Returns a non-const reference to the plasticity flags for this
     * neuron, allowing direct modification of plasticity settings.
     * 
     * @return Reference to plasticity flags
     * 
     * @post Return value valid as long as neuron exists
     * @complexity O(1)
     * 
     * @see getPlasticityFlags() const - Const version
     * @see PlasticityFlags - Flags structure
     * @see enablePlasticity() - For setting plasticity
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable plasticity rules
     * 
     * Enables or disables specific plasticity rules for this neuron.
     * This controls which learning mechanisms are active for the
     * neuron, allowing fine-grained control over neural learning.
     * 
     * @param hebbian Enable Hebbian learning (neurons that fire together)
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     * 
     * @post Plasticity flags updated
     * @post Learning rules enabled/disabled
     * 
     * @note Multiple plasticity rules can be enabled simultaneously
     * @see getPlasticityFlags() - For accessing flags
     * @see STDP - For spike-timing-dependent plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set region ID
     * 
     * Assigns this neuron to a specific neural region. Regions are
     * specialized neural populations that perform specific
     * computations or represent specific information. This establishes
     * the neuron's organizational structure.
     * 
     * @param region Region ID to assign
     * 
     * @post Neuron assigned to region
     * @post Regional connectivity established
     * 
     * @note Regions organize neurons into functional groups
     * @see getRegionId() - For getting region ID
     * @see NeuralRegion - For region class
     * @see setPopulationId() - For population assignment
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region ID
     * 
     * Returns the region ID assigned to this neuron. This identifies
     * which neural region the neuron belongs to.
     * 
     * @return Region ID
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setRegionId() - For setting region
     * @see NeuralRegion - For region class
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population ID
     * 
     * Assigns this neuron to a specific neural population within its
     * region. Populations are specialized groups of neurons that
     * perform similar computations or represent similar information.
     * 
     * @param population Population ID to assign
     * 
     * @post Neuron assigned to population
     * @post Population membership established
     * 
     * @note Populations provide finer-grained organization
     * @see getPopulationId() - For getting population ID
     * @see NeuralPopulation - For population class
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population ID
     * 
     * Returns the population ID assigned to this neuron. This identifies
     * which neural population the neuron belongs to within its region.
     * 
     * @return Population ID
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setPopulationId() - For setting population
     * @see NeuralPopulation - For population class
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step
     * 
     * Alternative timestep function that updates the neuron for one
     * simulation step using the current simulation time. This is
     * a convenience function that calls stepLIF() with the appropriate
     * timestep duration.
     * 
     * @param currentTime Current simulation time (ms)
     * 
     * @pre currentTime must be non-negative
     * @post Neuron state updated for timestep
     * @post Spike generation completed
     * 
     * @note Calls stepLIF() internally
     * @see stepLIF() - For LIF dynamics
     * @see update() - Alternative timestep function
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     * 
     * Resets the neuron to its initial state, undoing all changes
     * accumulated during simulation. This includes:
     * - Membrane potential reset to resting
     * - Firing state reset to resting
     * - Refractory period cleared
     * - Spike history cleared
     * - Adaptation variables reset
     * - Synaptic connectivity preserved
     * 
     * @post Neuron returned to initial state
     * @post Ready for new simulation
     * @post All dynamic variables reset
     * 
     * @note Synaptic connections preserved
     * @see initializeRandom() - For custom initialization
     * @see step() - For simulation stepping
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random parameters
     * 
     * Initializes the neuron with random biophysical parameters,
     * typically used for generating biologically realistic neural
     * networks. This function randomizes various parameters to create
     * diversity across neurons while maintaining functional properties.
     * 
     * @param rng Random number generator for reproducible initialization
     * 
     * @pre rng must be valid and seeded
     * @post Neuron parameters randomized
     * @post State initialized with random values
     * 
     * @note Used for creating diverse neural populations
     * @see reset() - For state reset without parameter changes
     * @see RandomGenerator - For random number generation
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
