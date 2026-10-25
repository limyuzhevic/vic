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
class Neuron final {
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept = default;
    
    // Identity
    [[nodiscard]] NeuronId getId() const noexcept;
    
    // Type
    [[nodiscard]] NeuronType getType() const noexcept;
    void setType([[maybe_unused]] NeuronType type) noexcept;
    
    // State access
    [[nodiscard]] const NeuronState& getState() const noexcept;
    [[nodiscard]] NeuronState& getState() noexcept;
    
    // Membrane potential
    [[nodiscard]] MembranePotential getMembranePotential() const noexcept;
    void setMembranePotential([[maybe_unused]] MembranePotential potential) noexcept;
    void addToMembranePotential([[maybe_unused]] MembranePotential delta) noexcept;
    
    // Threshold
    [[nodiscard]] MembranePotential getThreshold() const noexcept;
    void setThreshold([[maybe_unused]] MembranePotential threshold) noexcept;
    
    // Firing state
    [[nodiscard]] bool isFiring() const noexcept;
    [[nodiscard]] bool isRefractory() const noexcept;
    void setFiringState([[maybe_unused]] FiringState state) noexcept;
    void setRefractoryPeriod([[maybe_unused]] uint32_t steps) noexcept;
    void decrementRefractory() noexcept;
    
    // Firing rate (for rate-based computation)
    [[nodiscard]] FiringRate getFiringRate() const noexcept;
    void setFiringRate([[maybe_unused]] FiringRate rate) noexcept;
    
    // LIF neuron parameters
    void setLeakConductance([[maybe_unused]] MembranePotential conductance) noexcept;
    [[nodiscard]] MembranePotential getLeakConductance() const noexcept;
    [[nodiscard]] uint32_t getRefractoryPeriod() const noexcept;
    void setRestingPotential([[maybe_unused]] MembranePotential potential) noexcept;
    [[nodiscard]] MembranePotential getRestingPotential() const noexcept;
    void setResetPotential([[maybe_unused]] MembranePotential potential) noexcept;
    
    // Spike detection
    [[nodiscard]] bool checkThreshold() const noexcept;
    [[nodiscard]] float getLastSpikeTime() const noexcept;
    
    // LIF step function - returns true if neuron fired
    [[nodiscard]] bool stepLIF([[maybe_unused]] Timestamp currentTime, [[maybe_unused]] TimestepDuration dt) noexcept;
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput([[maybe_unused]] MembranePotential amplitude) noexcept;
    void receiveInhibitoryInput([[maybe_unused]] MembranePotential amplitude) noexcept;
    void receiveModulatoryInput([[maybe_unused]] MembranePotential amplitude) noexcept;
    
    // Current injection (from external sources)
    void injectCurrent([[maybe_unused]] MembranePotential current) noexcept;
    [[nodiscard]] MembranePotential getTotalCurrent() const noexcept;
    void clearTotalCurrent() noexcept;
    
    // Spike history (recent spikes for STDP)
    void recordSpike([[maybe_unused]] Timestamp timestamp) noexcept;
    [[nodiscard]] const std::vector<Timestamp>& getSpikeHistory() const noexcept;
    void clearSpikeHistory() noexcept;
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse([[maybe_unused]] SynapseHandle handle) noexcept;
    void addOutgoingSynapse([[maybe_unused]] SynapseHandle handle) noexcept;
    [[nodiscard]] const std::vector<SynapseHandle>& getIncomingSynapses() const noexcept;
    [[nodiscard]] const std::vector<SynapseHandle>& getOutgoingSynapses() const noexcept;
    
    // Plasticity state
    [[nodiscard]] const PlasticityFlags& getPlasticityFlags() const noexcept;
    [[nodiscard]] PlasticityFlags& getPlasticityFlags() noexcept;
    void enablePlasticity([[maybe_unused]] bool hebbian, [[maybe_unused]] bool stdp, [[maybe_unused]] bool rewardModulated) noexcept;
    
    // Region/population membership
    void setRegionId([[maybe_unused]] RegionId region) noexcept;
    [[nodiscard]] RegionId getRegionId() const noexcept;
    void setPopulationId([[maybe_unused]] PopulationId population) noexcept;
    [[nodiscard]] PopulationId getPopulationId() const noexcept;
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step([[maybe_unused]] Timestamp currentTime) noexcept;
    
    // Reset to initial state
    void reset() noexcept;
    
    // Initialize with random parameters
    void initializeRandom([[maybe_unused]] RandomGenerator& rng) noexcept;
    
private:
    struct Impl;
    [[maybe_unused]] Impl* pImpl;
};

} // namespace nlm
