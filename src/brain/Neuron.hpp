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
// 
// This class models individual biological neurons with realistic membrane dynamics,
// including leak currents, capacitance, threshold-based firing, refractory periods,
// and spike-frequency adaptation. It's the fundamental computational unit of the
// NLM brain, capable of complex temporal dynamics and integration.
// 
// Key Features:
// - Leaky Integrate-and-Fire (LIF) membrane potential dynamics
// - Realistic biophysical parameters (capacitance, time constants)
// - Refractory periods to prevent unrealistic firing rates
// - Spike-frequency adaptation for realistic firing patterns
// - Synaptic integration with excitatory/inhibitory inputs
// - Spike history for plasticity learning
// - Plasticity flags for STDP and Hebbian learning
// - Population and region membership for network connectivity
// - Support for both rate-based and event-driven computation
// 
// Biological Plausibility:
// - Membrane potentials scale with realistic mV ranges (-100mV to +50mV)
// - Time constants based on typical neuronal values (20ms)
// - Threshold voltages around -55mV (typical for cortical neurons)
// - Refractory periods of 2-10ms based on biological observations
// - Adaptation variable for spike-frequency modulation

class Neuron {
public:
    /**
     * @brief Construct a new Neuron object with specified identifier
     * 
     * Creates a new neuron with a unique ID and initializes it with default
     * biophysical parameters. The neuron starts in a resting state with no
     * synaptic input or spike history.
     * 
     * @param id Unique identifier for this neuron
     * 
     * @throws std::bad_alloc if memory allocation fails
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy the Neuron object and clean up resources
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor (deleted - neurons cannot be copied)
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted - neurons cannot be copied)
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership of implementation
     * 
     * @param other Neuron to move from
     */
    Neuron(Neuron&& other) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership of implementation
     * 
     * @param other Neuron to move from
     * @return Neuron& Reference to this neuron
     */
    Neuron& operator=(Neuron&& other) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * 
     * @return NeuronId The neuron's ID
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron type (Sensory, Internal, or Motor)
     * 
     * @return NeuronType The neuron type
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type
     * 
     * @param type New neuron type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the complete neuron state for analysis and debugging
     * 
     * @return const NeuronState& Reference to the neuron state structure
     * 
     * @note The returned reference is read-only. Use getState() for mutable access.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable access to the neuron state for modification
     * 
     * @return NeuronState& Reference to the neuron state structure
     * 
     * @note This allows direct modification of neuron parameters for
     *       advanced use cases or testing. Use caution when modifying.
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential (electrical charge)
     * 
     * @return MembranePotential The membrane potential in millivolts (mV)
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the absolute membrane potential
     * 
     * @param potential Membrane potential in millivolts (mV)
     * 
     * @note This should only be used for initialization or testing.
     *       Use addToMembranePotential() for normal synaptic integration.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add a delta to the membrane potential
     * 
     * @param delta Change in membrane potential (mV)
     * 
     * @note This is the primary way synaptic inputs affect membrane potential.
     *       Can be positive (excitatory) or negative (inhibitory).
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold voltage
     * 
     * The threshold is the membrane potential level at which the neuron
     * becomes capable of generating an action potential (spike).
     * 
     * @return MembranePotential Threshold voltage in millivolts (mV)
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold voltage
     * 
     * @param threshold Threshold voltage in millivolts (mV)
     * 
     * @note Typical values range from -55mV to -40mV in cortical neurons.
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if the neuron is currently firing (spiking)
     * 
     * @return bool true if the neuron is in FiringState::Active or FiringState::Refractory
     */
    bool isFiring() const;
    
    /**
     * @brief Check if the neuron is in refractory period
     * 
     * @return bool true if the neuron cannot fire during this period
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the firing state of the neuron
     * 
     * @param state New firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period duration
     * 
     * @param steps Number of simulation steps the neuron will be refractory
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement the refractory period counter
     * 
     * @note This is called automatically during the simulation step.
     *       When refractoryRemaining reaches zero, the neuron can fire again.
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate of the neuron (Hz)
     * 
     * The firing rate is the inverse of the inter-spike interval, representing
     * how often the neuron generates action potentials.
     * 
     * @return FiringRate Firing rate in spikes per second (Hz)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate of the neuron
     * 
     * @param rate Firing rate in spikes per second (Hz)
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance (membrane resistance)
     * 
     * Leak conductance controls the rate at which the membrane potential
     * returns to the resting potential when there are no inputs.
     * 
     * @param conductance Leak conductance in nanoSiemens (nS)
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the leak conductance
     * 
     * @return MembranePotential Leak conductance in nanoSiemens (nS)
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period duration
     * 
     * @return uint32_t Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential (baseline voltage)
     * 
     * @param potential Resting potential in millivolts (mV)
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential
     * 
     * @return MembranePotential Resting potential in millivolts (mV)
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential (voltage after spike)
     * 
     * @param potential Reset potential in millivolts (mV)
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if the membrane potential has reached or exceeded threshold
     * 
     * @return bool true if membrane potential >= threshold
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the last spike
     * 
     * @return float Timestamp of last spike, or -1.0 if no spike has occurred
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Execute the Leaky Integrate-and-Fire (LIF) dynamics step
     * 
     * This is the core computational method that simulates neuronal dynamics
     * according to the LIF model. It processes membrane potential integration,
     * checks for threshold crossing, and handles spike generation with proper
     * refractory periods and adaptation mechanisms.
     * 
     * @param currentTime Current simulation time in milliseconds
     * @param dt Timestep duration in milliseconds
     * @return bool true if neuron fired (generated an action potential)
     * 
     * @note This method implements the differential equation:
     *       dV/dt = (V_rest - V)/tau + I/C
     *       where V is membrane potential, tau is time constant, C is capacitance,
     *       and I is total synaptic current.
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Apply excitatory synaptic input to the neuron
     * 
     * This method represents the arrival of an excitatory postsynaptic potential (EPSP)
     * that depolarizes the membrane (makes it more positive).
     * 
     * @param amplitude Amplitude of excitatory input in millivolts (mV)
     * 
     * @note Positive values increase membrane potential toward threshold.
     *       Biological basis: Glutamate binding opens Na+ channels.
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Apply inhibitory synaptic input to the neuron
     * 
     * This method represents the arrival of an inhibitory postsynaptic potential (IPSP)
     * that hyperpolarizes the membrane (makes it more negative).
     * 
     * @param amplitude Amplitude of inhibitory input in millivolts (mV)
     * 
     * @note Negative values decrease membrane potential away from threshold.
     *       Biological basis: GABA binding opens Cl- channels.
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Apply modulatory input (neuromodulation)
     * 
     * Modulatory input affects neural plasticity and excitability without
     * directly changing membrane potential. It's used for neuromodulation
     * (e.g., dopamine, acetylcholine, serotonin).
     * 
     * @param amplitude Amplitude of modulatory input
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject current directly into this neuron (external input)
     * 
     * This method represents direct current injection from external sources
     * such as sensory input or experimental stimulation.
     * 
     * @param current Current to inject in millivolts (mV)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total synaptic current injected in current timestep
     * 
     * @return MembranePotential Total current from all synaptic inputs
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear all synaptic current injections
     * 
     * @note Called automatically at the end of each LIF step.
     *       Resets the synapticInput accumulator for the next timestep.
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike timestamp for plasticity learning
     * 
     * This method records when this neuron fires, which is used by STDP
     * and Hebbian plasticity rules to update synaptic weights.
     * 
     * @param timestamp Timestamp of the spike event
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the history of spike timestamps for this neuron
     * 
     * @return const std::vector<Timestamp>& Reference to spike history vector
     * 
     * @note The spike history is limited to the most recent spikes (typically
     *       the last 100 spikes) to prevent memory issues and maintain focus
     *       on recent activity.
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear all spike history for this neuron
     * 
     * @note Useful for resetting learning state or when replaying episodes.
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add an incoming synapse connection to this neuron
     * 
     * This method establishes a synaptic connection from another neuron to this neuron.
     * The synapse is stored in a vector for efficient access during simulation.
     * 
     * @param handle Handle to the synapse (contains source and destination information)
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse connection from this neuron
     * 
     * This method establishes a synaptic connection from this neuron to another neuron.
     * The synapse is stored in a vector for efficient spike propagation.
     * 
     * @param handle Handle to the synapse (contains source and destination information)
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapse connections
     * 
     * @return const std::vector<SynapseHandle>& Reference to incoming synapses vector
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapse connections
     * 
     * @return const std::vector<SynapseHandle>& Reference to outgoing synapses vector
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the plasticity flags for this neuron
     * 
     * @return const PlasticityFlags& Reference to plasticity flags structure
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable access to plasticity flags for modification
     * 
     * @return PlasticityFlags& Reference to plasticity flags structure
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable or disable specific plasticity mechanisms
     * 
     * This method configures which plasticity rules will be applied to this neuron.
     * For example, STDP (Spike-Timing-Dependent Plasticity) enables activity-dependent
     * synaptic strengthening and weakening based on precise spike timing.
     * 
     * @param hebbian Enable Hebbian learning (co-activation based)
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the region identifier for this neuron
     * 
     * Regions group neurons into functional areas of the brain. This helps
     * organize large networks and enable different processing specializations.
     * 
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the region identifier of this neuron
     * 
     * @return RegionId The neuron's region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the population identifier for this neuron
     * 
     * Populations group neurons with similar types and functions within a region.
     * For example, a region might have sensory, internal, and motor populations.
     * 
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the population identifier of this neuron
     * 
     * @return PopulationId The neuron's population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Execute one simulation step using the default timestep
     * 
     * This is a convenience wrapper for stepLIF() with a standard timestep of 1ms.
     * It provides the basic LIF dynamics without the precision of the two-parameter version.
     * 
     * @param currentTime Current simulation time in seconds
     * 
     * @note This method is kept for API compatibility and simple use cases.
     *       For precise timing control, use stepLIF() instead.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset the neuron to its initial state
     * 
     * This method resets all neuron state variables to their default values,
     * essentially returning the neuron to a fresh initialization state. This is
     * useful for experimental protocols or when replaying saved brain states.
     * 
     * @note Does not reset synaptic connections or structural changes.
     */
    void reset();
    
    /**
     * @brief Initialize neuron parameters with random biological values
     * 
     * This method sets the neuron's biophysical parameters to realistic random values
     * based on biological distributions. It's called during brain initialization to
     * create heterogeneous neural populations.
     * 
     * @param rng Random number generator for parameter variability
     * 
     * @note Ensures biological plausibility by using realistic parameter ranges
     *       for things like threshold voltages, resting potentials, and refractory periods.
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
