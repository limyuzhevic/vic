#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <vector>
#include <array>
#include <atomic>
#include <limits>
#include <cassert>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron error categories
enum class NeuronError {
    INVALID_STATE,
    OUT_OF_RANGE,
    INVALID_INPUT,
    SIMULATION_ERROR,
    PLASTICITY_ERROR,
    THRESHOLD_VIOLATION
};

// Enhanced Neuron state structure with better organization
struct NeuronState {
    // Membrane dynamics
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    
    // Firing state
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    
    // Refractory period
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    
    // Adaptation variables for spike-frequency adaptation
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    float spikeCount;                      // Total number of spikes (for metrics)
    
    // Stability and safety fields
    mutable bool stabilityWarning;         // Flag for numerical stability warnings
    mutable int instabilityCounter;        // Counter for instability events
    
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
        , stabilityWarning(false)
        , instabilityCounter(0) {}
    
    // Validate state consistency
    bool isValid() const {
        if (threshold <= restingPotential) {
            return false; // Threshold should be above resting potential
        }
        if (resetPotential != restingPotential && std::abs(resetPotential - restingPotential) > 20.0f) {
            return false; // Reset potential should be close to resting
        }
        if (leakConductance < 0.0f || leakConductance > 100.0f) {
            return false; // Leak conductance out of range
        }
        if (refractoryRemaining > refractoryPeriod) {
            return false; // Refractory counter invalid
        }
        return true;
    }
    
    // Get membrane potential with bounds checking
    MembranePotential getSafeMembranePotential() const {
        return std::clamp(membranePotential, -100.0f, 50.0f);
    }
};

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics with enhanced stability
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
    NeuronId getId() const { return pImpl->id; }
    
    // Type
    NeuronType getType() const { return pImpl->type; }
    void setType(NeuronType type);
    
    // State access
    const NeuronState& getState() const { return pImpl->state; }
    NeuronState& getState() { return pImpl->state; }
    
    // Membrane potential with validation
    MembranePotential getMembranePotential() const { return pImpl->state.membranePotential; }
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold with validation
    MembranePotential getThreshold() const { return pImpl->state.threshold; }
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    bool isFiring() const { return pImpl->state.firingState == FiringState::Active; }
    bool isRefractory() const { return pImpl->state.refractoryRemaining > 0; }
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const { return pImpl->state.firingRate; }
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const { return pImpl->state.leakConductance; }
    uint32_t getRefractoryPeriod() const { return pImpl->state.refractoryPeriod; }
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const { return pImpl->state.restingPotential; }
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    bool checkThreshold() const { return pImpl->state.membranePotential >= pImpl->state.threshold; }
    float getLastSpikeTime() const { return pImpl->state.lastSpikeTime; }
    uint64_t getTotalSpikes() const { return pImpl->state.spikeCount; }
    
    // LIF step function - returns true if neuron fired (with enhanced stability)
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials) with validation
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources) with validation
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const { return pImpl->totalCurrent; }
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const { return pImpl->spikeHistory; }
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const { return pImpl->incomingSynapses; }
    const std::vector<SynapseHandle>& getOutgoingSynapses() const { return pImpl->outgoingSynapses; }
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const { return pImpl->plasticityFlags; }
    PlasticityFlags& getPlasticityFlags() { return pImpl->plasticityFlags; }
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const { return pImpl->regionId; }
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const { return pImpl->populationId; }
    
    // Update neuron for one simulation step with enhanced error handling
    void step(Timestamp currentTime);
    
    // Reset to initial state with validation
    void reset();
    
    // Initialize with random parameters with validation
    void initializeRandom(RandomGenerator& rng);
    
    // Error handling and validation
    void validateState() const;
    void throwIfError(NeuronError error, const std::string& message) const;
    
    // Performance and stability metrics
    struct PerformanceMetrics {
        uint64_t spikeCount = 0;
        uint64_t totalSteps = 0;
        uint64_t refractoryExits = 0;
        float avgFiringRate = 0.0f;
        float maxMembranePotential = -std::numeric_limits<float>::infinity();
        float minMembranePotential = std::numeric_limits<float>::infinity();
        uint64_t instabilityEvents = 0;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Configuration-based parameter setting
    static std::shared_ptr<Config> getDefaultNeuronConfig();
    void applyConfig(const Config& config);
    
    // Memory optimization
    static size_t getMemoryUsage() { return sizeof(Neuron) + sizeof(Impl); }
    
private:
    // Internal implementation details
    struct Impl {
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
        
        Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
                 totalCurrent(0.0f), synapticInput(0.0f) {}
    };
    
    Impl* pImpl;
    
    // Helper functions
    void updateStabilityWarnings();
    void clampMembranePotential();
    static float calculateMembranePotentialChange(
        const NeuronState& state, 
        MembranePotential synapticInput, 
        TimestepDuration dt);
};

} // namespace nlm
