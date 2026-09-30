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
/**
 * @class Neuron
 * @brief Core neural computation unit implementing Leaky Integrate-and-Fire (LIF) dynamics.
 * 
 * The Neuron class represents a single biological neuron implementing the Leaky
 * Integrate-and-Fire (LIF) model, which is a fundamental model in computational
 * neuroscience for simulating neuronal electrical activity. The neuron integrates
 * incoming synaptic inputs and generates action potentials (spikes) when its
 * membrane potential exceeds a threshold.
 * 
 * Neurons form the fundamental building blocks of the neural network, receiving
 * synaptic inputs, processing them according to LIF dynamics, and transmitting
 * output spikes through synaptic connections to other neurons.
 * 
 * @note The Neuron class uses a private implementation (pImpl pattern) for efficient
 *       memory management and to maintain flexibility in implementation details.
 *       The actual LIF dynamics are implemented in the private Impl class.
 * 
 * @see Brain for neural system coordination
 * @see Synapse for synaptic connections
 * @see NeuralRegion for neuron population organization
 * @see InterRegionConnection for long-range connectivity
 */

class Neuron {
public:
    // Create neuron with ID
    /**
     * @brief Constructor with neuron ID
     * 
     * Creates a new neuron with the specified ID and initializes its state
     * to default values. The neuron ID uniquely identifies the neuron within
     * the neural system and is used for addressing and organization.
     * 
     * @param id Unique identifier for this neuron. The ID must be unique within
     *           the neural system and is used for neuron addressing and debugging.
     * 
     * @note Neuron IDs are typically allocated by neural region managers or the
     *       brain system. Each neuron should have a unique ID for proper tracking
     *       and management.
     * 
     * @see getId for retrieving neuron ID
     * @see NeuralRegion::addNeuron for region-based neuron creation
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Default destructor
     * 
     * Cleans up neuron resources and state. The destructor ensures proper cleanup
     * of neuron-specific resources and state management.
     * 
     * @note Cleanup includes releasing any allocated resources and ensuring the
     *       neuron is properly removed from synaptic connections and regional
     *       management systems.
     */
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    /**
     * @brief Get neuron ID
     * 
     * Returns the unique identifier of this neuron. The neuron ID is essential
     * for neuron addressing, debugging, and organization within the neural system.
     * 
     * @return NeuronId The unique identifier of this neuron
     * 
     * @note Neuron IDs are assigned during construction and remain constant
     *       throughout the neuron's lifetime. They are used for synaptic connections,
     *       regional management, and neural system tracking.
     * 
     * @see Neuron(NeuronId) constructor
     */
    NeuronId getId() const;
    
    // Type
    /**
     * @brief Get neuron type
     * 
     * Returns the functional type of this neuron, which determines its role and
     * characteristics within the neural network. Neuron types include sensory,
     * motor, and interneuron types.
     * 
     * @return NeuronType The functional type of this neuron
     * 
     * @note Neuron type affects neuron behavior, connectivity rules, and its role
     *       in the neural network. Sensory neurons receive external input, motor
     *       neurons produce output actions, and interneurons process and integrate
     *       signals between neurons.
     * 
     * @see setType for setting neuron type
     * @see NeuronType for available neuron types
     */
    NeuronType getType() const;
    
    /**
     * @brief Set neuron type
     * 
     * Sets the functional type of this neuron. Changing neuron type can affect
     * its behavior, connectivity rules, and role in the neural network.
     * 
     * @param type The new functional type for this neuron
     * 
     * @note Neuron type changes may affect synaptic connectivity rules and neural
     *       processing behavior. Use with caution as it can significantly alter
     *       neuron function.
     * 
     * @warning Changing neuron type after synaptic connections have been established
     *         may lead to unexpected behavior or connectivity issues.
     * 
     * @see getType for retrieving neuron type
     * @see NeuronType for available neuron types
     */
    void setType(NeuronType type);
    
    // State access
    /**
     * @brief Get neuron state (const version)
     * 
     * Returns a constant reference to the neuron's current state structure. The
     * state contains all relevant neuron parameters including membrane potential,
     * firing state, and synaptic information.
     * 
     * @return const NeuronState& Constant reference to neuron state structure.
     *         The state is read-only and should not be modified directly.
     * 
     * @note The neuron state is updated automatically during simulation steps.
     *       This method provides read-only access to the current state for inspection
     *       and debugging purposes.
     * 
     * @see NeuronState for state structure details
     * @see getState() for mutable state access
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get neuron state (mutable version)
     * 
     * Returns a mutable reference to the neuron's state structure. This method
     * provides direct access to neuron state for advanced operations and
     * simulation control.
     * 
     * @return NeuronState& Mutable reference to neuron state structure
     * 
     * @note This method should be used with caution as direct state modification
     *       can lead to incorrect neural dynamics and simulation instability.
     *       Use for debugging, testing, or specialized simulation control only.
     * 
     * @see getState() const for read-only access
     * @see NeuronState for state structure details
     */
    NeuronState& getState();
    
    // Membrane potential
    /**
     * @brief Get membrane potential
     * 
     * Returns the current membrane potential of the neuron, which represents the
     * electrical charge difference across the neuron's membrane. This potential
     * integrates incoming synaptic inputs and determines firing likelihood.
     * 
     * @return MembranePotential Current membrane potential in millivolts (mV)
     * 
     * @note Membrane potential typically ranges from -70 mV (resting) to +30 mV
     *       (threshold). Values above threshold trigger action potentials (spikes).
     * 
     * @see setMembranePotential for setting membrane potential
     * @see addToMembranePotential for adjusting membrane potential
     * @see checkThreshold for threshold checking
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential
     * 
     * Sets the membrane potential of the neuron to an absolute value. This method
     * provides direct control over neuron membrane potential, bypassing normal
     * synaptic integration.
     * 
     * @param potential New membrane potential in millivolts (mV)
     * 
     * @note Setting membrane potential directly can cause immediate firing if the
     *               value exceeds the threshold. Use with caution for precise control
     *               of neural activity.
     * 
     * @warning Direct membrane potential setting can bypass normal synaptic
     *         processing and may lead to unnatural neural dynamics if used inappropriately.
     * 
     * @see getMembranePotential for retrieving current potential
     * @see addToMembranePotential for adjusting potential
     * @see checkThreshold for threshold checking
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to membrane potential
     * 
     * Adds a delta value to the current membrane potential, simulating the effect
     * of incoming synaptic inputs. This is the normal way neurons integrate synaptic
     * inputs during simulation.
     * 
     * @param delta Change in membrane potential in millivolts (mV). Positive values
     *               increase potential, negative values decrease it.
     * 
     * @note This method represents synaptic input integration, which is the fundamental
     *               process by which neurons communicate and process information.
     * 
     * @see getMembranePotential for retrieving current potential
     * @see setMembranePotential for absolute setting
     * @see receiveExcitatoryInput for excitatory synaptic input
     * @see receiveInhibitoryInput for inhibitory synaptic input
     */
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    /**
     * @brief Get firing threshold
     * 
     * Returns the current firing threshold of the neuron, which is the membrane
     * potential level at which the neuron generates an action potential (spike).
     * This threshold can be modified to control neuron excitability.
     * 
     * @return MembranePotential Current firing threshold in millivolts (mV)
     * 
     * @note The firing threshold typically ranges from -55 mV to -40 mV in
     *               biological neurons. Lower thresholds make neurons more excitable.
     * 
     * @see setThreshold for setting threshold
     * @see getMembranePotential for current membrane potential
     * @see checkThreshold for threshold checking
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set firing threshold
     * 
     * Sets the firing threshold of the neuron. The threshold determines the
     * membrane potential level required to trigger an action potential (spike).
     
     * @param threshold New firing threshold in millivolts (mV)
     * 
     * @note Lowering the threshold increases neuron excitability, while raising
     *               it decreases excitability. Threshold modulation is a form of
     *               gain control in neural processing.
     * 
     * @warning Modifying threshold can significantly affect network dynamics.
     *         Carefully test threshold changes in the context of the full neural
     *         network.
     * 
     * @see getThreshold for retrieving current threshold
     * @see getMembranePotential for current membrane potential
     * @see checkThreshold for threshold checking
     */
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    /**
     * @brief Check if neuron is firing
     * 
     * Returns whether the neuron is currently in the firing state (generating
     * action potentials). This is typically true after a spike has been generated
     * and before the refractory period ends.
     * 
     * @return bool true if neuron is currently firing, false otherwise
     * 
     * @retval true Neuron is in firing state
     * @retval false Neuron is not firing
     * 
     * @note Firing state is managed by the neuron state machine and typically
     *               lasts for a short duration before transitioning to refractory state.
     * 
     * @see isRefractory for refractory state checking
     * @see setFiringState for state modification
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory state
     * 
     * Returns whether the neuron is currently in the refractory period, during
     * which the neuron cannot generate additional action potentials regardless of
     * input.
     * 
     * @return bool true if neuron is in refractory state, false otherwise
     * 
     * @note Refractory period prevents excessive firing and models biological
     *               refractory periods where neurons need time to recover after firing.
     *               During this time, neurons cannot generate additional spikes.
     * 
     * @see decrementRefractory for refractory period management
     * @see setRefractoryPeriod for setting refractory duration
     * @see isFiring for firing state checking
     */
    bool isRefractory() const;
    
    /**
     * @brief Set firing state
     * 
     * Sets the firing state of the neuron directly. This method provides low-level
     * control over neuron state management.
     * 
     * @param state New firing state for the neuron
     * 
     * @note Setting firing state directly can bypass normal LIF dynamics. Use with
     *               caution for debugging, testing, or specialized simulation control.
     * 
     * @see isFiring for firing state checking
     * @see isRefractory for refractory state checking
     * @see setRefractoryPeriod for setting refractory duration
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period duration
     * 
     * Sets the total refractory period duration for the neuron. The refractory
     * period is the time during which the neuron cannot generate action potentials
     * after firing.
     * 
     * @param steps Total refractory period duration in simulation steps
     * 
     * @note Refractory period duration affects firing rate and neural dynamics.
     *               Longer refractory periods reduce maximum firing rates.
     * 
     * @see getRefractoryPeriod for retrieving refractory duration
     * @see isRefractory for refractory state checking
     * @see decrementRefractory for refractory period management
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory period
     * 
     * Decrements the remaining refractory period by one step. This method is
     * typically called automatically during simulation steps to manage the
     * refractory period countdown.
     * 
     * @note This method should only be called when the neuron is in refractory state.
     *               Calling it when not in refractory state has no effect.
     * 
     * @see isRefractory for refractory state checking
     * @see setRefractoryPeriod for setting refractory duration
     * @see stepLIF for full refractory period management
     */
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
