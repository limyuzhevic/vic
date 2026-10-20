/*!
 * @file Neuron.hpp
 * @brief Neuron class representing a single neuron in the NLM brain
 * @details Implements Leaky Integrate-and-Fire (LIF) dynamics with spike generation,
 *          refractory periods, and various plasticity mechanisms. The neuron serves
 *          as the fundamental computational unit of the NLM neural network.
 * @author NLM Project
 * @date Phase 2-6 Development
 * @version 6.0.0
 * @copyright MIT License
 */

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

/**
 * @struct NeuronState
 * @brief Contains the complete state information for a neuron
 * @details This structure stores all neuron state variables needed for LIF dynamics
 *          and plasticity computations. Designed for cache efficiency with Structure
 *          of Arrays (SoA) principles for SIMD vectorization.
 */
struct NeuronState {
    MembranePotential membranePotential;    ///< Current membrane potential (mV)
    MembranePotential restingPotential;    ///< Resting potential (mV)
    MembranePotential threshold;           ///< Firing threshold (mV)
    MembranePotential resetPotential;       ///< Post-spike reset value (mV)
    MembranePotential leakConductance;      ///< Leak conductance (nS)
    MembranePotential synapseConductance;   ///< Synaptic conductance (nS)
    FiringRate firingRate;                 ///< Current firing rate (Hz)
    FiringState firingState;               ///< Current firing state (Resting, Firing, Refractory)
    uint32_t refractoryRemaining;          ///< Steps remaining in refractory period
    uint32_t refractoryPeriod;             ///< Total refractory period (steps)
    float adaptationVariable;              ///< For spike-frequency adaptation
    float lastSpikeTime;                   ///< Timestamp of last spike (-1 if none)
    
    /**
     * @brief Default constructor initializes neuron to resting state
     * @details Sets all neuron parameters to typical biological values for
     *          stable initial conditions.
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
 * @brief Core computational unit of the NLM brain
 * @details Implements Leaky Integrate-and-Fire (LIF) dynamics with support for:
 *          - Event-driven spike generation
 *          - Refractory period management
 *          - Synaptic integration
 *          - Spike-frequency adaptation
 *          - Type classification (Excitatory, Inhibitory, Modulatory)
 *          - Region and population membership
 * 
 * The Neuron class uses a pImpl pattern for encapsulation and thread-safe operations.
 * Memory is managed through a memory pool for performance optimization.
 * 
 * @tparam MemoryPool Policy for memory allocation (single-threaded vs multi-threaded)
 * 
 * @see Brain
 * @see Synapse
 * @see VectorizedLIF
 * @see https://en.wikipedia.org/wiki/Leaky_integrate-and-fire
 */
class Neuron {
public:
    /**
     * @brief Constructs a neuron with a unique identifier
     * @param id Unique identifier for this neuron within the brain
     * @details Creates a new neuron instance. Memory allocation depends on the
     *          thread safety policy (see MemoryPool.hpp for details).
     */
    explicit Neuron(NeuronId id);

    /**
     * @brief Destructor cleans up neuron resources
     * @details Returns neuron to memory pool if using memory pool allocation,
     *          otherwise frees the implementation object.
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor is deleted (non-copyable)
     * @details Neurons cannot be copied due to the pImpl pattern and memory management
     *          complexities. Use move operations or create new neurons instead.
     */
    Neuron(const Neuron&) = delete;

    /**
     * @brief Copy assignment operator is deleted (non-assignable)
     * @details Maintains the pImpl encapsulation and prevents unintended sharing
     *          of neuron resources.
     */
    Neuron& operator=(const Neuron&) = delete;

    /**
     * @brief Move constructor transfers ownership of implementation
     * @param other The neuron to move from (becomes invalid after move)
     * @details Efficiently transfers ownership of the pImpl object without
     *          deep copying neuron state. The source neuron becomes unusable.
     */
    Neuron(Neuron&& other) noexcept;

    /**
     * @brief Move assignment operator transfers ownership of implementation
     * @param other The neuron to move from (becomes invalid after move)
     * @return Reference to this neuron for chaining
     * @details Replaces current implementation with other's implementation and
     *          cleans up the old implementation. The source neuron becomes invalid.
     */
    Neuron& operator=(Neuron&& other) noexcept;

    // Identity operations
    
    /**
     * @brief Gets the unique identifier of this neuron
     * @return NeuronId The neuron's unique identifier
     * @details The identifier is set during construction and remains constant
     *          throughout the neuron's lifetime.
     */
    NeuronId getId() const;

    /**
     * @brief Sets the neuron type classification
     * @param type The neuron type (Excitatory, Inhibitory, Modulatory, etc.)
     * @details Determines how the neuron affects connected synapses:
     *          - Excitatory: Positive synaptic weight changes
     *          - Inhibitory: Negative synaptic weight changes
     *          - Modulatory: Neuromodulator-based effects
     */
    void setType(NeuronType type);

    /**
     * @brief Gets the neuron type classification
     * @return NeuronType The current neuron type
     */
    NeuronType getType() const;

    // State management

    /**
     * @brief Gets a reference to the complete neuron state
     * @return const NeuronState& Const reference to the neuron's state
     * @details Provides read-only access to all neuron state variables.
     */
    const NeuronState& getState() const;

    /**
     * @brief Gets a modifiable reference to the neuron state
     * @return NeuronState& Non-const reference to the neuron's state
     * @warning Modifying neuron state directly is not recommended. Use dedicated
     *          setter methods for thread-safe state management.
     */
    NeuronState& getState();

    // Membrane potential operations

    /**
     * @brief Sets the absolute membrane potential
     * @param potential New membrane potential value (mV)
     * @details Direct modification of membrane potential. For spike-driven updates,
     *          use the simulation engine.
     */
    void setMembranePotential(MembranePotential potential);

    /**
     * @brief Gets the current membrane potential
     * @return MembranePotential Current membrane potential (mV)
     */
    MembranePotential getMembranePotential() const;

    /**
     * @brief Increments membrane potential by a delta value
     * @param delta Change in membrane potential (mV)
     * @details Thread-safe addition operation used during synaptic integration.
     */
    void addToMembranePotential(MembranePotential delta);

    // Threshold and firing control

    /**
     * @brief Sets the firing threshold potential
     * @param threshold New threshold value (mV)
     * @details Neurons fire when membrane potential exceeds this value.
     *          Typical values: -50mV to -20mV for excitatory neurons.
     */
    void setThreshold(MembranePotential threshold);

    /**
     * @brief Gets the current firing threshold
     * @return MembranePotential The firing threshold (mV)
     */
    MembranePotential getThreshold() const;

    /**
     * @brief Checks if the neuron is currently capable of firing
     * @return bool true if neuron can fire (membrane potential >= threshold), false otherwise
     * @details Returns true regardless of refractory period. Use isFiring() for
     *          actual firing capability including refractory state.
     */
    bool checkThreshold() const;

    /**
     * @brief Checks if the neuron is currently firing
     * @return bool true if neuron is in Firing state, false otherwise
     */
    bool isFiring() const;

    /**
     * @brief Checks if neuron is in refractory period
     * @return bool true if neuron cannot fire due to refractory period
     */
    bool isRefractory() const;

    /**
     * @brief Sets the firing state of the neuron
     * @param state The desired firing state (Resting, Firing, Refractory)
     * @warning This is a low-level function. Use the simulation engine for normal
     *          firing state management.
     */
    void setFiringState(FiringState state);

    /**
     * @brief Sets the refractory period duration
     * @param steps Number of simulation steps the neuron should remain refractory
     * @details Neurons cannot fire during the refractory period regardless of
     *          membrane potential.
     */
    void setRefractoryPeriod(uint32_t steps);

    /**
     * @brief Decrements the refractory timer by one step
     * @details Should be called once per simulation step. When refractory period
     *          reaches zero, neuron may fire again if conditions are met.
     */
    void decrementRefractory();

    // Rate-based computations

    /**
     * @brief Sets the firing rate (for rate-based simulation modes)
     * @param rate Firing rate in Hertz (Hz)
     */
    void setFiringRate(FiringRate rate);

    /**
     * @brief Gets the current firing rate
     * @return FiringRate Current firing rate (Hz)
     */
    FiringRate getFiringRate() const;

    // LIF neuron parameters

    /**
     * @brief Sets the leak conductance parameter
     * @param conductance Leak conductance (nS). Typical values: 5-20 nS
     * @details Controls the rate of membrane potential decay towards resting potential.
     *          Higher conductance = faster decay.
     */
    void setLeakConductance(MembranePotential conductance);

    /**
     * @brief Gets the leak conductance parameter
     * @return MembranePotential The leak conductance (nS)
     */
    MembranePotential getLeakConductance() const;

    /**
     * @brief Sets the resting potential
     * @param potential New resting potential (mV)
     * @details Steady-state membrane potential in the absence of inputs.
     */
    void setRestingPotential(MembranePotential potential);

    /**
     * @brief Gets the resting potential
     * @return MembranePotential The resting potential (mV)
     */
    MembranePotential getRestingPotential() const;

    /**
     * @brief Sets the post-spike reset potential
     * @param potential Reset potential (mV)
     * @details Membrane potential immediately after a spike (typically below resting).
     */
    void setResetPotential(MembranePotential potential);

    /**
     * @brief Gets the post-spike reset potential
     * @return MembranePotential The reset potential (mV)
     */
    MembranePotential getResetPotential() const;

    /**
     * @brief Gets the refractory period duration
     * @return uint32_t Number of steps the neuron remains refractory
     */
    uint32_t getRefractoryPeriod() const;

    // Spike operations

    /**
     * @brief Gets the timestamp of the last spike
     * @return float Time of last spike in simulation steps, or -1.0 if no spike
     */
    float getLastSpikeTime() const;

    /**
     * @brief Advances the neuron by one simulation step using vectorized LIF dynamics
     * @param timestep Simulation time step (ms)
     * @param currentVoltage Current membrane potential before update
     * @param inputCurrent Total synaptic current input (nA)
     * @param maxVoltage Maximum allowable membrane potential
     * @param externalInput External input current (nA)
     * @return Pair of updated voltage and spike status
     * @details Vectorized version using SIMD instructions for performance.
     *          Suitable for batch processing of many neurons.
     */
    std::pair<MembranePotential, bool> stepLIFVectorized(
        float timestep,
        float currentVoltage,
        float inputCurrent,
        float maxVoltage = -30.0f,
        float externalInput = 0.0f
    );

    /**
     * @brief Advances the neuron by one simulation step (scalar version)
     * @param timestep Simulation time step (ms)
     * @return bool True if neuron spiked during this step
     * @details Scalar version of LIF dynamics. Maintains full precision and is
     *          suitable for single neuron simulations or reference implementations.
     */
    bool stepLIF(float timestep);

    /**
     * @brief Computes spike statistics for a batch of neurons
     * @param voltages Array of membrane potentials
     * @param spikes Array of spike flags (output parameter)
     * @param count Number of neurons in the batch
     * @param timestep Simulation time step
     * @return Statistics::SpikeStats Computed spike statistics
     */
    static Statistics::SpikeStats computeSpikeStats(
        const float* voltages,
        bool* spikes,
        size_t count,
        float timestep
    );

    // Memory and resource management

    /**
     * @brief Adds neuron to a memory pool
     * @details This is typically called by the Brain::addNeuron method.
     *          Neurons allocated through memory pools have optimized memory usage
     *          and reduced allocation overhead.
     */
    void addToMemoryPool();

    /**
     * @brief Returns neuron to memory pool
     * @details Frees neuron's resources for reuse. Memory pools manage neuron
     *          lifecycle efficiently for large networks.
     */
    void returnToMemoryPool();

    // Debugging and monitoring

    /**
     * @brief Logs neuron status for debugging and monitoring
     * @details Outputs neuron state information to the logging system. Useful for
     *          debugging and system monitoring.
     */
    void logStatus() const;

private:
    /**
     * @brief pImpl class for neuron implementation details
     * @details Contains all neuron implementation details including:
     *          - Neuron state and parameters
     *          - Synaptic connections
     *          - Plasticity flags
     *          - Memory management
     * 
     * The pImpl pattern encapsulates implementation details and provides
     * thread-safe access to neuron state.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
