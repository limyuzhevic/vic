#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @brief Neuron state structure for efficient storage and management
 * 
 * This structure stores all state variables needed for Leaky Integrate-and-Fire (LIF)
 * neuron dynamics. Organized for cache efficiency with related variables grouped together.
 * 
 * Scientific Basis:
 * - MembranePotential: Represents membrane voltage following differential equation
 * - FiringState: Captures biophysical refractory period
 * - AdaptationVariable: Models spike-frequency adaptation (e.g., calcium dynamics)
 * 
 * Memory Integration:
 * - LastSpikeTime: Critical for STDP and eligibility traces
 * - PlasticityFlags: Determines how neuron participates in learning
 * - Region/Population membership: Enables spatially-organized memory networks
 */
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
    
    /**
     * @brief Default constructor initializes neuron to resting state
     * 
     * Sets biophysical parameters typical for cortical neurons:
     * - Resting potential: -70mV (standard for biological neurons)
     * - Threshold: -55mV (approx. 15mV above resting)
     * - Refractory period: 5 steps (at typical timestep)
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
 * @brief Individual neuron implementation with Leaky Integrate-and-Fire dynamics
 * 
 * This class implements the fundamental computational unit of the NLM brain.
 * Each neuron integrates inputs, generates spikes, and participates in learning
 * through plasticity rules. Neurons are organized into regions and populations
 * for spatially-organized cognitive processing.
 * 
 * Integration with Memory Systems:
 * - Working Memory: Firing neurons are recorded as memory traces
 * - Episodic Memory: Neuronal assemblies encode experiences
 * - Prediction System: Neuron activity patterns predict future states
 * 
 * Neuromodulation Integration:
 * - Dopamine modulates neuron excitability and plasticity thresholds
 * - Novelty affects adaptation variable and firing thresholds
 * - Curiosity influences adaptation dynamics for exploration
 * 
 * Memory-Reinforcement Loop:
 * 1. Sensory input → Neuron excitation
 * 2. Spike generation → Working memory storage
 * 3. Spike timing → STDP on synapses
 * 4. Success prediction → Dopamine release
 * 5. Dopamine → Weight scaling and threshold adjustment
 * 6. Updated weights → Modified future neuron responses
 * 
 * @example
 * // Create and configure a neuron
 * Neuron neuron(NeuronId(1));
 * neuron.setType(NeuronType::Internal);
 * neuron.setLeakConductance(10.0f);
 * neuron.setRestingPotential(-70.0f);
 * neuron.setThreshold(-55.0f);
 * 
 * // Initialize with random parameters
 * neuron.initializeRandom(rng);
 * 
 * // Simulate neural dynamics
 * bool spiked = neuron.stepLIF(currentTime, timestep);
 * if (spiked) {
 *     // Neuron fired - record in working memory
 *     brain->getWorkingMemory()->storeToNeuron(neuron->getId(), activity);
 * }
 */
class Neuron {
public:
    /**
     * @brief Create neuron with unique identifier
     * 
     * @param id Unique neuron identifier for tracking and connectivity
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor cleans up neuron resources
     */
    ~Neuron();
    
    /**
     * @brief Disable copying (neurons are reference objects)
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Enable move operations
     */
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get neuron's unique identifier
     * 
     * @return NeuronId Unique identifier
     */
    NeuronId getId() const;
    
    /**
     * @brief Get neuron's type and functional classification
     * 
     * @return NeuronType Classification (Sensory, Internal, Motor, etc.)
     * 
     * @note Neuron type determines connectivity patterns and functional role:
     *       - Sensory: Receives external input, encodes perceptions
     *       - Internal: Processes information, supports cognition
     *       - Motor: Generates actions, controls behavior
     */
    NeuronType getType() const;
    
    /**
     * @brief Set neuron's functional type
     * 
     * @param type New neuron type
     * 
     * @note Changing type affects connectivity and integration with cognitive systems
     */
    void setType(NeuronType type);
    
    /**
     * @brief Access current neuron state
     * 
     * @return Constant reference to neuron state (for reading)
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Access current neuron state (non-constant)
     * 
     * @return Reference to neuron state (allows modifications)
     * 
     * @note Required for plasticity and neuromodulation systems
     */
    NeuronState& getState();
    
    /**
     * @brief Get current membrane potential
     * 
     * @return Current membrane potential in mV
     * 
     * @note Values above threshold trigger spike generation
     * @see getThreshold()
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential directly
     * 
     * @param potential New membrane potential in mV
     * 
     * @warning Direct manipulation bypasses normal LIF dynamics
     *          Use injectCurrent() for physiological current injection
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Inject current to change membrane potential
     * 
     * @param delta Current magnitude in nA (positive for excitatory)
     * 
     * @note Implements current-based input in LIF model:
     *       dV/dt = (I - g_L*(V - E_L)) / C_m
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get firing threshold voltage
     * 
     * @return Threshold membrane potential in mV
     * 
     * @note Default: -55mV (approx. 15mV above resting potential)
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set firing threshold
     * 
     * @param threshold New threshold in mV
     * 
     * @note Threshold can be modulated by neuromodulators (ACh, NE)
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * 
     * @return true if neuron has recently spiked, false otherwise
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return true if neuron cannot spike, false otherwise
     */
    bool isRefractory() const;
    
    /**
     * @brief Set firing state
     * 
     * @param state New firing state (Resting, Firing, Refractory)
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period duration
     * 
     * @param steps Number of simulation steps in refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter
     * 
     * @note Called automatically during simulation steps
     */
    void decrementRefractory();
    
    /**
     * @brief Get current firing rate
     * 
     * @return Firing rate in Hz (spikes per second)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set firing rate directly
     * 
     * @param rate New firing rate in Hz
     * 
     * @note Used for rate-based computations and debugging
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Get leak conductance
     * 
     * @return Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Set leak conductance
     * 
     * @param conductance New leak conductance in nS
     * 
     * @note Controls membrane time constant (τ = C/g_L)
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get refractory period duration
     * 
     * @return Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Get resting potential
     * 
     * @return Resting membrane potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set resting potential
     * 
     * @param potential New resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get reset potential
     * 
     * @return Reset membrane potential in mV
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Set reset potential (after spike)
     * 
     * @param potential New reset potential in mV
     * 
     * @note Typically equals resting potential for simple LIF neurons
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if neuron should spike based on threshold
     * 
     * @return true if membrane potential exceeds threshold
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get timestamp of last spike
     * 
     * @return Last spike timestamp, or -1 if never spiked
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Get last spike timestamp (float version)
     * 
     * @return Last spike timestamp in ms
     */
    float getLastSpikeTimeFloat() const;
    
    /**
     * @brief Set last spike timestamp
     * 
     * @param time New spike timestamp in ms
     */
    void setLastSpikeTime(float time);
    
    /**
     * @brief Set last spike timestamp with validation
     * 
     * @param time New spike timestamp in ms
     * @param valid Whether timestamp is valid
     */
    void setLastSpikeTime(float time, bool valid);
    
    /**
     * @brief Execute Leaky Integrate-and-Fire dynamics for one timestep
     * 
     * Implements the core LIF neuron equation:
     * dV/dt = (I - g_L*(V - E_L)) / C_m
     * 
     * @param currentTime Current simulation time in ms
     * @param dt Timestep duration in ms
     * @return true if neuron spiked during this timestep
     * 
     * @note Handles refractory period, adaptation, and reset
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * @param amplitude Synaptic current magnitude in nA
     * 
     * @note Adds to membrane potential with sign appropriate for excitation
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * @param amplitude Synaptic current magnitude in nA
     * 
     * @note Subtracts from membrane potential (inhibition)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * @param amplitude Modulatory current magnitude in nA
     * 
     * @note Modulators affect plasticity thresholds and adaptation
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current (sensory, neuromodulatory)
     * 
     * @param current Current magnitude in nA
     * 
     * @note External sources include environment, neuromodulators, development
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total injected current
     * 
     * @return Sum of all external current inputs
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear total injected current
     * 
     * @note Resets current summation for next timestep
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record spike timestamp for plasticity
     * 
     * @param timestamp Spike time in ms
     * 
     * @note Maintains spike history for STDP computation
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get spike history for plasticity computation
     * 
     * @return Vector of spike timestamps in ms
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * 
     * @note Called at end of plasticity window
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse to neuron
     * 
     * @param handle Synapse handle for incoming connection
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse from neuron
     * 
     * @param handle Synapse handle for outgoing connection
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapses
     * 
     * @return Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapses
     * 
     * @return Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity flags for this neuron
     * 
     * @return Plasticity flags structure
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get plasticity flags (non-constant)
     * 
     * @return Reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity types
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set region ID for brain region organization
     * 
     * @param region Region ID
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region ID
     * 
     * @return Region ID
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population ID for local connectivity
     * 
     * @param population Population ID
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population ID
     * 
     * @return Population ID
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step (placeholder)
     * 
     * @param currentTime Current simulation time in ms
     * 
     * @note TODO: Implement full integrate-and-fire dynamics
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random biophysical parameters
     * 
     * @param rng Random number generator reference
     * 
     * @note Sets leak conductance, threshold, refractory period randomly
     *       based on configuration parameters
     */
    void initializeRandom(RandomGenerator& rng);
    
    /**
     * @brief Get synaptic efficacy for plasticity scaling
     * 
     * @return Synaptic efficacy (typically 0-1, scales plasticity)
     */
    float getSynapticEfficacy() const;
    
    /**
     * @brief Set synaptic efficacy
     * 
     * @param efficacy New efficacy value
     */
    void setSynapticEfficacy(float efficacy);
    
    /**
     * @brief Get eligibility trace for reward-modulated learning
     * 
     * @return Eligibility trace value
     */
    float getEligibilityTrace() const;
    
    /**
     * @brief Set eligibility trace
     * 
     * @param trace New eligibility trace value
     */
    void setEligibilityTrace(float trace);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
