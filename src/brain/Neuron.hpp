#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <optional>
#include <expected>

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
    [[nodiscard]] NeuronId getId() const noexcept;
    
    // Type
    [[nodiscard]] NeuronType getType() const noexcept;
    void setType(NeuronType type) noexcept;
    
    // State access
    [[nodiscard]] const NeuronState& getState() const noexcept;
    NeuronState& getState() noexcept;
    
    // Membrane potential
    [[nodiscard]] MembranePotential getMembranePotential() const noexcept;
    void setMembranePotential(MembranePotential potential) noexcept;
    void addToMembranePotential(MembranePotential delta) noexcept;
    
    // Threshold
    [[nodiscard]] MembranePotential getThreshold() const noexcept;
    void setThreshold(MembranePotential threshold) noexcept;
    
    // Firing state
    [[nodiscard]] bool isFiring() const noexcept;
    [[nodiscard]] bool isRefractory() const noexcept;
    void setFiringState(FiringState state) noexcept;
    void setRefractoryPeriod(uint32_t steps) noexcept;
    void decrementRefractory() noexcept;
    
    // Firing rate (for rate-based computation)
    [[nodiscard]] FiringRate getFiringRate() const noexcept;
    void setFiringRate(FiringRate rate) noexcept;
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance) noexcept;
    [[nodiscard]] MembranePotential getLeakConductance() const noexcept;
    [[nodiscard]] uint32_t getRefractoryPeriod() const noexcept;
    void setRestingPotential(MembranePotential potential) noexcept;
    [[nodiscard]] MembranePotential getRestingPotential() const noexcept;
    void setResetPotential(MembranePotential potential) noexcept;
    
    // Spike detection
    [[nodiscard]] bool checkThreshold() const noexcept;
    [[nodiscard]] float getLastSpikeTime() const noexcept;
    
    // LIF step function - returns true if neuron fired
    [[nodiscard]] bool stepLIF(Timestamp currentTime, TimestepDuration dt) noexcept;
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude) noexcept;
    void receiveInhibitoryInput(MembranePotential amplitude) noexcept;
    void receiveModulatoryInput(MembranePotential amplitude) noexcept;
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current) noexcept;
    [[nodiscard]] MembranePotential getTotalCurrent() const noexcept;
    void clearTotalCurrent() noexcept;
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp) noexcept;
    [[nodiscard]] const std::vector<Timestamp>& getSpikeHistory() const noexcept;
    void clearSpikeHistory() noexcept;
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle) noexcept;
    void addOutgoingSynapse(SynapseHandle handle) noexcept;
    [[nodiscard]] const std::vector<SynapseHandle>& getIncomingSynapses() const noexcept;
    [[nodiscard]] const std::vector<SynapseHandle>& getOutgoingSynapses() const noexcept;
    
    // Plasticity state
    [[nodiscard]] const PlasticityFlags& getPlasticityFlags() const noexcept;
    PlasticityFlags& getPlasticityFlags() noexcept;
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) noexcept;
    
    // Region/population membership
    void setRegionId(RegionId region) noexcept;
    [[nodiscard]] RegionId getRegionId() const noexcept;
    void setPopulationId(PopulationId population) noexcept;
    [[nodiscard]] PopulationId getPopulationId() const noexcept;
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime) noexcept;
    
    // Reset to initial state
    void reset() noexcept;
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng) noexcept;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
