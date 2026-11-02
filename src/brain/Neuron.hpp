#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @brief Complete state representation for a single neuron.
 * 
 * NeuronState contains all dynamic and static properties of a neuron in the
 * Leaky Integrate-and-Fire (LIF) model. This structure is copied and moved
 * during neuron lifecycle management and checkpoint serialization.
 * 
 * The LIF model follows the differential equation:
 * dV/dt = (V_rest - V)/τ + I/C
 * where V is membrane potential, τ is time constant, I is input current,
 * and C is membrane capacitance.
 */
struct NeuronState {
    MembranePotential membranePotential;    ///< Current membrane potential (mV)
    MembranePotential restingPotential;    ///< Resting potential (mV) - equilibrium voltage
    MembranePotential threshold;           ///< Firing threshold (mV) - spike trigger point
    MembranePotential resetPotential;       ///< Post-spike reset value (mV) - voltage after spike
    MembranePotential leakConductance;      ///< Leak conductance (nS) - scales leak term
    MembranePotential synapseConductance;   ///< Synaptic conductance (nS) - total synaptic input
    FiringRate firingRate;                 ///< Current firing rate (Hz) - exponential moving average
    FiringState firingState;               ///< Current firing state (Resting, Active, Refractory, Inhibited)
    uint32_t refractoryRemaining;          ///< Steps remaining in refractory period (0 = not refractory)
    uint32_t refractoryPeriod;             ///< Total refractory period in simulation steps (refractory recovery time)
    float adaptationVariable;              ///< For spike-frequency adaptation - slowly varying adaptation current
    float lastSpikeTime;                   ///< Timestamp of last spike (-1.0f if no spike yet)
    
    /**
     * @brief Default constructor initializes neuron with biologically plausible defaults.
     * 
     * Standard LIF neuron parameters based on typical cortical neuron characteristics:
     * - Resting potential: -70mV (typical neuronal resting state)
     * - Threshold: -55mV (50mV above rest for spike initiation)
     * - Reset: -70mV (back to rest immediately after spike)
     * - Leak conductance: 10 nS (time constant τ = 2ms)
     * - Refractory period: 9 steps (300ms at 1ms timestep, prevents immediate re-firing)
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
        , refractoryPeriod(9)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

/**
 * @brief Single neuron implementing Leaky Integrate-and-Fire (LIF) dynamics.
 * 
 * The Neuron class represents a biologically-inspired spiking neuron that
 * integrates incoming currents and generates action potentials when its
 * membrane potential reaches threshold. This is the fundamental computational
 * unit of the NLM brain simulation.
 * 
 * Key features:
 * - LIF dynamics with exponential integration
 * - Spike-frequency adaptation via adaptation variable
 * - Refractory period preventing excessive firing
 * - Synaptic input integration (excitatory/inhibitory/modulatory)
 * - Plasticity flags for learning mechanisms
 * - Real-time spike detection and reset
 * 
 * The neuron operates in discrete time steps, updating its membrane potential
 * according to the LIF equation and generating spikes when threshold is reached.
 */
class Neuron {
public:
    /**
     * @brief Constructs a neuron with unique identifier.
     * 
     * @param id Unique neuron identifier for network lookup and referencing.
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor - cleans up internal implementation details.
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor deleted - neurons are not copied.
     * 
     * Neurons use move semantics for efficient lifetime management in the
     * brain simulation, preventing unintended copies.
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator deleted - neurons are not copied.
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership of implementation.
     * 
     * Efficient transfer of implementation pointer during neuron relocation
     * and network reconfiguration.
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership of implementation.
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    // =============================================================================
    // IDENTITY AND CLASSIFICATION
    // =============================================================================
    
    /**
     * @brief Gets the neuron's unique identifier.
     * 
     * @return NeuronId Unique identifier for this neuron.
     */
    NeuronId getId() const;
    
    /**
     * @brief Gets the neuron's functional type.
     * 
     * @return NeuronType The neuron's type (Excitatory, Inhibitory, etc.).
     */
    NeuronType getType() const;
    
    /**
     * @brief Sets the neuron's type (e.g., Sensory, Motor, Internal).
     * 
     * @param type New neuron type for functional classification.
     */
    void setType(NeuronType type);
    
    // =============================================================================
    // STATE ACCESS
    // =============================================================================
    
    /**
     * @brief Gets a const reference to the neuron's complete state.
     * 
     * @return const NeuronState& Complete neuron state information.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Gets a mutable reference to the neuron's state for direct updates.
     * 
     * @return NeuronState& Mutable reference to neuron state.
     */
    NeuronState& getState();
    
    // =============================================================================
    // MEMBRANE POTENTIAL DYNAMICS
    // =============================================================================
    
    /**
     * @brief Gets the current membrane potential.
     * 
     * @return MembranePotential Current membrane potential in mV.
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Sets the absolute membrane potential value.
     * 
     * @param potential New membrane potential in mV.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Adds delta to the current membrane potential.
     * 
     * @param delta Potential change in mV (can be positive or negative).
     */
    void addToMembranePotential(MembranePotential delta);
    
    // =============================================================================
    // THRESHOLD AND FIRING CONTROL
    // =============================================================================
    
    /**
     * @brief Gets the firing threshold potential.
     * 
     * @return MembranePotential Voltage level that triggers a spike.
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Sets the firing threshold.
     * 
     * @param threshold New threshold voltage in mV.
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Checks if neuron has reached firing threshold.
     * 
     * @return bool True if membrane potential >= threshold.
     */
    bool checkThreshold() const;
    
    /**
     * @brief Gets timestamp of most recent spike.
     * 
     * @return float Timestamp in seconds of last spike, or -1.0f if none.
     */
    float getLastSpikeTime() const;
    
    // =============================================================================
    // FIRING STATE MANAGEMENT
    // =============================================================================
    
    /**
     * @brief Checks if neuron is currently firing (in Active state).
     * 
     * @return bool True if neuron is actively firing.
     */
    bool isFiring() const;
    
    /**
     * @brief Checks if neuron is in refractory period.
     * 
     * @return bool True if neuron cannot fire due to refractory constraints.
     */
    bool isRefractory() const;
    
    /**
     * @brief Sets the neuron's firing state.
     * 
     * @param state New firing state (Resting, Active, Refractory, Inhibited).
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Sets the refractory period duration.
     * 
     * @param steps Number of simulation steps for refractory period.
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrements refractory counter and updates firing state.
     * 
     * Called each simulation step to progress neuron through refractory period.
     */
    void decrementRefractory();
    
    // =============================================================================
    // FIRING RATE METRICS
    // =============================================================================
    
    /**
     * @brief Gets the neuron's current firing rate.
     * 
     * @return FiringRate Current firing rate in Hz (spikes per second).
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Sets the neuron's firing rate directly.
     * 
     * @param rate New firing rate in Hz.
     */
    void setFiringRate(FiringRate rate);
    
    // =============================================================================
    // LIF NEURON PARAMETERS
    // =============================================================================
    
    /**
     * @brief Sets the leak conductance parameter.
     * 
     * @param conductance Leak conductance in nS (inverse of time constant).
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Gets the leak conductance parameter.
     * 
     * @return MembranePotential Current leak conductance in nS.
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Gets the refractory period duration.
     * 
     * @return uint32_t Total refractory period in simulation steps.
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Sets the resting potential.
     * 
     * @param potential New resting potential in mV.
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Gets the resting potential.
     * 
     * @return MembranePotential Current resting potential in mV.
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Sets the reset potential (post-spike voltage).
     * 
     * @param potential New reset potential in mV.
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Gets the reset potential.
     * 
     * @return MembranePotential Current reset potential in mV.
     */
    MembranePotential getResetPotential() const;
    
    // =============================================================================
    // SYNAPTIC INPUT INTEGRATION
    // =============================================================================
    
    /**
     * @brief Receives excitatory synaptic input (depolarizing).
     * 
     * @param amplitude Input current in mV (positive value).
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives inhibitory synaptic input (hyperpolarizing).
     * 
     * @param amplitude Input current in mV (negative value effect).
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives modulatory synaptic input (affects plasticity).
     * 
     * @param amplitude Modulatory input that affects plasticity mechanisms.
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Injects external current (e.g., from sensory input).
     * 
     * @param current External current input in mV.
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Gets total integrated current for this timestep.
     * 
     * @return MembranePotential Total synaptic + external current.
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clears accumulated synaptic input for next timestep.
     * 
     * Called at the end of each LIF integration step.
     */
    void clearTotalCurrent();
    
    // =============================================================================
    // SPIKE RECORDING AND HISTORY
    // =============================================================================
    
    /**
     * @brief Records a spike event with timestamp.
     * 
     * @param timestamp Time when spike occurred.
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Gets history of recent spikes for STDP learning.
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps.
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clears spike history.
     */
    void clearSpikeHistory();
    
    // =============================================================================
    // SYNAPSE CONNECTIONS
    // =============================================================================
    
    /**
     * @brief Adds incoming synapse handle to this neuron.
     * 
     * @param handle SynapseHandle to incoming synaptic connection.
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Adds outgoing synapse handle to this neuron.
     * 
     * @param handle SynapseHandle to outgoing synaptic connection.
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Gets list of incoming synapse handles.
     * 
     * @return const std::vector<SynapseHandle>& Incoming synapses.
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Gets list of outgoing synapse handles.
     * 
     * @return const std::vector<SynapseHandle>& Outgoing synapses.
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // =============================================================================
    // PLASTICITY CONTROL
    // =============================================================================
    
    /**
     * @brief Gets plasticity flags for this neuron.
     * 
     * @return const PlasticityFlags& Current plasticity configuration.
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Gets mutable plasticity flags for direct modification.
     * 
     * @return PlasticityFlags& Mutable plasticity configuration.
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enables specific plasticity mechanisms for this neuron.
     * 
     * @param hebbian Enable Hebbian learning
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // =============================================================================
    // NEURAL NETWORK MEMBERSHIP
    // =============================================================================
    
    /**
     * @brief Sets the neuron's parent region identifier.
     * 
     * @param region RegionId of the neuron's containing region.
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Gets the neuron's parent region identifier.
     * 
     * @return RegionId Region identifier.
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Sets the neuron's population identifier.
     * 
     * @param population PopulationId of the neuron's functional group.
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Gets the neuron's population identifier.
     * 
     * @return PopulationId Population identifier.
     */
    PopulationId getPopulationId() const;
    
    // =============================================================================
    // SIMULATION CORE
    // =============================================================================
    
    /**
     * @brief Performs one LIF integration step and returns spike status.
     * 
     * Implements the complete Leaky Integrate-and-Fire algorithm:
     * 1. Processes refractory period
     * 2. Integrates membrane potential with synaptic currents
     * 3. Applies spike-frequency adaptation
     * 4. Detects threshold crossing
     * 5. Resets membrane potential and enters refractory period
     * 
     * @param currentTime Current simulation timestamp in seconds.
     * @param dt Timestep duration in seconds.
     * @return bool True if neuron fired during this timestep.
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Performs one simulation step using default timestep.
     * 
     * @param currentTime Current simulation timestamp in seconds.
     */
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // =============================================================================
    // BRAIN STATE MANAGEMENT
    // =============================================================================
    
    /**
     * @brief Resets neuron to initial resting state.
     * 
     * Clears membrane potential, spike history, and resets firing state.
     * Used between simulation episodes to prepare neuron for new activity.
     */
    void reset();
    
    /**
     * @brief Initializes neuron with biologically plausible random parameters.
     * 
     * Sets membrane potential, threshold, refractory period, and other parameters
     * according to random distributions that produce realistic neural behavior.
     * 
     * @param rng Random number generator for stochastic initialization.
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Internal implementation details using pImpl idiom.
     * 
     * All neuron state and implementation details are stored in a private
     * implementation structure to reduce compilation dependencies and improve
     * encapsulation.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
