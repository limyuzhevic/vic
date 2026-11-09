#pragma once

#include "../core/Types/Types.hpp"
#include <array>
#include <vector>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Optimized neuron state structure with better cache locality
// Using struct of arrays (SoA) for better memory access patterns
struct NeuronState {
    // State variables that are accessed together during processing
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapticInput;        // Accumulated synaptic input
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    // Constants for neuron parameters (could be moved to configuration)
    static constexpr uint32_t MAX_SPIKE_HISTORY = 64;  // Reduced from 100
    
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapticInput(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        // Phase 6: Optimized refractory period for better performance
        , refractoryPeriod(9)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

// Optimized spike history storage using fixed-size array for better performance
// This avoids dynamic allocation for the common case
class SpikeHistory {
private:
    std::array<Timestamp, NeuronState::MAX_SPIKE_HISTORY> spikes;
    size_t count = 0;
    
public:
    void add(Timestamp timestamp) {
        if (count < spikes.size()) {
            spikes[count++] = timestamp;
        } else {
            // Shift array and add new spike (simple circular buffer)
            std::move(spikes.begin() + 1, spikes.end(), spikes.begin());
            spikes[count - 1] = timestamp;
        }
    }
    
    size_t size() const { return count; }
    bool empty() const { return count == 0; }
    
    Timestamp operator[](size_t index) const {
        return spikes[index];
    }
    
    void clear() {
        count = 0;
    }
    
    const Timestamp* data() const {
        return spikes.data();
    }
};

// Neuron class representing a single neuron with optimized performance
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
    
    // Optimized state access for better performance
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
    
    // Optimized LIF step function - inlines for performance
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current);
    void clearTotalCurrent();  // Alias for synaptic input clearing
    
    // Optimized spike history management
    void recordSpike(Timestamp timestamp);
    const Timestamp* getSpikeHistory() const;
    size_t getSpikeHistorySize() const;
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
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
    // Performance optimization: batch operations for better throughput
    void batchAddIncomingSynapses(const std::vector<SynapseHandle>& handles);
    void batchAddOutgoingSynapses(const std::vector<SynapseHandle>& handles);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm