#pragma once

/**
 * @file Neuron.hpp
 * @brief Leaky Integrate-and-Fire (LIF) neuron implementation
 * @author NLM Project
 * @date Phase 3
 * 
 * This file defines the Neuron class, which represents a single spiking neuron
 * in the NLM (熙然) computational neuroscience platform. The neuron implements
 * the Leaky Integrate-and-Fire (LIF) model, a fundamental computational model
 * in neuroscience that describes how neurons integrate inputs and generate
 * action potentials (spikes) when their membrane potential crosses a threshold.
 * 
 * The LIF model is characterized by:
 * - Membrane potential that leaks towards resting potential
 * - Synaptic currents that modify the potential
 * - A firing threshold that, when reached, triggers a spike
 * - Refractory period after spiking during which the neuron cannot spike again
 * 
 * @note This implementation uses the PImpl idiom to avoid circular dependencies
 *       and improve compilation times.
 * 
 * @see https://en.wikipedia.org/wiki/Leaky_integrate-and-fire
 */

/**
 * @brief Forward declaration to avoid circular dependencies
 */
class Synapse;
class RandomGenerator;

/**
 * @brief Complete neuron state for efficient storage and access
 * 
 * This struct stores all state variables needed for LIF neuron dynamics.
 * The state is organized by functional groups for efficient memory access.
 * 
 * @note This struct is used by both the main neuron implementation and
 *       simulation code to maintain neuron state across time steps.
 */
struct NeuronState {
    // Membrane potential variables
    MembranePotential membranePotential;    ///< Current membrane potential (mV)
    MembranePotential restingPotential;    ///< Resting potential (mV)
    MembranePotential threshold;           ///< Firing threshold (mV)
    MembranePotential resetPotential;       ///< Post-spike reset value (mV)
    MembranePotential leakConductance;      ///< Leak conductance (nS)
    MembranePotential synapseConductance;   ///< Synaptic conductance (nS)
    
    // Firing dynamics
    FiringRate firingRate;                 ///< Current firing rate (Hz)
    FiringState firingState;               ///< Current firing state
    uint32_t refractoryRemaining;          ///< Steps remaining in refractory period
    uint32_t refractoryPeriod;             ///< Total refractory period (steps)
    
    // Adaptation and history
    float adaptationVariable;              ///< For spike-frequency adaptation
    float lastSpikeTime;                   ///< Timestamp of last spike (-1 if none)

    /**
     * @brief Default constructor initializes neuron to resting state
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
 * @brief Leaky Integrate-and-Fire (LIF) neuron implementation
 * 
 * The Neuron class represents a single spiking neuron in the NLM computational
 * neuroscience platform. It implements the LIF model, which is one of the most
 * widely used models in computational neuroscience due to its balance of
 * biological plausibility and computational tractability.
 * 
 * Key features:
 * - Membrane potential integration with leak dynamics
 * - Synaptic input processing
 * - Spike generation and refractory period
 * - State management and persistence
 * - Plasticity support via spike history tracking
 * 
 * The neuron follows the LIF differential equation:
 * dV/dt = (-g_L*(V - E_L) + I_syn) / C_m
 * 
 * Where:
 * - V is membrane potential
 * - g_L is leak conductance
 * - E_L is resting potential
 * - I_syn is synaptic current
 * - C_m is membrane capacitance
 * 
 * @note This class uses the PImpl idiom to minimize compilation dependencies.
 *       The implementation details are hidden in the private implementation struct.
 * 
 * @see Synapse - for synaptic connections that provide input to neurons
 * @see PlasticityRule - for synaptic plasticity mechanisms
 */
class Neuron {
public:
    /**
     * @brief Constructor creates a new neuron with a unique ID
     * 
     * @param id Unique identifier for this neuron
     * 
     * @throws std::bad_alloc if memory allocation fails
     * 
     * @note Neuron IDs are used for graph operations and connectivity tracing.
     *       IDs should be unique within a neural population.
     */
    explicit Neuron(NeuronId id);

    /**
     * @brief Destructor cleans up neuron resources
     */
    ~Neuron();

    /**
     * @brief Copy constructor is deleted to prevent shallow copying
     * 
     * @warning Neurons are not copyable due to internal PImpl structure.
     *          Use move operations instead.
     */
    Neuron(const Neuron&) = delete;

    /**
     * @brief Copy assignment operator is deleted to prevent shallow copying
     * 
     * @warning Neurons are not copyable due to internal PImpl structure.
     *          Use move operations instead.
     */
    Neuron& operator=(const Neuron&) = delete;

    /**
     * @brief Move constructor efficiently transfers ownership
     * 
     * @param other Neuron to move from
     * @note After moving, the source neuron is in a valid but unspecified state.
     */
    Neuron(Neuron&&) noexcept;

    /**
     * @brief Move assignment operator efficiently transfers ownership
     * 
     * @param other Neuron to move from
     * @return Reference to this neuron
     * @note After moving, the source neuron is in a valid but unspecified state.
     */
    Neuron& operator=(Neuron&&) noexcept;

    /**
     * @brief Get the neuron's unique identifier
     * 
     * @return Neuron ID
     * @note Neuron IDs are used for graph operations and connectivity tracing.
     */
    NeuronId getId() const;

    /**
     * @brief Get the neuron's type (excitatory/inhibitory/internal)
     * 
     * @return Neuron type
     * @note Neuron type influences synaptic connection rules and behavior.
     */
    NeuronType getType() const;

    /**
     * @brief Set the neuron's type (excitatory/inhibitory/internal)
     * 
     * @param type New neuron type
     * @note Changing neuron type may affect synaptic connection eligibility.
     */
    void setType(NeuronType type);

    /**
     * @brief Get read-only access to neuron state
     * 
     * @return Constant reference to neuron state
     * @note This provides access to all neuron state variables without
     *       allowing modifications to internal state.
     */
    const NeuronState& getState() const;

    /**
     * @brief Get mutable access to neuron state
     * 
     * @return Reference to neuron state
     * @warning Direct state modification should be avoided except for
     *          testing and specialized simulation code.
     */
    NeuronState& getState();

    /**
     * @brief Get current membrane potential
     * 
     * @return Membrane potential in mV
     * @note Typical values range from -70 mV (resting) to -40 mV (threshold).
     */
    MembranePotential getMembranePotential() const;

    /**
     * @brief Set membrane potential directly
     * 
     * @param potential New membrane potential in mV
     * @warning This is a low-level operation. Normal neuron dynamics should
     *          be achieved through synaptic inputs, not direct manipulation.
     */
    void setMembranePotential(MembranePotential potential);

    /**
     * @brief Add synaptic current to membrane potential
     * 
     * @param delta Change in membrane potential (mV)
     * @note Positive delta represents depolarizing input (excitation).
     */
    void addToMembranePotential(MembranePotential delta);

    /**
     * @brief Get firing threshold
     * 
     * @return Threshold voltage in mV
     * @note When membrane potential reaches or exceeds this value, a spike is generated.
     */
    MembranePotential getThreshold() const;

    /**
     * @brief Set firing threshold
     * 
     * @param threshold New threshold voltage in mV
     */
    void setThreshold(MembranePotential threshold);

    /**
     * @brief Check if neuron is currently firing
     * 
     * @return true if neuron is in firing state
     */
    bool isFiring() const;

    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return true if neuron cannot fire (refractory period active)
     */
    bool isRefractory() const;

    /**
     * @brief Set neuron's firing state
     * 
     * @param state New firing state
     * @warning This is a low-level operation. Normal state transitions
     *          should be achieved through threshold checking and spike generation.
     */
    void setFiringState(FiringState state);

    /**
     * @brief Set refractory period duration
     * 
     * @param steps Number of simulation steps the neuron remains refractory
     */
    void setRefractoryPeriod(uint32_t steps);

    /**
     * @brief Decrement refractory period counter
     * 
     * @note This should be called each simulation step to advance the refractory state.
     */
    void decrementRefractory();

    /**
     * @brief Get current firing rate (Hz)
     * 
     * @return Firing rate in Hertz
     * @note This is a rate-based measure of neuron activity.
     */
    FiringRate getFiringRate() const;

    /**
     * @brief Set firing rate directly
     * 
     * @param rate New firing rate in Hz
     * @warning This is a low-level operation used for testing and rate-based models.
     */
    void setFiringRate(FiringRate rate);

    /**
     * @brief Set leak conductance parameter
     * 
     * @param conductance Leak conductance in nS
     * @note Higher conductance means faster potential decay towards resting potential.
     */
    void setLeakConductance(MembranePotential conductance);

    /**
     * @brief Get leak conductance parameter
     * 
     * @return Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;

    /**
     * @brief Get refractory period duration
     * 
     * @return Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;

    /**
     * @brief Set resting potential parameter
     * 
     * @param potential New resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);

    /**
     * @brief Get resting potential parameter
     * 
     * @return Resting potential in mV
     */
    MembranePotential getRestingPotential() const;

    /**
     * @brief Set reset potential parameter
     * 
     * @param potential New reset potential in mV
     * @note This is the membrane potential after a spike.
     */
    void setResetPotential(MembranePotential potential);

    /**
     * @brief Get reset potential parameter
     * 
     * @return Reset potential in mV
     */
    MembranePotential getResetPotential() const;

    /**
     * @brief Check if membrane potential has reached firing threshold
     * 
     * @return true if neuron should fire
     */
    bool checkThreshold() const;

    /**
     * @brief Get timestamp of last spike
     * 
     * @return Last spike time in simulation steps (-1 if never spiked)
     */
    float getLastSpikeTime() const;

    /**
     * @brief Advance neuron by one simulation step
     * 
     * This method implements the LIF dynamics for one time step:
     * - Decay membrane potential towards resting potential
     * - Apply synaptic inputs
     * - Check for threshold crossing
     * - Handle spike generation and refractory period
     * 
     * @param dt Time step duration in milliseconds
     * @param rng Random number generator for stochastic processes
     * @note This is the core neuron simulation method.
     */
    void step(TimestepDuration dt, class RandomGenerator& rng);

    /**
     * @brief Reset neuron to initial state
     * 
     * @note This sets membrane potential to resting potential and resets
     *       firing state, clearing spike history.
     */
    void reset();

    /**
     * @brief Initialize neuron with random parameters
     * 
     * @param rng Random number generator
     * @note This populates neuron-specific parameters with random values
     *       for use in network initialization.
     */
    void initializeRandom(class RandomGenerator& rng);

protected:
    /**
     * @brief Protected constructor to enforce ID requirement
     * 
     * @param id Unique identifier for this neuron
     * @param initialize Automatically initialize neuron state
     */
    Neuron(NeuronId id, bool initialize);

private:
    /**
     * @brief Private implementation details (PImpl idiom)
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
