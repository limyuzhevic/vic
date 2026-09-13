#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <string>

namespace nlm {

/**
 * @brief Complete LIF neuron with adaptation, synaptic management, and statistics
 * 
 * This enhanced Neuron class implements a fully-featured Leaky Integrate-and-Fire
 * (LIF) neuron model suitable for large-scale neural simulations. The class
 * provides advanced functionality including:
 * 
 * - Biological parameter configuration (resting potential, threshold, etc.)
 * - Multiple input types (excitatory, inhibitory, modulatory)
 * - Synaptic management for connectivity
 * - Plasticity control (Hebbian, STDP, reward-modulated)
 * - Detailed spike history tracking for STDP computations
 * - Spike-frequency adaptation
 * - Statistics collection (firing rate, membrane potential variance)
 * - Advanced simulation methods with proper error handling
 * 
 * @note This implementation follows Phase 2+ requirements and includes
 *       comprehensive error handling, configuration options, and statistics.
 *       The neuron model is suitable for both simple demonstrations and
 *       advanced research simulations.
 * 
 * @see NeuronState - Complete neuron state structure
 * @see PlasticityFlags - Plasticity control flags
 * @see Synapse - Synaptic connection management
 * 
 * @version 2.0 (Phase 6 integration ready)
 * @author NLM Development Team
 */
class Neuron {
public:
    /**
     * @brief Constructs a neuron with the given ID and default parameters
     * 
     * @param id Unique identifier for this neuron (typically NeuronId(0), NeuronId(1), etc.)
     * @param type The initial neuron type (defaults to NeuronType::Internal)
     * 
     * @note Use Neuron::createConfigured() for more advanced configuration
     *       including custom membrane parameters and initial conditions.
     */
    Neuron(NeuronId id, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Default virtual destructor
     */
    virtual ~Neuron();
    
    /**
     * @brief Move constructor - transfers ownership efficiently
     * 
     * @param other Neuron to move from (will be left in invalid state)
     * @note This constructor is noexcept to guarantee no exceptions during movement.
     *       The source neuron will have its implementation pointer set to null.
     */
    Neuron(Neuron&& other) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership efficiently
     * 
     * @param other Neuron to move from (will be left in invalid state)
     * @return Reference to this neuron for chaining operations
     * @note This operator is noexcept to guarantee no exceptions during assignment.
     *       The source neuron will have its implementation pointer set to null.
     */
    Neuron& operator=(Neuron&& other) noexcept;
    
    /**
     * @brief Creates a new neuron with complete configuration
     * 
     * This factory method provides the most convenient way to create neurons with
     * biologically plausible parameters. It's recommended for most use cases.
     * 
     * @param id Unique identifier for the neuron
     * @param type Neuron type (defaults to NeuronType::Internal)
     * @param restingPotential Membrane resting potential in mV (typical: -70mV)
     * @param threshold Firing threshold in mV (typical: -55mV)
     * @param refractoryPeriod Refractory period in simulation steps (typical: 5)
     * @param leakConductance Leak conductance in nS (typical: 10nS)
     * @return std::unique_ptr<Neuron> Configured neuron with default parameters
     * 
     * @note This method allocates memory on the heap. Use initializeRandom() for
     *       stochastic initialization of existing neurons.
     */
    static std::unique_ptr<Neuron> createConfigured(
        NeuronId id,
        NeuronType type = NeuronType::Internal,
        MembranePotential restingPotential = -70.0f,
        MembranePotential threshold = -55.0f,
        uint32_t refractoryPeriod = 5,
        MembranePotential leakConductance = 10.0f
    );
    
    /**
     * @brief Creates a neuron with advanced biological parameters
     * 
     * This factory method allows for more detailed biological modeling including
     * reset potential, adaptation variable, and initial conditions.
     * 
     * @param id Unique identifier for the neuron
     * @param config Configuration structure with advanced parameters
     * @return std::unique_ptr<Neuron> Neuron configured with biological parameters
     * 
     * @note For research applications requiring detailed biological accuracy.
     */
    static std::unique_ptr<Neuron> createBiologicallyAccurate(
        NeuronId id,
        const struct BiologicalNeuronConfig& config
    );
    
    /**
     * @brief Gets the unique ID of this neuron
     * 
     * @return The neuron ID (immutable identifier for this neuron)
     */
    NeuronId getId() const;
    
    /**
     * @brief Gets the neuron's type (excitatory, inhibitory, etc.)
     * 
     * @return The neuron type from the NeuronType enumeration
     */
    NeuronType getType() const;
    
    /**
     * @brief Sets the neuron type
     * 
     * @param type The neuron type from the NeuronType enumeration
     */
    void setType(NeuronType type);
    
    /**
     * @brief Gets the current neuron state
     * 
     * @return Const reference to the NeuronState structure containing current values
     * 
     * @note Use getState() for mutable access, getState() const for immutable access.
     *       The state includes all dynamic variables: membrane potential, firing rate,
     *       refractory counters, etc.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Gets the current neuron state (mutable)
     * 
     * @return Reference to the NeuronState structure for modification
     * 
     * @warning Modifying state directly can lead to inconsistent neuron behavior.
     *          Use setter methods or stepLIF() for proper state management.
     */
    NeuronState& getState();
    
    /**
     * @brief Gets the neuron's current membrane potential
     * 
     * @return The membrane potential in millivolts (mV)
     * @note This value changes dynamically during simulation steps.
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Sets the neuron's membrane potential directly
     * 
     * @param potential The membrane potential in mV
     * @warning This method should only be used for initialization or testing.
     *          During normal simulation, use stepLIF() to update potential naturally.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Adds a delta to the membrane potential (synaptic input)
     * 
     * This is the primary method for adding synaptic inputs to the neuron.
     * Positive values depolarize (excitatory), negative values hyperpolarize (inhibitory).
     * 
     * @param delta The change in membrane potential (mV). Can be positive or negative.
     * 
     * @see receiveExcitatoryInput() - Specialized excitatory input
     * @see receiveInhibitoryInput() - Specialized inhibitory input
     * @see injectCurrent() - For external current injection
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Gets the neuron's firing threshold
     * 
     * @return The threshold potential in mV (typically -55mV)
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Sets the neuron's firing threshold
     * 
     * @param threshold The threshold potential in mV (typical range: -60 to -50mV)
     * 
     * @note Changing the threshold affects neuronal excitability. Use with caution
     *       during ongoing simulations.
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Checks if the neuron is currently firing
     * 
     * @return true if the neuron is in Active or Refractory state, false otherwise
     */
    bool isFiring() const;
    
    /**
     * @brief Checks if the neuron is in refractory period
     * 
     * @return true if the neuron is in Refractory state, false otherwise
     */
    bool isRefractory() const;
    
    /**
     * @brief Gets the neuron's current firing state
     * 
     * @return The current firing state (Resting, Active, Refractory, Inhibited)
     */
    FiringState getFiringState() const;
    
    /**
     * @brief Sets the firing state directly
     * 
     * @param state The firing state to set
     * @warning This should only be used for initialization or testing. During
     *          normal simulation, use stepLIF() to update state naturally.
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Gets the neuron's current refractory period
     * 
     * @return The refractory period in simulation steps (typically 2-10)
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Sets the neuron's refractory period
     * 
     * @param steps The number of simulation timesteps the neuron will remain
     *              refractory after firing (typical range: 1-100)
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Gets the remaining refractory period
     * 
     * @return Steps remaining in the refractory period (0 if not refractory)
     */
    uint32_t getRefractoryRemaining() const;
    
    /**
     * @brief Decrements the refractory counter (called by simulation framework)
     * 
     * This method should be called once per simulation timestep while the neuron
     * is in the refractory period. When the counter reaches zero, the neuron
     * becomes eligible to fire again.
     */
    void decrementRefractory();
    
    /**
     * @brief Gets the neuron's current firing rate
     * 
     * @return The firing rate in Hz (spikes per second)
     * @note This is calculated from spike history over time.
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Sets the firing rate (for testing or rate-based models)
     * 
     * @param rate The firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Gets the neuron's leak conductance
     * 
     * @return The leak conductance value (nS)
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Sets the neuron's leak conductance
     * 
     * @param conductance The leak conductance value (nS)
     * @note Higher leak conductance means faster decay to resting potential.
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Gets the neuron's resting potential
     * 
     * @return The resting potential in mV (typically -70mV)
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Sets the neuron's resting potential
     * 
     * @param potential The resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Gets the neuron's reset potential (post-spike)
     * 
     * @return The reset potential in mV (typically -75mV)
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Sets the neuron's reset potential
     * 
     * @param potential The reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Checks if the neuron has reached firing threshold
     * 
     * @return true if the neuron's membrane potential >= threshold, false otherwise
     * @note This is checked within stepLIF(), but can be used for inspection.
     */
    bool checkThreshold() const;
    
    /**
     * @brief Gets the time of the last spike
     * 
     * @return The timestamp of the last spike, or -1.0 if no spike has occurred
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Gets the neuron's adaptation variable (for spike-frequency adaptation)
     * 
     * @return The current adaptation variable value (typically 0-10)
     * @note Higher values cause more hyperpolarization after spikes.
     */
    float getAdaptationVariable() const;
    
    /**
     * @brief Sets the adaptation variable
     * 
     * @param var The adaptation variable value
     */
    void setAdaptationVariable(float var);
    
    /**
     * @brief Gets the neuron's spike count (recent spikes)
     * 
     * @return Number of spikes recorded in the spike history (last 100 spikes)
     */
    size_t getSpikeCount() const;
    
    /**
     * @brief Gets the neuron's membrane potential variance
     * 
     * @return The variance in mV² (calculated from recent membrane potentials)
     */
    float getMembranePotentialVariance() const;
    
    /**
     * @brief Performs one timestep of LIF dynamics
     * 
     * This is the core simulation step that implements the Leaky Integrate-and-Fire
     * equation with proper handling of:
     * - Leaky integration based on resting potential
     * - Synaptic input accumulation
     * - Spike detection and reset
     * - Refractory period management
     * - Spike-frequency adaptation
     * 
     * @param currentTime The current simulation timestamp
     * @param dt The timestep duration (typically 0.001 for 1ms simulation)
     * @return true if the neuron fired during this timestep, false otherwise
     * 
     * @note This method should be called once per timestep for each neuron.
     *       The LIF equation is: dV/dt = -(V - V_rest)/tau + I/C
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Performs one timestep of neuron simulation (convenience method)
     * 
     * This method uses a default timestep of 1ms and is suitable for most
     * simple simulations. For more precise timing, use stepLIF() directly.
     * 
     * @param currentTime The current simulation timestamp
     * @see stepLIF() - For precise timestep control
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets the neuron to its initial state
     * 
     * Clears all internal state including membrane potential, spike history,
     * and refractory counters. Does not change the neuron's structural properties
     * (type, region, population ID). Preserves biologically plausible parameters
     * like resting potential and threshold.
     */
    void reset();
    
    /**
     * @brief Initializes neuron parameters with random biological values
     * 
     * This method sets up the neuron's intrinsic properties with biologically
     * plausible random values based on the provided random number generator.
     * The initialization follows known biological ranges:
     * - Membrane potential: V_rest ± 3mV
     * - Threshold: -55mV ± 2mV
     * - Refractory period: 2-10 timesteps
     * - Adaptation: 0 initially
     * 
     * @param rng Random number generator for stochastic initialization
     * 
     * @see RandomGenerator - For proper random number generation
     */
    void initializeRandom(RandomGenerator& rng);
    
    /**
     * @brief Adds excitatory synaptic input to the neuron
     * 
     * This represents the net effect of excitatory synapses that depolarize
     * (raise) the membrane potential. Excitatory inputs typically bring the
     * neuron closer to threshold.
     * 
     * @param amplitude The strength of the excitatory input in mV
     * 
     * @see receiveInhibitoryInput() - For inhibitory inputs
     * @see receiveModulatoryInput() - For neuromodulation
     * @see addToMembranePotential() - Generic synaptic input
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Adds inhibitory synaptic input to the neuron
     * 
     * This represents the net effect of inhibitory synapses that hyperpolarize
     * (lower) the membrane potential. Inhibitory inputs push the neuron away
     * from threshold.
     * 
     * @param amplitude The strength of the inhibitory input in mV
     * @note The implementation subtracts from membrane potential internally.
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Adds modulatory synaptic input to the neuron
     * 
     * Modulatory inputs (e.g., from neuromodulators like dopamine) typically
     * affect plasticity or excitability rather than directly changing the
     * membrane potential. This simplified model stores the modulation for
     * plasticity calculations.
     * 
     * @param amplitude The strength of the modulatory input in mV
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Injects direct current into the neuron (e.g., from sensory input)
     * 
     * This represents external input that directly affects the neuron's
     * membrane potential, such as from sensory receptors or external sources.
     * This is different from synaptic input as it bypasses synapse-specific
     * dynamics.
     * 
     * @param current The current to inject in appropriate units (mV/ms or similar)
     * 
     * @see receiveExcitatoryInput() - For synaptic excitatory inputs
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Gets the total accumulated current (for debugging/statistics)
     * 
     * @return The total current value accumulated since last clear
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clears accumulated synaptic input
     * 
     * This should be called at the end of the synaptic integration phase to
     * prevent input from carrying over to subsequent timesteps. Note that
     * direct current injection via injectCurrent() is handled separately.
     */
    void clearTotalCurrent();
    
    /**
     * @brief Records a spike at the given timestamp
     * 
     * Updates the neuron's spike history for STDP computations and statistics.
     * This is called automatically when the neuron fires in stepLIF().
     * 
     * @param timestamp The simulation timestamp when the spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Gets the neuron's spike history (recent spikes)
     * 
     * Returns the timestamps of all spikes recorded in the history buffer.
     * The history is limited to the most recent spikes (typically 100).
     * 
     * @return Const reference to the vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clears the neuron's spike history
     * 
     * Removes all recorded spike times. Useful for resetting the neuron
     * to a clean state or preparing for a new simulation episode.
     */
    void clearSpikeHistory();
    
    /**
     * @brief Adds an incoming synapse to this neuron
     * 
     * This is used when building connectivity graphs. The neuron maintains
     * lists of incoming and outgoing synapses for efficient connectivity queries.
     * 
     * @param handle Unique identifier for the incoming synapse
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Adds an outgoing synapse from this neuron
     * 
     * This is used when building connectivity graphs. The neuron maintains
     * lists of incoming and outgoing synapses for efficient connectivity queries.
     * 
     * @param handle Unique identifier for the outgoing synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Gets the list of incoming synapses
     * 
     * @return Const reference to the vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Gets the list of outgoing synapses
     * 
     * @return Const reference to the vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Gets the plasticity flags for this neuron (const)
     * 
     * Controls which plasticity mechanisms (Hebbian, STDP, reward-modulated)
     * should be applied to this neuron. Use getPlasticityFlags() for mutable access.
     * 
     * @return Const reference to the PlasticityFlags structure
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Gets the plasticity flags for this neuron (mutable)
     * 
     * Controls which plasticity mechanisms (Hebbian, STDP, reward-modulated)
     * should be applied to this neuron.
     * 
     * @return Reference to the PlasticityFlags structure
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enables or disables plasticity mechanisms
     * 
     * This is the primary method for controlling plasticity in this neuron. By
     * default, all plasticity mechanisms are enabled for biologically realistic
     * simulations.
     * 
     * @param hebbian Enable Hebbian (rate-based) plasticity
     * @param stdp Enable Spike-Timing-Dependent Plasticity (STDP)
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Checks if Hebbian plasticity is enabled
     * 
     * @return true if Hebbian plasticity is enabled
     */
    bool isHebbianEnabled() const;
    
    /**
     * @brief Checks if STDP plasticity is enabled
     * 
     * @return true if STDP plasticity is enabled
     */
    bool isSTDPEnabled() const;
    
    /**
     * @brief Checks if reward-modulated plasticity is enabled
     * 
     * @return true if reward-modulated plasticity is enabled
     */
    bool isRewardModulatedEnabled() const;
    
    /**
     * @brief Sets the region ID this neuron belongs to
     * 
     * @param region The region ID
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Gets the region ID this neuron belongs to
     * 
     * @return The region ID
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Sets the population ID this neuron belongs to
     * 
     * @param population The population ID
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Gets the population ID this neuron belongs to
     * 
     * @return The population ID
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Calculates and returns the neuron's adaptation index
     * 
     * This is a convenience method that combines multiple adaptation metrics
     * into a single score. Useful for research applications analyzing neural
     * adaptation behavior.
     * 
     * @return An adaptation index between 0 (no adaptation) and 1 (strong adaptation)
     */
    float calculateAdaptationIndex() const;
    
    /**
     * @brief Determines if the neuron is likely to fire in the near future
     * 
     * This predictive method analyzes the neuron's current state to estimate
     * the probability of firing within the next timestep. Useful for advanced
     * simulations and control applications.
     * 
     * @param dt Timestep duration for prediction
     * @return true if the neuron is likely to fire soon, false otherwise
     */
    bool predictImminentFiring(TimestepDuration dt) const;
    
    /**
     * @brief Simulates the neuron with external input
     * 
     * This advanced method combines the core LIF dynamics with external input
     * handling in a single call. It's more convenient than calling separate
     * methods for input injection and stepping.
     * 
     * @param currentTime The current simulation timestamp
     * @param externalInput The external input to apply (e.g., sensory input)
     * @param dt The timestep duration
     * @return true if the neuron fired during this timestep
     * 
     * @note This is particularly useful for agent-based simulations where
     *       neurons need to respond to environmental inputs.
     */
    bool stepWithInput(Timestamp currentTime, MembranePotential externalInput, TimestepDuration dt);
    
    /**
     * @brief Compares two neurons for equality
     * 
     * @param other The neuron to compare with
     * @return true if both neurons have the same ID, false otherwise
     */
    bool operator==(const Neuron& other) const;
    
    /**
     * @brief Compares two neurons for inequality
     * 
     * @param other The neuron to compare with
     * @return true if neurons have different IDs, false otherwise
     */
    bool operator!=(const Neuron& other) const;
    
    /**
     * @brief Stream insertion operator for debugging
     * 
     * @param os Output stream
     * @param neuron The neuron to serialize
     * @return Reference to the output stream
     * 
     * @note This operator formats the neuron state in a readable way for
     *       debugging and logging purposes.
     */
    friend std::ostream& operator<<(std::ostream& os, const Neuron& neuron);

private:
    /**
     * @brief Pimpl implementation pointer (pointer to implementation)
     * 
     * This implements the Pimpl idiom to hide implementation details and reduce
     * compilation dependencies. The actual implementation is in the private
     * Neuron::Impl struct within Neuron.cpp.
     */
    struct Impl;
    Impl* pImpl;

}; // class Neuron

// ========== BIOLOGICAL NEURON CONFIGURATION ==========

/**
 * @brief Configuration structure for biologically accurate neurons
 * 
 * This structure allows for detailed specification of neuron parameters
 * based on biological principles. It can be used with createBiologicallyAccurate().
 */
struct BiologicalNeuronConfig {
    NeuronId id;
    NeuronType type;
    
    // Membrane properties
    MembranePotential restingPotential;     // mV
    MembranePotential threshold;            // mV
    MembranePotential resetPotential;       // mV
    MembranePotential leakConductance;      // nS
    
    // Dynamics
    uint32_t refractoryPeriod;              // timesteps
    float adaptationTimeConstant;            // for adaptation modeling
    
    // State
    FiringRate initialFiringRate;           // Hz
    FiringState initialState;               // Starting state
    float initialAdaptation;                // Initial adaptation variable
    float lastSpikeTime;                    // For initialization
    
    BiologicalNeuronConfig()
        : id(), type(NeuronType::Internal)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-75.0f)
        , leakConductance(10.0f)
        , refractoryPeriod(5)
        , adaptationTimeConstant(100.0f)
        , initialFiringRate(0.0f)
        , initialState(FiringState::Resting)
        , initialAdaptation(0.0f)
        , lastSpikeTime(-1.0f) {}
};

// ========== UTILITY FUNCTIONS ==========

/**
 * @brief Creates a standardized neuron for research simulations
 * 
 * This factory method creates a neuron with parameters optimized for
 * typical research applications in computational neuroscience.
 * 
 * @param id Neuron ID
 * @param neuronType Neuron type (defaults to internal)
 * @return std::unique_ptr<Neuron> Research-ready neuron
 * 
 * @note Uses biologically plausible default parameters and enables all
 *       plasticity mechanisms for comprehensive research capabilities.
 */
inline std::unique_ptr<Neuron> createResearchNeuron(
    NeuronId id,
    NeuronType neuronType = NeuronType::Internal
) {
    return Neuron::createConfigured(id, neuronType, -70.0f, -55.0f, 5, 10.0f);
}

/**
 * @brief Creates a fast-spiking neuron (typical of cortical pyramidal neurons)
 * 
 * This factory method creates a neuron optimized for high-frequency firing
 * with lower threshold and shorter refractory period.
 * 
 * @param id Neuron ID
 * @return std::unique_ptr<Neuron> Fast-spiking neuron
 */
inline std::unique_ptr<Neuron> createFastSpikingNeuron(NeuronId id) {
    return Neuron::createConfigured(id, NeuronType::Excitatory, -65.0f, -50.0f, 2, 15.0f);
}

/**
 * @brief Creates an adapting neuron for rate coding studies
 * 
 * This factory method creates a neuron with strong adaptation properties,
 * suitable for studying spike-frequency adaptation phenomena.
 * 
 * @param id Neuron ID
 * @return std::unique_ptr<Neuron> Adapting neuron
 */
inline std::unique_ptr<Neuron> createAdaptingNeuron(NeuronId id) {
    auto neuron = Neuron::createConfigured(id, NeuronType::Internal, -70.0f, -55.0f, 8, 12.0f);
    neuron->setAdaptationVariable(2.0f);
    return neuron;
}

/**
 * @brief Calculates firing rate from spike history
 * 
 * @param spikeHistory Vector of spike timestamps
 * @param currentTime Current simulation time
 * @param window Window size for calculation (defaults to recent spikes)
 * @return FiringRate in Hz, or 0 if insufficient data
 */
inline FiringRate calculateFiringRate(
    const std::vector<Timestamp>& spikeHistory,
    Timestamp currentTime,
    float window = 100.0f
) {
    if (spikeHistory.empty()) return 0.0f;
    
    size_t recentSpikes = 0;
    for (Timestamp t : spikeHistory) {
        if (currentTime - t < window) {
            recentSpikes++;
        }
    }
    
    return static_cast<FiringRate>(recentSpikes) / window * 1000.0f;
}

/**
 * @brief Compares two neurons based on their firing characteristics
 * 
 * @param lhs Left-hand side neuron
 * @param rhs Right-hand side neuron
 * @param tolerance Tolerance for floating-point comparison
 * @return true if neurons have similar firing properties
 */
inline bool neuronsSimilarForSimulation(
    const Neuron& lhs,
    const Neuron& rhs,
    float tolerance = 0.01f
) {
    const auto& state1 = lhs.getState();
    const auto& state2 = rhs.getState();
    
    return std::abs(state1.membranePotential - state2.membranePotential) < tolerance &&
           std::abs(state1.threshold - state2.threshold) < tolerance &&
           lhs.getType() == rhs.getType();
}

} // namespace nlm

/**
 * @brief Outputs neuron state to stream in readable format
 * 
 * @param os Output stream
 * @param neuron Neuron to output
 * @return Reference to the output stream
 */
inline std::ostream& operator<<(std::ostream& os, const nlm::Neuron& neuron) {
    const auto& state = neuron.getState();
    os << "Neuron[id=" << neuron.getId().value 
       << ", type=" << static_cast<int>(neuron.getType())
       << ", V=" << state.membranePotential << "mV"
       << ", threshold=" << state.threshold << "mV"
       << ", firing=" << (neuron.isFiring() ? "yes" : "no")
       << ", spikes=" << neuron.getSpikeCount() << "]";
    return os;
}

/**
 * @brief Equality operator for Neurons
 * 
 * @param lhs Left-hand side neuron
 * @param rhs Right-hand side neuron
 * @return true if both neurons have the same ID
 */
inline bool operator==(const nlm::Neuron& lhs, const nlm::Neuron& rhs) {
    return lhs.getId() == rhs.getId();
}

/**
 * @brief Inequality operator for Neurons
 * 
 * @param lhs Left-hand side neuron
 * @param rhs Right-hand side neuron
 * @return true if neurons have different IDs
 */
inline bool operator!=(const nlm::Neuron& lhs, const nlm::Neuron& rhs) {
    return !(lhs == rhs);
}
