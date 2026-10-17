/**
 * @file Neuron.hpp
 * @brief Neuron class representing a single biological neuron
 * @namespace nlm
 * 
 * This file implements the Leaky Integrate-and-Fire (LIF) neuron model,
 * the fundamental computational unit in the NLM neural simulation framework.
 * Neurons receive synaptic inputs, integrate their membrane potential,
 * and generate action potentials (spikes) when reaching threshold.
 * 
 * The neuron implementation follows biological principles of neuronal dynamics,
 * including refractory periods, spike-frequency adaptation, and conductance-based
 * synaptic integration. Each neuron maintains state information for simulation,
 * plasticity, and event tracking.
 * 
 * @note Neuron.hpp provides the core neural processing unit for building
 *       larger brain models and neural networks. The class is designed for
 *       efficiency in both memory usage and computational performance.
 * 
 * @see Synapse.hpp - Synaptic connections between neurons
 * @see PlasticityRule.hpp - Rules for modifying synaptic weights
 * @see Neuromodulator.hpp - Neuromodulatory signals affecting plasticity
 * @see Brain.hpp - Collections of neurons organized into regions and populations
 */

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
 * @brief Complete state snapshot of a neuron at a given simulation time
 * @namespace nlm
 * 
 * This structure stores all state variables needed to simulate neuronal dynamics.
 * It is designed for efficient memory layout and access patterns during simulation.
 * The structure contains membrane potential dynamics, firing state, and plasticity
 * related information.
 * 
 * @note NeuronState is a value type that can be copied and assigned. It is used
 *       by reference in the Neuron class to maintain state across simulation steps.
 *       The layout is optimized for cache-friendly access during neural computation.
 */
struct NeuronState {
    /**
     * @brief Current membrane potential of the neuron (mV)
     * 
     * This represents the electrical potential difference across the neuronal membrane.
     * Negative values indicate hyperpolarization (reduced excitability),
     * positive values indicate depolarization (increased excitability).
     * 
     * @note The membrane potential is the primary integration variable in LIF dynamics.
     *       It changes in response to synaptic inputs and intrinsic membrane properties.
     */
    MembranePotential membranePotential;    // Current membrane potential (mV)
    
    /**
     * @brief Resting potential of the neuron (mV)
     * 
     * This is the equilibrium membrane potential when no inputs are present.
     * Typical values range from -70mV to -65mV in biological neurons.
     * 
     * @note Resting potential provides the baseline toward which the membrane
     *       potential naturally decays due to leak conductances.
     */
    MembranePotential restingPotential;    // Resting potential (mV)
    
    /**
     * @brief Firing threshold for spike generation (mV)
     * 
     * When the membrane potential reaches or exceeds this value, the neuron
     * generates an action potential (spike). Threshold values are typically
     * between -55mV and -40mV in biological neurons.
     * 
     * @note Crossing this threshold triggers the spike detection mechanism
     *       and initiates the refractory period.
     */
    MembranePotential threshold;           // Firing threshold (mV)
    
    /**
     * @brief Membrane potential after spike reset (mV)
     * 
     * This value is set as the membrane potential immediately following a spike.
     * Reset potentials are typically close to the resting potential (around -70mV).
     * 
     * @note The reset potential implements the absolute refractory period by
     *       temporarily clamping the membrane potential below threshold.
     */
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    
    /**
     * @brief Leak conductance of the neuronal membrane (nS)
     * 
     * This represents the passive ion channel conductance that causes the
     * membrane potential to decay toward the resting potential.
     * 
     * @note The leak conductance determines the time constant of membrane
     *       potential decay and influences the neuron's integration properties.
     */
    MembranePotential leakConductance;      // Leak conductance (nS)
    
    /**
     * @brief Total synaptic conductance from incoming connections (nS)
     * 
     * This represents the net effect of all synaptic inputs on the neuron's
     * membrane potential. It is the sum of excitatory and inhibitory conductances.
     * 
     * @note Synaptic conductance is converted to current using the driving force
     *       (difference between membrane potential and reversal potential).
     */
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    
    /**
     * @brief Firing rate for rate-based computation and homeostasis (Hz)
     * 
     * This represents the instantaneous firing frequency of the neuron, computed
     * from spike history or used as a target for adaptive processes.
     * 
     * @note Firing rate provides a continuous measure of neuronal activity that
     *       can be used for population coding, homeostasis, and learning rules.
     */
    FiringRate firingRate;                 // Current firing rate (Hz)
    
    /**
     * @brief Current firing state of the neuron
     * 
     * This enumerated state tracks whether the neuron is resting, active,
     * or in a refractory period following a spike.
     * 
     * @note Firing state influences synaptic integration (neurons in refractory
     *       periods typically do not receive or integrate new inputs).
     */
    FiringState firingState;               // Current firing state
    
    /**
     * @brief Steps remaining in the absolute refractory period
     * 
     * During the refractory period, the neuron cannot generate additional spikes
     * regardless of input. This counter decreases with each simulation step
     * until reaching zero.
     * 
     * @note The refractory period enforces biological realism and prevents
     *       unrealistic firing patterns. It also affects synaptic integration
     *       during this period.
     */
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    
    /**
     * @brief Total refractory period duration (steps)
     * 
     * This specifies how many simulation steps the neuron will be refractory
     * after generating a spike. Typical values range from 2-10 steps
     * depending on neuron type.
     * 
     * @note Refractory period duration influences the maximum firing frequency
     *       and is an important parameter in neural coding and information processing.
     */
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    
    /**
     * @brief Spike-frequency adaptation variable (dimensionless)
     * 
     * This implements spike-frequency adaptation, where increased firing rates
     * lead to gradual decreases in excitability over time.
     * 
     * @note Adaptation provides homeostatic regulation of neuronal firing,
     *       preventing runaway excitation and stabilizing network activity.
     */
    float adaptationVariable;              // For spike-frequency adaptation
    
    /**
     * @brief Timestamp of the last spike (s), -1.0 if no spike has occurred
     * 
     * This records the simulation time of the most recent action potential.
     * The value -1.0 is used as a sentinel to indicate that the neuron has never
     * fired (initial state).
     * 
     * @note Spike history is used for spike-timing-dependent plasticity (STDP),
     *       synaptic delay effects, and various learning algorithms.
     */
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    /**
     * @brief Default constructor initializing neuron to resting state
     * 
     * Sets the neuron to biologically plausible initial conditions: resting
     * potential as baseline, typical threshold values, and zero adaptation.
     * All spike history and synaptic inputs are cleared.
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
     * @brief Implementation of a Leaky Integrate-and-Fire (LIF) neuron model
     * @namespace nlm
     * 
     * This class represents a single biological neuron with LIF dynamics, modeling
     * the essential behavior of neurons in biological neural networks. The neuron
     * receives synaptic inputs, integrates its membrane potential, and generates
     * action potentials when threshold is reached.
     * 
     * The neuron implementation includes key biological mechanisms: leaky membrane
     * dynamics for temporal integration, absolute refractory periods after spikes,
     * spike-frequency adaptation, and support for synaptic plasticity. Each neuron
     * maintains state information for both its intrinsic dynamics and its position
     * in the network topology.
     * 
     * @note Neurons are memory-efficient objects using the Pimpl idiom to hide
     *       implementation details and allow dynamic configuration. They are designed
     *       for high-performance simulation of large-scale neural networks.
     * 
     * @see Synapse - Synaptic connections from/to this neuron
     * @see PlasticityRule - Rules for modifying synaptic weights
     * @see Neuromodulator - Neuromodulatory signals affecting plasticity
     * @see Brain - Collections of neurons organized into regions and populations
     * @see RandomGenerator - For stochastic initialization and random inputs
     */
class Neuron {
public:
    /**
     * @brief Constructor creating a neuron with a unique identifier
     * 
     * Creates a new neuron initialized with default biological parameters.
     * The neuron is assigned the given ID and placed in an initial resting state
     * with biologically plausible membrane properties.
     * 
     * @param id Unique identifier for this neuron (must be valid and unique)
     * 
     * @note Neuron IDs should be assigned by the neural network to ensure
     *       uniqueness. The ID is used for synaptic addressing and network topology.
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Virtual destructor for proper cleanup of Pimpl implementation
     */
    ~Neuron();
    
    /**
     * @brief Disable copy construction (neurons are not copyable)
     * 
     * Neurons cannot be copied due to the Pimpl idiom and internal state management.
     * Attempting to copy a neuron will result in a compile-time error.
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Disable copy assignment (neurons are not assignable)
     * 
     * Neurons cannot be assigned due to the Pimpl idiom and internal state management.
     * Attempting to assign to a neuron will result in a compile-time error.
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor (enables efficient neuron transfer)
     * 
     * Move constructor allows transferring neuron ownership without deep copying.
     * After moving, the source neuron is left in a valid but unspecified state.
     * 
     * @param other Neuron to move from (left in valid but unspecified state)
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator (enables efficient neuron transfer)
     * 
     * Move assignment allows transferring neuron ownership without deep copying.
     * After assignment, the source neuron is left in a valid but unspecified state.
     * 
     * @param other Neuron to move from (left in valid but unspecified state)
     * @return Reference to this neuron after assignment
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * 
     * @return NeuronId identifier for this neuron
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron type classification
     * 
     * @return NeuronType type of this neuron (e.g., Excitatory, Inhibitory)
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type classification
     * 
     * @param type Neuron type to assign (e.g., Excitatory, Inhibitory)
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the complete state snapshot of this neuron
     * 
     * @return const reference to the neuron's current state
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to the neuron's state
     * 
     * @return Reference to the neuron's current state (for direct modification)
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * 
     * @return MembranePotential current membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential directly
     * 
     * @param potential New membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add a delta value to the current membrane potential
     * 
     * @param delta Value to add to membrane potential (can be negative)
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold voltage
     * 
     * @return MembranePotential firing threshold in mV
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
     * @return bool true if neuron is in active firing state, false otherwise
     */
    bool isFiring() const;
    
    /**
     * @brief Check if the neuron is in refractory period
     * 
     * @return bool true if neuron cannot fire due to absolute refractory period
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the firing state of the neuron
     * 
     * @param state New firing state (Resting, Active, or Refractory)
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the duration of the refractory period
     * 
     * @param steps Number of simulation steps for the refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrease the remaining refractory period counter
     * 
     * This should be called once per simulation step. When the refractory
     * period reaches zero, the neuron can fire again.
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate
     * 
     * @return FiringRate current firing rate in Hz
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate
     * 
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance for membrane dynamics
     * 
     * The leak conductance determines how strongly the membrane potential
     * decays toward the resting potential over time.
     * 
     * @param conductance Leak conductance in nS (typically 1-100 nS)
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the current leak conductance
     * 
     * @return MembranePotential leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period duration
     * 
     * @return uint32_t refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential of the neuron
     * 
     * @param potential Resting potential in mV (typically -70mV)
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential
     * 
     * @return MembranePotential resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential after spiking
     * 
     * @param potential Reset potential in mV (typically close to resting)
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get the reset potential
     * 
     * @return MembranePotential reset potential in mV
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Check if membrane potential has reached firing threshold
     * 
     * @return bool true if neuron should fire (membrane potential >= threshold)
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the most recent spike
     * 
     * @return float Timestamp of last spike, -1.0 if no spike has occurred
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform one step of LIF integration with real membrane dynamics
     * 
     * This implements the complete Leaky Integrate-and-Fire dynamics for one
     * simulation timestep, including membrane potential integration, spike
     * detection, and biological adaptations. Returns true if a spike occurred
     * during this step.
     * 
     * @param currentTime Current simulation time in seconds
     * @param dt Time step duration in seconds
     * @return bool true if neuron fired during this step, false otherwise
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * Excitatory inputs increase the neuron's membrane potential toward depolarization.
     * This represents the effect of glutamatergic synapses in biological neurons.
     * 
     * @param amplitude Synaptic input strength (positive mV contribution)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * Inhibitory inputs decrease the neuron's membrane potential toward hyperpolarization.
     * This represents the effect of GABAergic synapses in biological neurons.
     * 
     * @param amplitude Synaptic input strength (negative mV contribution)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * Modulatory inputs affect neuronal plasticity and adaptation rather than
     * directly changing membrane potential. This represents neuromodulatory synapses
     * like dopaminergic or cholinergic inputs.
     * 
     * @param amplitude Modulatory input strength (affects plasticity)
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current input from non-synaptic sources
     * 
     * External current inputs can come from sensory inputs, intrinsic neuronal
     * dynamics, or other non-synaptic sources. This is added to synaptic inputs
     * for membrane potential integration.
     * 
     * @param current External current injection (positive for depolarization)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total current input for this simulation step
     * 
     * @return MembranePotential total current input (synaptic + external)
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear accumulated current inputs for the next simulation step
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike event for plasticity and learning
     * 
     * This should be called when the neuron fires, recording the precise
     * timing of the spike for use in STDP and other spike-timing dependent
     * plasticity rules.
     * 
     * @param timestamp Time when the spike occurred (simulation time)
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the history of recent spike times
     * 
     * @return const reference to vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear all recorded spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add an incoming synapse to this neuron
     * 
     * @param handle Handle to the incoming synapse (connecting to this neuron)
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse from this neuron
     * 
     * @param handle Handle to the outgoing synapse (originating from this neuron)
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapses to this neuron
     * 
     * @return const reference to vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapses from this neuron
     * 
     * @return const reference to vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the plasticity flags for this neuron
     * 
     * @return const reference to plasticity configuration flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to plasticity flags
     * 
     * @return Reference to plasticity configuration flags (for direct modification)
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Configure which plasticity mechanisms are enabled for this neuron
     * 
     * This sets the plasticity flags that determine which plasticity rules
     * will be applied to synapses connected to this neuron.
     * 
     * @param hebbian Enable Hebbian learning ("fire together, wire together")
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the brain region identifier for this neuron
     * 
     * @param region Region identifier this neuron belongs to
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the brain region identifier for this neuron
     * 
     * @return RegionId identifier of the region this neuron belongs to
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the population identifier for this neuron
     * 
     * @param population Population identifier this neuron belongs to
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the population identifier for this neuron
     * 
     * @return PopulationId identifier of the population this neuron belongs to
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Perform one complete simulation step
     * 
     * This method integrates the neuron over one simulation timestep using
     * the current membrane dynamics and synaptic inputs. It calls the LIF
     * integration function and updates the internal state accordingly.
     * 
     * @note PHASE 2: Enhanced implementation with biological realism and
     *       stochastic components is planned for future development. This
     *       placeholder provides the basic stepping functionality with
     *       configurable timestep for LIF dynamics.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset the neuron to its initial state
     * 
     * This clears all internal state variables and resets the neuron to its
     * default initialization state. After reset, the neuron will not fire
     * until it receives appropriate inputs and integrates to threshold.
     */
    void reset();
    
    /**
     * @brief Initialize neuron parameters with random biological values
     * 
     * This sets up the neuron with biologically plausible but stochastic
     * parameters, mimicking natural variations in neuronal properties.
     * The initialization includes membrane potentials, thresholds, and
     * plasticity-related parameters drawn from appropriate distributions.
     * 
     * @param rng Reference to random number generator for stochastic initialization
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Pimpl implementation structure (pointer to implementation)
     * 
     * The Neuron class uses the Pimpl idiom to hide implementation details
     * and allow dynamic configuration. All actual implementation is contained
     * in this private structure, which is allocated on the heap.
     */
    struct Impl;
    
    /**
     * @brief Pointer to the implementation structure
     */
    Impl* pImpl;
};

} // namespace nlm
