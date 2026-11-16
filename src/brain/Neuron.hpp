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

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
class Neuron {
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    /**
     * @brief Get the unique identifier of this neuron
     * @return NeuronId The neuron's ID
     */
    NeuronId getId() const;
    
    // Type
    /**
     * @brief Get the neuron type (Sensory, Internal, Motor)
     * @return NeuronType The neuron type
     */
    NeuronType getType() const;
    /**
     * @brief Set the neuron type
     * @param type The neuron type to set
     */
    void setType(NeuronType type);
    
    // State access
    /**
     * @brief Get constant reference to neuron state
     * @return const NeuronState& Reference to the state
     */
    const NeuronState& getState() const;
    /**
     * @brief Get mutable reference to neuron state
     * @return NeuronState& Mutable reference to the state
     */
    NeuronState& getState();
    
    // Membrane potential
    /**
     * @brief Get current membrane potential
     * @return MembranePotential Current membrane potential (mV)
     */
    MembranePotential getMembranePotential() const;
    /**
     * @brief Set membrane potential directly
     * @param potential New membrane potential (mV)
     */
    void setMembranePotential(MembranePotential potential);
    /**
     * @brief Add delta to membrane potential
     * @param delta Change in potential (mV)
     */
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    /**
     * @brief Get firing threshold
     * @return MembranePotential Firing threshold (mV)
     */
    MembranePotential getThreshold() const;
    /**
     * @brief Set firing threshold
     * @param threshold New firing threshold (mV)
     */
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    /**
     * @brief Check if neuron is currently firing (membrane potential >= threshold)
     * @return bool True if neuron is firing
     */
    bool isFiring() const;
    /**
     * @brief Check if neuron is in refractory period
     * @return bool True if neuron is refractory
     */
    bool isRefractory() const;
    /**
     * @brief Set firing state
     * @param state The firing state to set
     */
    void setFiringState(FiringState state);
    /**
     * @brief Set refractory period duration
     * @param steps Number of simulation steps in refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    /**
     * @brief Decrement refractory period counter (called each simulation step)
     */
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    /**
     * @brief Get current firing rate
     * @return FiringRate Current firing rate (Hz)
     */
    FiringRate getFiringRate() const;
    /**
     * @brief Set firing rate directly
     * @param rate New firing rate (Hz)
     */
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    /**
     * @brief Set leak conductance (membrane resistance)
     * @param conductance Leak conductance (nS)
     */
    void setLeakConductance(MembranePotential conductance);
    /**
     * @brief Get leak conductance
     * @return MembranePotential Leak conductance (nS)
     */
    MembranePotential getLeakConductance() const;
    /**
     * @brief Get refractory period duration
     * @return uint32_t Number of simulation steps in refractory period
     */
    uint32_t getRefractoryPeriod() const;
    /**
     * @brief Set resting potential
     * @param potential Resting potential (mV)
     */
    void setRestingPotential(MembranePotential potential);
    /**
     * @brief Get resting potential
     * @return MembranePotential Resting potential (mV)
     */
    MembranePotential getRestingPotential() const;
    /**
     * @brief Set reset potential (membrane potential after spike)
     * @param potential Reset potential (mV)
     */
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    /**
     * @brief Check if neuron has reached firing threshold
     * @return bool True if membrane potential >= threshold
     */
    bool checkThreshold() const;
    /**
     * @brief Get time of last spike
     * @return float Last spike timestamp (-1.0 if no spike yet)
     */
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    /**
     * @brief Perform one step of Leaky Integrate-and-Fire dynamics
     * @param currentTime Current simulation timestamp
     * @param dt Time step duration (ms)
     * @return bool True if neuron fired during this step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    /**
     * @brief Receive excitatory synaptic input
     * @param amplitude Synaptic current amplitude (nA)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    /**
     * @brief Receive inhibitory synaptic input
     * @param amplitude Synaptic current amplitude (nA)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    /**
     * @brief Receive modulatory synaptic input
     * @param amplitude Synaptic current amplitude (nA)
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    /**
     * @brief Inject external current (e.g., from sensory input)
     * @param current External current (nA)
     */
    void injectCurrent(MembranePotential current);
    /**
     * @brief Get total current (synapse + external)
     * @return MembranePotential Total current (nA)
     */
    MembranePotential getTotalCurrent() const;
    /**
     * @brief Clear accumulated synaptic input for next step
     */
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    /**
     * @brief Record a spike event
     * @param timestamp Time when spike occurred
     */
    void recordSpike(Timestamp timestamp);
    /**
     * @brief Get recent spike history
     * @return const std::vector<Timestamp>& Vector of recent spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    /**
     * @brief Clear spike history
     */
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    /**
     * @brief Add incoming synapse
     * @param handle Handle of incoming synapse
     */
    void addIncomingSynapse(SynapseHandle handle);
    /**
     * @brief Add outgoing synapse
     * @param handle Handle of outgoing synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    /**
     * @brief Get incoming synapses
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    /**
     * @brief Get outgoing synapses
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    /**
     * @brief Get plasticity flags (const reference)
     * @return const PlasticityFlags& Const reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    /**
     * @brief Get plasticity flags (mutable reference)
     * @return PlasticityFlags& Mutable reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    /**
     * @brief Enable/disable plasticity mechanisms
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    /**
     * @brief Set region identifier
     * @param region Region ID
     */
    void setRegionId(RegionId region);
    /**
     * @brief Get region identifier
     * @return RegionId Region ID
     */
    RegionId getRegionId() const;
    /**
     * @brief Set population identifier
     * @param population Population ID
     */
    void setPopulationId(PopulationId population);
    /**
     * @brief Get population identifier
     * @return PopulationId Population ID
     */
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    /**
     * @brief Perform one simulation step
     * @param currentTime Current simulation timestamp
     */
    void step(Timestamp currentTime);
    
    // Reset to initial state
    /**
     * @brief Reset neuron to initial state (membrane potential, etc.)
     */
    void reset();
    
    // Initialize with random parameters
    /**
     * @brief Initialize neuron with biologically plausible random parameters
     * @param rng Random number generator
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
