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
// Implements Leaky Integrate-and-Fire (LIF) dynamics with biologically realistic behavior
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
    
    // ========== IDENTITY ==========
    
    /// Get the unique ID of this neuron
    NeuronId getId() const;
    
    // ========== TYPE ==========
    
    /// Get the neuron type (Internal, Sensory, Motor, etc.)
    NeuronType getType() const;
    
    /// Set the neuron type
    void setType(NeuronType type);
    
    // ========== STATE ==========
    
    /// Get constant reference to neuron state (membrane potential, firing rate, etc.)
    const NeuronState& getState() const;
    
    /// Get mutable reference to neuron state
    NeuronState& getState();
    
    // ========== MEMBRANE DYNAMICS ==========
    
    /// Get current membrane potential in mV
    MembranePotential getMembranePotential() const;
    
    /// Set absolute membrane potential
    void setMembranePotential(MembranePotential potential);
    
    /// Add delta to membrane potential (synaptic input)
    void addToMembranePotential(MembranePotential delta);
    
    /// Get firing threshold in mV
    MembranePotential getThreshold() const;
    
    /// Set firing threshold
    void setThreshold(MembranePotential threshold);
    
    // ========== FIRING STATE ==========
    
    /// Check if neuron is currently firing (above threshold)
    bool isFiring() const;
    
    /// Check if neuron is in refractory period
    bool isRefractory() const;
    
    /// Set firing state (Resting, Active, Refractory)
    void setFiringState(FiringState state);
    
    /// Set refractory period duration in steps
    void setRefractoryPeriod(uint32_t steps);
    
    /// Decrement refractory counter by one step
    void decrementRefractory();
    
    // ========== LIF PARAMETERS ==========
    
    /// Set leak conductance (nS)
    void setLeakConductance(MembranePotential conductance);
    
    /// Get leak conductance (nS)
    MembranePotential getLeakConductance() const;
    
    /// Get refractory period duration (steps)
    uint32_t getRefractoryPeriod() const;
    
    /// Set resting membrane potential (mV)
    void setRestingPotential(MembranePotential potential);
    
    /// Get resting membrane potential (mV)
    MembranePotential getRestingPotential() const;
    
    /// Set reset potential after spike (mV)
    void setResetPotential(MembranePotential potential);
    
    /// Get reset potential after spike (mV)
    MembranePotential getResetPotential() const;
    
    // ========== SPIKE DETECTION ==========
    
    /// Check if membrane potential exceeds threshold (without spike reset)
    bool checkThreshold() const;
    
    /// Get timestamp of last spike (-1 if none)
    float getLastSpikeTime() const;
    
    // ========== LIF COMPUTATION ==========
    
    /// LIF step function - returns true if neuron fired (legacy interface)
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // ========== INPUT SIGNALS ==========
    
    /// Receive excitatory synaptic input
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /// Receive inhibitory synaptic input
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /// Receive modulatory input (neuromodulators)
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // ========== CURRENT INJECTION ==========
    
    /// Inject external current (e.g., from sensory input)
    void injectCurrent(MembranePotential current);
    
    /// Get total current (not currently used, kept for compatibility)
    MembranePotential getTotalCurrent() const;
    
    /// Clear accumulated synaptic input
    void clearTotalCurrent();
    
    // ========== SPIKE HISTORY ==========
    
    /// Record spike timestamp for plasticity
    void recordSpike(Timestamp timestamp);
    
    /// Get recent spike history (last 100 spikes)
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /// Clear spike history
    void clearSpikeHistory();
    
    // ========== SYNAPSE MANAGEMENT ==========
    
    /// Add incoming synapse connection
    void addIncomingSynapse(SynapseHandle handle);
    
    /// Add outgoing synapse connection
    void addOutgoingSynapse(SynapseHandle handle);
    
    /// Get list of incoming synapse handles
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /// Get list of outgoing synapse handles
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // ========== PLASTICITY ==========
    
    /// Get plasticity flags (Hebbian, STDP, Reward-modulated)
    const PlasticityFlags& getPlasticityFlags() const;
    
    /// Get mutable plasticity flags
    PlasticityFlags& getPlasticityFlags();
    
    /// Enable specific plasticity mechanisms
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // ========== REGION/POPULATION MEMBERSHIP ==========
    
    /// Set region ID for brain region organization
    void setRegionId(RegionId region);
    
    /// Get region ID
    RegionId getRegionId() const;
    
    /// Set population ID for neuron grouping
    void setPopulationId(PopulationId population);
    
    /// Get population ID
    PopulationId getPopulationId() const;
    
    // ========== SIMULATION ==========
    
    /// Update neuron for one simulation step with standard timestep
    /// Uses real integrate-and-fire dynamics with biologically realistic parameters
    void step(Timestamp currentTime);

    /// Update neuron for one simulation step with explicit timestep
    /// Uses real integrate-and-fire dynamics with biologically realistic parameters
    void step(Timestamp currentTime, TimestepDuration dt);

    /// Reset neuron to initial state (resting potential, clear spikes)
    void reset();
    
    /// Initialize neuron with biologically plausible random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
