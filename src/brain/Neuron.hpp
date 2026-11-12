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

// Global performance optimization settings
struct GlobalPerformanceSettings {
    bool enableInputFiltering = true;
    bool enableRefractoryOptimization = true;
    bool enableAdaptation = true;
    float maxFiringRateHz = 1000.0f;  // Hard limit to prevent runaway firing
    float inputFilteringThreshold = 50.0f;  // Threshold for input filtering
    float adaptationTimeConstant = 100.0f;  // Time constant for adaptation
    float adaptationStrength = 0.01f;      // Strength of adaptation
    
    // Performance optimization flags
    bool useSparseSynapticDelivery = true;
    bool useCompartmentalizedUpdates = false;
    bool useAsynchronousSpikePropagation = true;
    
    // Configuration
    static GlobalPerformanceSettings getDefaultSettings() {
        GlobalPerformanceSettings settings;
        settings.useSparseSynapticDelivery = true;
        settings.useCompartmentalizedUpdates = false;  // Still in development
        settings.useAsynchronousSpikePropagation = true;
        return settings;
    }
};

inline GlobalPerformanceSettings globalPerformanceSettings;

inline void setGlobalPerformanceSettings(const GlobalPerformanceSettings& settings) {
    globalPerformanceSettings = settings;
}

inline GlobalPerformanceSettings getGlobalPerformanceSettings() {
    return globalPerformanceSettings;
}

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics with performance optimizations
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
    
    // LIF neuron parameters with optimization flags
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired with optimizations
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials) with filtering
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources) with clamping
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
    
    // Update neuron for one simulation step - OPTIMIZED PHASE 2
    void step(Timestamp currentTime);
    
    // Reset to initial state with optimizations
    void reset();
    
    // Initialize with random parameters and optimization settings
    void initializeRandom(RandomGenerator& rng);
    
    // Performance optimization methods
    void enableInputFiltering(bool enabled);
    void setMaxFiringRate(float maxRate);
    void setRefractoryOptimization(bool enabled);
    void setAdaptationEnabled(bool enabled);
    
    // Statistics and monitoring
    size_t getRecentSpikeCount(size_t windowSteps) const;
    float getFiringRateEstimate(size_t windowSteps) const;
    
    // Configuration methods
    void configureRefractoryPeriod(float maxFiringRateHz);
    void configureAdaptationParameters(float timeConstant, float strength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
