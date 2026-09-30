#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <memory>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron constants namespace
namespace NeuronConstants {
    // Biological parameters
    static constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;
    static constexpr float DEFAULT_THRESHOLD = -55.0f;
    static constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;
    static constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;
    static constexpr uint32_t DEFAULT_REFECTORY_PERIOD = 5;
    
    // State initialization
    static constexpr float STATE_INITIAL_MEMBRANE_POTENTIAL = -70.0f;
    static constexpr float STATE_INITIAL_LAST_SPIKE_TIME = -1.0f;
    static constexpr float STATE_INITIAL_ADAPTATION_VARIABLE = 0.0f;
    static constexpr float STATE_INITIAL_FIRING_RATE = 0.0f;
}

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
        : membranePotential(NeuronConstants::STATE_INITIAL_MEMBRANE_POTENTIAL)
        , restingPotential(NeuronConstants::DEFAULT_RESTING_POTENTIAL)
        , threshold(NeuronConstants::DEFAULT_THRESHOLD)
        , resetPotential(NeuronConstants::DEFAULT_RESET_POTENTIAL)
        , leakConductance(NeuronConstants::DEFAULT_LEAK_CONDUCTANCE)
        , synapseConductance(0.0f)
        , firingRate(NeuronConstants::STATE_INITIAL_FIRING_RATE)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(NeuronConstants::DEFAULT_REFECTORY_PERIOD)
        , adaptationVariable(NeuronConstants::STATE_INITIAL_ADAPTATION_VARIABLE)
        , lastSpikeTime(NeuronConstants::STATE_INITIAL_LAST_SPIKE_TIME) {}
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
    Neuron(Neuron&&) noexcept = default;  // unique_ptr handles move
    Neuron& operator=(Neuron&&) noexcept = default;  // unique_ptr handles move
    
    // Ensure implementation is valid before access
    void ensureImplNotNull() const;
    
    // Identity
    NeuronId getId() const;
    
    // Type
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // Safe access to implementation
    Impl* getImpl() const;
    Impl* getImpl();
    
    // Access implementation methods safely
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
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Null pointer validation helper
    void ensureImplNotNull() const;
};

} // namespace nlm
