#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Detailed statistics about neuron state
// Used for monitoring and analysis
struct NeuronStats {
    float firingRate;              // Average firing rate (Hz)
    float adaptationLevel;         // Current adaptation variable (0-1)
    float membranePotential;       // Current membrane potential (mV)
    float thresholdDistance;       // Distance to firing threshold (mV)
    uint32_t spikeCount;           // Total spikes in current step
    uint32_t recentSpikes;         // Spikes in last 100 steps
    float energyConsumption;       // Approximate energy used this step
    bool isActive;                 // Currently in active state
};

// Neuron state structure for efficient storage
// Contains all dynamic state variables for LIF computation
// All values use biologically realistic units
struct NeuronState {
    // Membrane dynamics
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV) ~ -70mV
    MembranePotential threshold;           // Firing threshold (mV) ~ -55mV
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    
    // Firing dynamics
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    
    // Adaptation and history
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    // Internal state tracking
    uint64_t spikeCount;                   // Total spikes since reset
    float energyUsed;                      // Energy consumed this step
    
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
        , lastSpikeTime(-1.0f)
        , spikeCount(0)
        , energyUsed(0.0f) {}
};

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
// Key features:
// - Event-driven spike propagation
// - Adaptive firing rate based on recent activity
// - Multiple input types (excitatory, inhibitory, modulatory)
// - Spike frequency adaptation
// - Refractory period protection
// - Comprehensive plasticity state tracking
//
// Design principles:
// - Strong typing for all neural parameters (MembranePotential, FiringRate, etc.)
// - Pimpl idiom for implementation hiding
// - Move semantics for efficient copying
// - Thread-safe operations where appropriate
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
    NeuronId getId() const;
    
    // Type
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State access
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane potential
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
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
    // Implements the main integration step using LIF dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state and clear history
    void reset();
    
    // Initialize neuron with random biological parameters
    // Sets realistic membrane potentials, thresholds, and dynamics
    void initializeRandom(RandomGenerator& rng);
    
    // Get detailed statistics about neuron state
    // Returns firing rate, adaptation level, and other metrics
    NeuronStats getStats() const;
    
    // Check if neuron can accept new input
    // Returns true if neuron is not refractory
    bool canReceiveInput() const;
    
    // Calculate energy consumption for current step
    // Uses biophysical model to estimate energy usage
    float calculateEnergyConsumption() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
