// Neuron.hpp - Improved Documentation

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <string>

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
        // Phase 6: 9 steps, not 5. One action presentation is 10
        // brain steps, so a 5-step refractory let a neuron fire
        // twice inside a single burst; 9 steps caps it at about
        // one spike per presentation, which is the population's
        // natural burst rhythm anyway. This lowers the reported
        // rate without changing any drive constant (drive
        // scaling was tried and reverted: -30% drive silenced
        // the network entirely, 0 Hz, because this is a
        // threshold system with no gentle middle ground).
        , refractoryPeriod(9)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

// Detailed neuron documentation comment
/**
 * @brief Neuron class representing a single neuron in the NLM brain.
 * 
 * This class implements the Leaky Integrate-and-Fire (LIF) neuron model,
 * which is a fundamental component of the NLM neural simulation framework.
 * LIF neurons are mathematically tractable and computationally efficient
 * while still capturing essential neural dynamics including:
 * - Membrane potential integration
 * - Threshold-based firing
 * - Refractory periods
 * - Spike-frequency adaptation
 * - Synaptic input integration
 * 
 * The neuron operates as a coincidence detector that transforms incoming
 * synaptic currents into discrete action potentials (spikes), which are
 * then propagated through the network via synaptic connections.
 * 
 * @details **Physical Modeling**
 * - Based on the classic Hodgkin-Huxley model simplified to LIF dynamics
 * - Membrane potential V(t) follows: dV/dt = (V_rest - V)/τ + I/C
 * - τ represents membrane time constant (~20ms for typical neurons)
 * - C represents membrane capacitance (~1nF for typical neurons)
 * - Threshold typically around -55mV, resting potential around -70mV
 * 
 * @details **Behavioral Properties**
 * - Homeostatic balance between excitation and inhibition
 * - Short-term plasticity through synaptic depression/facilitation
 * - Long-term plasticity through STDP and Hebbian mechanisms
 * - Developmental changes affecting plasticity rates and connectivity
 * 
 * @warning This is a simplified neuron model. For detailed biophysical
 * modeling, consider Hodgkin-Huxley models with ion channel dynamics.
 */
class Neuron {
public:
    /**
     * @brief Constructs a neuron with a unique identifier.
     * 
     * @param[in] id Unique identifier for this neuron within the brain.
     * 
     * @pre The neuron ID must be valid within the brain's region/population
     *      indexing scheme.
     * 
     * @note Neuron IDs are managed internally by the brain and regions to
     *       ensure uniqueness across the entire neural network.
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor.
     * 
     * Properly cleans up the neuron's internal resources and memory.
     */
    ~Neuron();
    
    /**
     * @brief Copy operations are disabled for memory safety.
     * 
     * Neurons use pointer-to-implementation (pImpl) pattern, so copying
     * would lead to double-free issues. Use move operations instead.
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor for efficient neuron transfer.
     * 
     * Allows neurons to be moved between containers without copying,
     * improving performance in dynamic allocation scenarios.
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator for efficient neuron transfer.
     * 
     * Allows neurons to be moved between containers without copying,
     * improving performance in dynamic allocation scenarios.
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Retrieves the neuron's unique identifier.
     * 
     * @return const NeuronId& Reference to the neuron's identifier.
     * 
     * @note This ID is used for referencing neurons in synaptic connections
     *       and for looking up neurons in brain data structures.
     */
    NeuronId getId() const;
    
    /**
     * @brief Retrieves the neuron's functional type.
     * 
     * @return NeuronType The neuron's type (excitatory, inhibitory, etc.).
     * 
     * @note Neuron type determines synaptic output characteristics and
     *       influences network dynamics through E/I balance.
     */
    NeuronType getType() const;
    
    /**
     * @brief Sets the neuron's functional type.
     * 
     * @param[in] type The new neuron type to set.
     * 
     * @note Changing neuron type affects synaptic output and network
     *       behavior. Use with caution as it may disrupt established
     *       connectivity patterns.
     */
    void setType(NeuronType type);
    
    /**
     * @brief Accesses the complete neuron state structure.
     * 
     * @return const NeuronState& Const reference to the state structure.
     * 
     * @note This provides read-only access to all neuron membrane and
     *       spiking state variables. For modifications, use individual
     *       setter methods or getState() for mutable access.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Accesses the complete neuron state structure (mutable).
     * 
     * @return NeuronState& Mutable reference to the state structure.
     * 
     * @warning Modifying state directly bypasses some validation checks
     *          that individual setter methods perform. Use individual
     *          setters for normal operations.
     */
    NeuronState& getState();
    
    /**
     * @brief Retrieves the current membrane potential.
     * 
     * @return MembranePotential Current membrane potential in mV.
     * 
     * @note Typical range: -100mV (hyperpolarized) to +50mV (depolarized).
     *       Threshold is typically around -55mV.
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Sets the absolute membrane potential.
     * 
     * @param[in] potential New membrane potential in mV.
     * 
     * @warning This bypasses some validation that gradual changes provide.
     *          Use addToMembranePotential() for more realistic synaptic inputs.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Adds delta to the membrane potential (simulates synaptic input).
     * 
     * @param[in] delta Change in membrane potential in mV.
     * 
     * @note This is the primary method for simulating synaptic inputs.
     *       Positive delta represents excitatory input, negative represents
     *       inhibitory input.
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Retrieves the current firing threshold.
     * 
     * @return MembranePotential Current firing threshold in mV.
     * 
     * @note Threshold determines when the neuron will fire. Lower threshold
     *       makes the neuron more likely to fire, higher threshold makes it
     *       less likely.
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Sets the firing threshold.
     * 
     * @param[in] threshold New firing threshold in mV.
     * 
     * @note Changing threshold affects neuronal excitability and network
     *       dynamics. Typical range: -60mV to -40mV.
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Checks if the neuron is currently firing an action potential.
     * 
     * @return bool True if neuron is in active firing state, false otherwise.
     * 
     * @note This is a high-level state check. For detailed spike timing,
     *       use getLastSpikeTime() and check spike history.
     */
    bool isFiring() const;
    
    /**
     * @brief Checks if the neuron is in refractory period.
     * 
     * @return bool True if neuron is refractory (cannot fire), false otherwise.
     * 
     * @note During refractory period, the neuron cannot generate new spikes
     *       and has its membrane potential reset to resting state.
     */
    bool isRefractory() const;
    
    /**
     * @brief Sets the neuron's firing state.
     * 
     * @param[in] state The new firing state.
     * 
     * @warning This bypasses automatic state transitions that normally
     *          occur during neuronal dynamics. Use the stepLIF() method
     *          for normal state transitions.
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Sets the refractory period duration.
     * 
     * @param[in] steps Number of simulation steps the neuron will be refractory.
     * 
     * @note Typical refractory periods: 2-10 steps (2-10ms at 1ms timestep).
     *       Longer refractory periods reduce maximum firing rate.
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrements refractory period counter.
     * 
     * @note This is typically called automatically during neuron stepping.
     *       When refractory period reaches zero, neuron becomes eligible to fire.
     */
    void decrementRefractory();
    
    /**
     * @brief Retrieves the neuron's current firing rate.
     * 
     * @return FiringRate Current firing rate in Hz (spikes per second).
     * 
     * @note This is a rate-based metric computed from spike history and
     *       simulation timing, providing a smoothed representation of
     *       neuronal activity.
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Sets the neuron's firing rate directly.
     * 
     * @param[in] rate New firing rate in Hz.
     * 
     * @warning This bypasses normal spike generation mechanisms and should
     *          only be used for testing or special simulation scenarios.
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Sets the neuron's leak conductance parameter.
     * 
     * @param[in] conductance Leak conductance in nS.
     * 
     * @note Controls how quickly membrane potential returns to resting
     *       level in absence of input. Typical values: 5-20 nS.
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Retrieves the neuron's leak conductance.
     * 
     * @return MembranePotential Leak conductance in nS.
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Retrieves the neuron's refractory period.
     * 
     * @return uint32_t Refractory period in simulation steps.
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Sets the neuron's resting potential.
     * 
     * @param[in] potential Resting potential in mV.
     * 
     * @note Typical value: -70mV. Changes affect neuron's equilibrium potential.
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Retrieves the neuron's resting potential.
     * 
     * @return MembranePotential Resting potential in mV.
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Sets the neuron's reset potential.
     * 
     * @param[in] potential Reset potential in mV (typically close to resting).
     * 
     * @note After firing, membrane potential is reset to this value.
     *       Usually slightly above resting potential to prevent immediate
     *       re-firing.
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Checks if membrane potential exceeds firing threshold.
     * 
     * @return bool True if neuron should fire, false otherwise.
     * 
     * @note This is a synchronous threshold check used in spike generation.
     *       Real neurons have more complex dynamics, but this approximation
     *       is computationally efficient and biologically plausible.
     */
    bool checkThreshold() const;
    
    /**
     * @brief Retrieves the timestamp of the last spike.
     * 
     * @return float Timestamp of last spike in simulation time units.
     *         Returns -1.0f if neuron has never spiked.
     * 
     * @note Useful for measuring inter-spike intervals and firing patterns.
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Performs one step of Leaky Integrate-and-Fire dynamics.
     * 
     * @param[in] currentTime Current simulation timestamp.
     * @param[in] dt Simulation timestep in seconds.
     * 
     * @return bool True if neuron fired during this step, false otherwise.
     * 
     * @details This implements the core LIF dynamics:
     * 1. Integrate synaptic inputs: V += (I/C) * dt
     * 2. Apply leak: V += (V_rest - V)/τ * dt
     * 3. Check threshold and fire if crossed
     * 4. Reset membrane potential and enter refractory period
     * 
     * @note This is the main computation performed by neurons during
     *       simulation. Most other methods provide access to neuron state
     *       for analysis or external interaction.
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receives excitatory synaptic input.
     * 
     * @param[in] amplitude Synaptic input amplitude in nS * mV.
     * 
     * @note Excitatory input increases membrane potential toward firing threshold.
     *       Typically represents glutamate-mediated AMPA/NMDA receptor activation.
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives inhibitory synaptic input.
     * 
     * @param[in] amplitude Synaptic input amplitude in nS * mV.
     * 
     * @note Inhibitory input decreases membrane potential away from firing threshold.
     *       Typically represents GABA-mediated receptor activation.
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives modulatory synaptic input.
     * 
     * @param[in] amplitude Modulatory input amplitude in nS * mV.
     * 
     * @note Modulatory input affects plasticity rather than immediate firing.
     *       Typically represents neuromodulator-mediated receptor activation
     *       (dopamine, acetylcholine, etc.).
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Injects direct current into the neuron (from external sources).
     * 
     * @param[in] current Current to inject in nA.
     * 
     * @note Used for sensory input simulation, optogenetic stimulation,
     *       or other external current sources. Different from synaptic input
     *       in that it bypasses synaptic dynamics.
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Retrieves the total injected current for this timestep.
     * 
     * @return MembranePotential Total current in nA.
     * 
     * @note This represents the sum of all injected currents from the current
     *       simulation timestep, before synaptic integration.
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clears the total current accumulator for next timestep.
     * 
     * @note This must be called at the end of each simulation timestep to
     *       ensure correct integration of new inputs in the next timestep.
     */
    void clearTotalCurrent();
    
    /**
     * @brief Records a spike in the neuron's history.
     * 
     * @param[in] timestamp Simulation timestamp when spike occurred.
     * 
     * @note Spike history is used for STDP (Spike-Timing-Dependent Plasticity)
     *       and for computing firing statistics. History is limited to prevent
     *       unbounded memory growth.
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Retrieves the neuron's spike history.
     * 
     * @return const std::vector<Timestamp>& Reference to spike history vector.
     * 
     * @note Contains timestamps of all recent spikes. Vector is typically
     *       limited to the last ~100 spikes to maintain efficiency.
     * 
     * @warning Modifying the returned vector directly will break internal
     *          invariants. Use clearSpikeHistory() for clearing.
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clears the neuron's spike history.
     * 
     * @note Typically called during neuron reset operation to prepare for
     *       new spiking activity. Also useful for analysis and debugging.
     */
    void clearSpikeHistory();
    
    /**
     * @brief Adds an incoming synaptic connection reference.
     * 
     * @param[in] handle Handle to the incoming synapse.
     * 
     * @note This maintains the neuron's synaptic graph for efficient lookup
     *       and management. Each incoming synapse is tracked for synaptic input
     *       integration.
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Adds an outgoing synaptic connection reference.
     * 
     * @param[in] handle Handle to the outgoing synapse.
     * 
     * @note This maintains the neuron's synaptic graph for efficient lookup
     *       and management. Each outgoing synapse is tracked for spike propagation.
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Retrieves all incoming synaptic connection handles.
     * 
     * @return const std::vector<SynapseHandle>& Reference to incoming synapses.
     * 
     * @note Used for synaptic input integration and connectivity analysis.
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Retrieves all outgoing synaptic connection handles.
     * 
     * @return const std::vector<SynapseHandle>& Reference to outgoing synapses.
     * 
     * @note Used for spike propagation and connectivity analysis.
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Accesses the neuron's plasticity flags.
     * 
     * @return const PlasticityFlags& Const reference to plasticity flags.
     * 
     * @note Plasticity flags determine which learning mechanisms are active
     *       for this neuron (Hebbian, STDP, reward-modulated, etc.).
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Accesses the neuron's plasticity flags (mutable).
     * 
     * @return PlasticityFlags& Mutable reference to plasticity flags.
     * 
     * @note Use set/get methods rather than direct modification when possible.
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Configures which plasticity mechanisms are active.
     * 
     * @param[in] hebbian Enable Hebbian plasticity.
     * @param[in] stdp Enable spike-timing-dependent plasticity.
     * @param[in] rewardModulated Enable reward-modulated plasticity.
     * 
     * @note Enabling plasticity increases computational complexity but
     *       enables adaptive learning. Disabling plasticity makes neurons
     *       static components of the network.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Sets the neuron's region identifier.
     * 
     * @param[in] region Region ID where this neuron resides.
     * 
     * @note Regions are logical groupings of neurons for organizational
     *       and computational purposes. This helps manage large networks.
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Retrieves the neuron's region identifier.
     * 
     * @return RegionId Region ID where this neuron resides.
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Sets the neuron's population identifier.
     * 
     * @param[in] population Population ID within the neuron's region.
     * 
     * @note Populations group neurons by type or function within a region.
     *       This provides hierarchical organization above the individual neuron level.
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Retrieves the neuron's population identifier.
     * 
     * @return PopulationId Population ID within the neuron's region.
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Performs one simulation step using default timestep.
     * 
     * @param[in] currentTime Current simulation timestamp.
     * 
     * @note This is a convenience method that uses a default timestep of
     *       1ms. For more precise control, use stepLIF() directly.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets the neuron to initial physiological state.
     * 
     * @note This resets membrane potential, firing rate, and clears spike
     *       history. Does not reset intrinsic parameters like threshold,
     *       refractory period, or neuron type.
     */
    void reset();
    
    /**
     * @brief Initializes neuron with random physiological parameters.
     * 
     * @param[in] rng Random number generator for stochastic initialization.
     * 
     * @note This sets initial membrane potential, threshold, refractory
     *       period, and other parameters based on biological distributions.
     *       Ensures each neuron has slightly different properties.
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Internal implementation structure for memory efficiency.
     * 
     * This pImpl pattern keeps the header file small while allowing the
     * implementation to evolve without breaking binary compatibility.
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation.
     * 
     * This pImpl pattern allows us to hide implementation details and
     * reduce compilation dependencies. Neuron operations are delegated
     * to this internal implementation.
     */
    Impl* pImpl;
};

} // namespace nlm

// Convenience type aliases for easier Python integration
namespace nlm {
    using NeuronPtr = std::unique_ptr<Neuron>;
    using ConstNeuronPtr = std::unique_ptr<const Neuron>;
    using NeuronWeakPtr = std::weak_ptr<Neuron>;
}