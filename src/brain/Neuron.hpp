#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <memory>
#include <stdexcept>
#include <limits>

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

// Neuron implementation details (Pimpl idiom)
struct Neuron::Impl;

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
    [[nodiscard]] NeuronState& getState() noexcept;
    
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
    [[nodiscard]] PlasticityFlags& getPlasticityFlags() noexcept;
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
    void initializeRandom(RandomGenerator& rng);
    
private:
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

// Forward declaration needed for Pimpl implementation
namespace nlm {
    struct Neuron::Impl {
        NeuronId id;
        NeuronType type;
        NeuronState state;
        RegionId regionId;
        PopulationId populationId;
        MembranePotential totalCurrent;  // Total synaptic current input this step
        MembranePotential synapticInput;  // Accumulated synaptic input
        std::vector<Timestamp> spikeHistory;
        std::vector<SynapseHandle> incomingSynapses;
        std::vector<SynapseHandle> outgoingSynapses;
        PlasticityFlags plasticityFlags;
        
        // LIF parameters
        static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
        static constexpr float TIME_CONSTANT = 20.0f;  // ms
        static constexpr size_t MAX_SPIKE_HISTORY = 100;
        
        Impl() noexcept : id(), type(NeuronType::Internal), regionId(), populationId(),
                          totalCurrent(0.0f), synapticInput(0.0f) {}
        
        // Move constructor
        Impl(Impl&& other) noexcept
            : id(std::move(other.id)), type(std::move(other.type)), state(std::move(other.state)),
              regionId(std::move(other.regionId)), populationId(std::move(other.populationId)),
              totalCurrent(std::move(other.totalCurrent)), synapticInput(std::move(other.synapticInput)),
              spikeHistory(std::move(other.spikeHistory)),
              incomingSynapses(std::move(other.incomingSynapses)),
              outgoingSynapses(std::move(other.outgoingSynapses)),
              plasticityFlags(std::move(other.plasticityFlags)) {}
        
        // Disable copy
        Impl(const Impl&) = delete;
        Impl& operator=(const Impl&) = delete;
    };
}
