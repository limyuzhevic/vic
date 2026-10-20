#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <cstring>  // For memset and memcpy

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Optimized neuron data layout using Structure-of-Arrays (SoA) for cache efficiency
// This layout is much more efficient for batch processing and SIMD operations
struct NeuronSOA {
    // All neuron state variables stored in separate arrays (contiguous memory)
    std::vector<MembranePotential> membranePotentials;     // Current membrane potential (mV)
    std::vector<MembranePotential> restingPotentials;     // Resting potential (mV)
    std::vector<MembranePotential> thresholds;            // Firing threshold (mV)
    std::vector<MembranePotential> resetPotentials;       // Post-spike reset value (mV)
    std::vector<MembranePotential> leakConductances;      // Leak conductance (nS)
    std::vector<MembranePotential> synapticConductances;  // Synaptic conductance (nS)
    std::vector<FiringRate> firingRates;                 // Current firing rate (Hz)
    std::vector<FiringState> firingStates;               // Current firing state
    std::vector<uint32_t> refractoryRemainings;          // Steps remaining in refractory period
    std::vector<uint32_t> refractoryPeriods;             // Total refractory period (steps)
    std::vector<float> adaptationVariables;              // For spike-frequency adaptation
    std::vector<float> lastSpikeTimes;                   // Timestamp of last spike (-1 if none)
    
    // Performance metadata
    size_t capacity;                                      // Allocated capacity
    bool isInitialized;                                   // Initialization flag
    
    NeuronSOA() : capacity(0), isInitialized(false) {}
    
    // Initialize with given capacity
    void initialize(size_t capacity) {
        this->capacity = capacity;
        membranePotentials.resize(capacity);
        restingPotentials.resize(capacity);
        thresholds.resize(capacity);
        resetPotentials.resize(capacity);
        leakConductances.resize(capacity);
        synapticConductances.resize(capacity);
        firingRates.resize(capacity);
        firingStates.resize(capacity);
        refractoryRemainings.resize(capacity);
        refractoryPeriods.resize(capacity);
        adaptationVariables.resize(capacity);
        lastSpikeTimes.resize(capacity);
        isInitialized = true;
        
        // Set default values
        const MembranePotential defaultMembranePotential = -70.0f;
        const MembranePotential defaultRestingPotential = -70.0f;
        const MembranePotential defaultThreshold = -55.0f;
        const MembranePotential defaultResetPotential = -70.0f;
        const MembranePotential defaultLeakConductance = 10.0f;
        
        const FiringState defaultFiringState = FiringState::Resting;
        const float defaultLastSpikeTime = -1.0f;
        const uint32_t defaultRefractoryPeriod = 5;
        
        for (size_t i = 0; i < capacity; ++i) {
            membranePotentials[i] = defaultMembranePotential;
            restingPotentials[i] = defaultRestingPotential;
            thresholds[i] = defaultThreshold;
            resetPotentials[i] = defaultResetPotential;
            leakConductances[i] = defaultLeakConductance;
            synapticConductances[i] = 0.0f;
            firingRates[i] = 0.0f;
            firingStates[i] = defaultFiringState;
            refractoryRemainings[i] = 0;
            refractoryPeriods[i] = defaultRefractoryPeriod;
            adaptationVariables[i] = 0.0f;
            lastSpikeTimes[i] = defaultLastSpikeTime;
        }
    }
    
    // Reset neuron to default state
    void resetNeuron(size_t index) {
        if (index < capacity) {
            membranePotentials[index] = -70.0f;
            restingPotentials[index] = -70.0f;
            thresholds[index] = -55.0f;
            resetPotentials[index] = -70.0f;
            leakConductances[index] = 10.0f;
            synapticConductances[index] = 0.0f;
            firingRates[index] = 0.0f;
            firingStates[index] = FiringState::Resting;
            refractoryRemainings[index] = 0;
            refractoryPeriods[index] = 5;
            adaptationVariables[index] = 0.0f;
            lastSpikeTimes[index] = -1.0f;
        }
    }
    
    // Fast batch operation for all neurons
    void resetAll() {
        const MembranePotential defaultMembranePotential = -70.0f;
        const MembranePotential defaultRestingPotential = -70.0f;
        const MembranePotential defaultThreshold = -55.0f;
        const MembranePotential defaultResetPotential = -70.0f;
        const MembranePotential defaultLeakConductance = 10.0f;
        
        const FiringState defaultFiringState = FiringState::Resting;
        const float defaultLastSpikeTime = -1.0f;
        const uint32_t defaultRefractoryPeriod = 5;
        
        for (size_t i = 0; i < capacity; ++i) {
            membranePotentials[i] = defaultMembranePotential;
            restingPotentials[i] = defaultRestingPotential;
            thresholds[i] = defaultThreshold;
            resetPotentials[i] = defaultResetPotential;
            leakConductances[i] = defaultLeakConductance;
            synapticConductances[i] = 0.0f;
            firingRates[i] = 0.0f;
            firingStates[i] = defaultFiringState;
            refractoryRemainings[i] = 0;
            refractoryPeriods[i] = defaultRefractoryPeriod;
            adaptationVariables[i] = 0.0f;
            lastSpikeTimes[i] = defaultLastSpikeTime;
        }
    }
};

// Lightweight neuron wrapper for backward compatibility and individual neuron access
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
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
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
