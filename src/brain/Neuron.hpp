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
    // TODO PHASE 2: Implement real integrate-and-fire dynamics (delegates to stepLIF)
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
