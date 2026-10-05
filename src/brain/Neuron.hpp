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
// Implements Leaky Integrate-and-Fire (LIF) dynamics with modern C++ optimizations
class Neuron {
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving with noexcept for exception safety
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
    [[nodiscard]] const NeuronState& getState() const noexcept;
    NeuronState& getState() noexcept;
    
    // Membrane potential
    [[nodiscard]] MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta) noexcept;
    
    // Threshold
    [[nodiscard]] MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state
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
    
    // Spike detection
    [[nodiscard]] bool checkThreshold() const;
    [[nodiscard]] float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    [[nodiscard]] bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude) noexcept;
    void receiveInhibitoryInput(MembranePotential amplitude) noexcept;
    void receiveModulatoryInput(MembranePotential amplitude) noexcept;
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current) noexcept;
    [[nodiscard]] MembranePotential getTotalCurrent() const;
    void clearTotalCurrent() noexcept;
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp) noexcept;
    [[nodiscard]] const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory() noexcept;
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle) noexcept;
    void addOutgoingSynapse(SynapseHandle handle) noexcept;
    [[nodiscard]] const std::vector<SynapseHandle>& getIncomingSynapses() const;
    [[nodiscard]] const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    [[nodiscard]] const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) noexcept;
    
    // Region/population membership
    void setRegionId(RegionId region);
    [[nodiscard]] RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    [[nodiscard]] PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // Real integrate-and-fire dynamics implementation
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset() noexcept;
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
    // Validation and safety checks
    [[nodiscard]] bool validate() const noexcept;
    
    // Statistics and debugging
    [[nodiscard]] float getMembranePotentialChange() const;
    [[nodiscard]] float getAdaptationLevel() const;
    [[nodiscard]] size_t getSpikeCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers for numerical stability
    static constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
    static constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
    
    // Numerical integration helper
    static MembranePotential integrateWithStability(
        MembranePotential V, 
        MembranePotential V_rest, 
        float leakRate, 
        float synapticInput, 
        float timestep) noexcept;
    
    // Safety clamping helper
    static MembranePotential clampPotential(MembranePotential V) noexcept;
};

} // namespace nlm
