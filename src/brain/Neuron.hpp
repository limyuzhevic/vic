#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

/**
 * @brief Comprehensive neuron state structure for efficient storage
 * 
 * Contains all neuron state variables needed for Leaky Integrate-and-Fire (LIF)
 * simulation with spike-frequency adaptation. All variables are float for CPU
 * cache efficiency and fast SIMD operations.
 * 
 * @note State is updated during each timestep and used by plasticity rules.
 */
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state (Resting/Active/Refractory)
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period in simulation steps
    float adaptationVariable;              // For spike-frequency adaptation (mV)
    float lastSpikeTime;                   // Timestamp of last spike (-1.0f if none)
    
    /**
     * @brief Initialize neuron with biologically realistic default values
     * 
     * LIF neuron parameters based on cortical neuron physiology:
     * - Resting potential: -70mV (typical neuronal resting)
     * - Threshold: -55mV (spike initiation)
     * - Reset potential: -70mV (reset after spike)
     * - Leak conductance: 10 nS (determines time constant)
     * - Refractory period: 5 steps (1ms at 1ms timestep)
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
 * @brief Leaky Integrate-and-Fire (LIF) neuron model with synaptic integration
 * 
 * Implements a biologically-inspired spiking neuron with:
 * 
 * ## Core Dynamics
 * - **Membrane potential integration**: dV/dt = (V_rest - V)/τ + I/C
 * - **Spike detection**: When V >= threshold, fire and reset
 * - **Refractory period**: Prevent immediate re-firing
 * - **Spike-frequency adaptation**: Slow hyperpolarization after spikes
 * - **Synaptic input integration**: Excitatory, inhibitory, and modulatory inputs
 * 
 * ## Biological Plausibility
 * - LIF dynamics approximate cortical neuron behavior
 * - Adaptation mimics spike-frequency adaptation in real neurons
 * - Multiple input types for rich connectivity patterns
 * - Random initialization with physiological parameters
 * 
 * ## Performance Features
 * - Event-driven spike handling for efficiency
 * - Fixed timestep integration for stability
 * - SIMD-friendly data layout
 * - Cache-efficient state access
 * 
 * @note Neuron IDs provide unique identification across the network.
 * @note Neuron types (Excitatory/Inhibitory/Sensory/Motor/Internal) determine functional role.
 * 
 * @see Neuron.cpp for detailed implementation of integration and spike handling.
 */
class Neuron {
public:
    /**
     * @brief Construct neuron with unique identifier
     * 
     * @param id Unique neuron identifier (assigned by brain region)
     * 
     * @note Memory ownership is managed through pointer-to-implementation (pImpl).
     *       This enables efficient copying and moving while hiding implementation details.
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy neuron and clean up implementation
     */
    ~Neuron();
    
    // Disable copying, enable moving for performance and memory safety
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&& other) noexcept;
    Neuron& operator=(Neuron&& other) noexcept;

    /**
     * @brief Get neuron identifier
     * 
     * @return Unique neuron ID for reference and mapping
     */
    NeuronId getId() const;
    
    /**
     * @brief Set neuron type determining functional role and synaptic effects
     * 
     * @param type Neuron type from NeuronType enum
     * 
     * @note Neuron type affects:
     *       - Synaptic polarity (excitatory/inhibitory)
     *       - Input/output processing (sensory/motor/internal)
     *       - Plasticity rules (different for different types)
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get current neuron type
     * 
     * @return The neuron type
     */
    NeuronType getType() const;

    // ----- State Access Methods -----
    /**
     * @brief Get const reference to neuron state (read-only access)
     * 
     * @return Const reference to neuron state structure
     * 
     * @note Used for analysis, logging, and debugging.
     *       For modification, use direct field access or setter methods.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to neuron state (read-write access)
     * 
     * @return Mutable reference to neuron state structure
     * 
     * @note Use for advanced plasticity rules and experimental modifications.
     *       Prefer individual setter methods for normal operation.
     */
    NeuronState& getState();

    // ----- Membrane Potential Management -----
    /**
     * @brief Get current membrane potential
     * 
     * @return Current potential in millivolts (mV)
     * 
     * @note Range typically: [-100mV, +50mV] with clamping in simulation.
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set absolute membrane potential value
     * 
     * @param potential New potential value in mV
     * 
     * @note Direct assignment. Use addToMembranePotential() for input integration.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add delta potential to current membrane potential
     * 
     * @param delta Change in potential (positive mV)
     * 
     * @note Used for synaptic and external current injection.
     *       Preserves precision for small changes.
     */
    void addToMembranePotential(MembranePotential delta);

    // ----- Firing Threshold Management -----
    /**
     * @brief Get spike threshold potential
     * 
     * @return Threshold in mV (-55mV typically)
     * 
     * @note Neurone fires when membrane potential >= threshold.
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set new spike threshold
     * 
     * @param threshold New threshold potential in mV
     * 
     * @note Threshold determines neuron excitability.
     *       Lower threshold = higher excitability.
     */
    void setThreshold(MembranePotential threshold);

    // ----- Firing State Management -----
    /**
     * @brief Check if neuron is currently firing (active)
     * 
     * @return True if neuron is in active firing state
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return True if neuron cannot fire this step
     */
    bool isRefractory() const;
    
    /**
     * @brief Set firing state (used for experimental control)
     * 
     * @param state New firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period duration
     * 
     * @param steps Number of simulation steps neuron cannot fire
     * 
     * @note Prevents immediate re-firing after spike.
     *       Typical values: 2-10 steps (2-10ms at 1ms timestep).
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter by one step
     * 
     * Called during each simulation step to progress through refractory period.
     */
    void decrementRefractory();

    // ----- Firing Rate Management -----
    /**
     * @brief Get current firing rate
     * 
     * @return Firing rate in Hertz (spikes per second)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set baseline firing rate for statistics
     * 
     * @param rate Desired firing rate in Hz
     */
    void setFiringRate(FiringRate rate);

    // ----- LIF Neuron Parameters -----
    /**
     * @brief Set leak conductance (determines membrane time constant)
     * 
     * @param conductance Leak conductance in nS (10.0 typical)
     * 
     * @note τ = 1/(g_leak × C_m). Lower conductance = longer time constant.
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get current leak conductance
     * 
     * @return Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get refractory period in steps
     * 
     * @return Number of simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set resting membrane potential
     * 
     * @param potential Resting potential in mV (-70mV typical)
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get current resting potential
     * 
     * @return Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set post-spike reset potential
     * 
     * @param potential Reset potential in mV (typically same as resting)
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get current reset potential
     * 
     * @return Reset potential in mV
     */
    MembranePotential getResetPotential() const;

    // ----- Spike Detection and Recording -----
    /**
     * @brief Check if membrane potential reaches or exceeds threshold
     * 
     * @return True if neuron should fire this simulation step
     * 
     * @note Called during LIF integration before spike handling.
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get timestamp of last spike
     * 
     * @return Last spike time, or -1.0f if neuron never spiked
     */
    float getLastSpikeTime() const;

    // ----- Synaptic Integration -----
    /**
     * @brief Receive excitatory synaptic input
     * 
     * @param amplitude Synaptic current contribution (positive mV)
     * 
     * @note Called when presynaptic neuron fires and synapse is excitatory.
     *       Increases membrane potential toward threshold.
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * @param amplitude Synaptic current contribution (positive mV)
     * 
     * @note Called when presynaptic neuron fires and synapse is inhibitory.
     *       Decreases membrane potential away from threshold.
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * @param amplitude Modulatory input contribution (positive mV)
     * 
     * @note Modulatory inputs affect plasticity and excitability, not integration.
     *       Used for neuromodulation signals (dopamine, acetylcholine, etc.).
     */
    void receiveModulatoryInput(MembranePotential amplitude);

    // ----- External Current Injection -----
    /**
     * @brief Inject direct external current
     * 
     * @param current Current in mV (positive for excitation)
     * 
     * @note Used for sensory inputs or external stimulation.
     *       Bypasses synaptic dynamics for direct control.
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get accumulated synaptic input for current timestep
     * 
     * @return Total synaptic input in mV
     * 
     * @note Used for debugging and monitoring input integration.
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear accumulated synaptic input for next timestep
     * 
     * @note Called after synaptic integration before next timestep.
     */
    void clearTotalCurrent();

    // ----- Spike Recording and History -----
    /**
     * @brief Record spike event for plasticity and statistics
     * 
     * @param timestamp Time when spike occurred (in ms)
     * 
     * @note Spike history is used by STDP and other plasticity rules.
     *       History is limited to prevent unbounded growth.
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get spike time history (chronological order)
     * 
     * @return Vector of recent spike timestamps
     * 
     * @note Used by STDP to calculate precise spike timing differences.
     *       History is automatically truncated when exceeds MAX_SPIKE_HISTORY.
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear all spike history
     * 
     * @note Called during reset or when history needs to be cleared.
     */
    void clearSpikeHistory();
    
    /**
     * @brief Get number of recorded spikes
     * 
     * @return Count of spikes in history
     */
    size_t getSpikeCount() const;

    // ----- Synapse Management -----
    /**
     * @brief Add incoming synapse reference (brain-side connection)
     * 
     * @param handle Handle to the incoming synapse
     * 
     * @note Called by brain region when establishing connections.
     *       Enables synapse to update neuron state on presynaptic spikes.
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse reference (neuron-side connection)
     * 
     * @param handle Handle to the outgoing synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get list of incoming synapse handles
     * 
     * @return Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get list of outgoing synapse handles
     * 
     * @return Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;

    // ----- Plasticity State Management -----
    /**
     * @brief Get plasticity flags determining learning rules
     * 
     * @return Const reference to plasticity flags
     * 
     * @note Flags control which plasticity mechanisms are active:
     *       - Hebbian: Activity-dependent strengthening
     *       - STDP: Spike-timing-dependent plasticity
     *       - Reward-modulated: Dopamine-gated learning
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity flags for modification
     * 
     * @return Mutable reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity mechanisms
     * 
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable STDP learning
     * @param rewardModulated Enable reward-modulated learning
     * 
     * @note Plasticity mechanisms determine how synapses change based on activity
     *       and neuromodulation signals.
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);

    // ----- Network Membership -----
    /**
     * @brief Set region ID for this neuron
     * 
     * @param region Region identifier (assigned by brain)
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region ID for this neuron
     * 
     * @return Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population ID within region
     * 
     * @param population Population identifier (assigned by brain)
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population ID within region
     * 
     * @return Population identifier
     */
    PopulationId getPopulationId() const;

    // ----- Simulation Methods -----
    /**
     * @brief Perform one complete simulation timestep
     * 
     * @param currentTime Current simulation time in milliseconds
     * 
     * @note Default implementation calls stepLIF with standard timestep.
     *       Override for alternative integration schemes.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Perform Leaky Integrate-and-Fire integration
     * 
     * @param currentTime Current simulation time in ms
     * @param dt Timestep duration in seconds
     * 
     * @return True if neuron fired during this timestep
     * 
     * @note Implements differential equation:
     *       dV/dt = (V_rest - V)/τ + I/C
     *       using exponential Euler integration.
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);

    // ----- Reset and Initialization -----
    /**
     * @brief Reset neuron to initial resting state
     * 
     * @note Clears membrane potential, refractory period, and spike history.
     *       Called when brain resets or neuron is removed from simulation.
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random biological parameters
     * 
     * @param rng Random number generator reference
     * 
     * @note Sets physiologically realistic initial conditions:
     *       - Membrane potential near resting (-70 ± 3mV)
     *       - Threshold around -55mV (±2mV)
     *       - Refractory period 2-10ms (typical biological range)
     *       - Adaptation variable initialized to zero
     */
    void initializeRandom(RandomGenerator& rng);

private:
    /**
     * @brief Implementation details (pointer-to-implementation idiom)
     * 
     * @note Enables efficient copying/moving and hides implementation details.
     *       All state variables stored in implementation struct for cache efficiency.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
