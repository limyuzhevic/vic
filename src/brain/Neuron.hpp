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
    Neuron(Neuron&&) noexcept = default;
    Neuron& operator=(Neuron&&) noexcept = default;
    
    // Identity
    [[nodiscard]] NeuronId getId() const;
    
    // Type
    [[nodiscard]] NeuronType getType() const;
    void setType(NeuronType type);
    
    // State access
    [[nodiscard]] const NeuronState& getState() const;
    [[nodiscard]] NeuronState& getState();
    
    // Membrane potential operations
    [[nodiscard]] MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold operations
    [[nodiscard]] MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state checks
    [[nodiscard]] bool isFiring() const;
    [[nodiscard]] bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    [[nodiscard]] FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    [[nodiscard]] MembranePotential getLeakConductance() const;
    [[nodiscard]] uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    [[nodiscard]] MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection and management
    [[nodiscard]] bool checkThreshold() const;
    [[nodiscard]] float getLastSpikeTime() const;
    [[nodiscard]] bool isActive() const;  // Convenience method for debugging
    
    // LIF step function - returns true if neuron fired
    [[nodiscard]] bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Input handling methods
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current);
    [[nodiscard]] MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history management (recent spikes for STDP)
    void recordSpike(Timestamp timestamp);
    [[nodiscard]] const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    [[nodiscard]] const std::vector<SynapseHandle>& getIncomingSynapses() const;
    [[nodiscard]] const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state management
    [[nodiscard]] const PlasticityFlags& getPlasticityFlags() const;
    [[nodiscard]] PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    void setRegionId(RegionId region);
    [[nodiscard]] RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    [[nodiscard]] PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
    // Convenience methods for common operations
    [[nodiscard]] float getExcitability() const;  // Sum of excitatory inputs
    [[nodiscard]] float getInhibition() const;   // Sum of inhibitory inputs
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
