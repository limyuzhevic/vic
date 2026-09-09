#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Neuron state structure for efficient storage
// Contains all membrane potential dynamics and firing state
// Aligned for performance: used in neural simulation loops
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
    
    // Initialize with biologically realistic default values
    NeuronState()
        : membranePotential(-70.0f)        // Typical resting potential
        , restingPotential(-70.0f)
        , threshold(-55.0f)               // Typical firing threshold
        , resetPotential(-70.0f)           // Reset close to resting
        , leakConductance(10.0f)            // Typical leak conductance
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)              // 5ms refractory period
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

// Neuron class representing a single spiking neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics with adaptation
// Optimized for high-performance neural simulation
class Neuron {
public:
    // Create neuron with unique identifier
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying (each neuron is unique), enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // ========== IDENTITY ==========
    // Get unique neuron identifier
    NeuronId getId() const;
    
    // ========== BASIC PROPERTIES ==========
    // Get neuron type (Excitatory, Inhibitory, Sensory, etc.)
    NeuronType getType() const;
    // Set neuron type for different functional roles
    void setType(NeuronType type);
    
    // ========== STATE ACCESS ==========
    // Get mutable reference to neuron state for performance-critical updates
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // ========== MEMBRANE DYNAMICS ==========
    // Get current membrane potential
    MembranePotential getMembranePotential() const;
    // Set membrane potential (used for external current injection)
    void setMembranePotential(MembranePotential potential);
    // Add to membrane potential (used by synaptic inputs)
    void addToMembranePotential(MembranePotential delta);
    
    // ========== LIF PARAMETERS ==========
    // Threshold for spike generation
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state management
    bool isFiring() const;              // Returns true if neuron is currently firing
    bool isRefractory() const;          // Returns true if neuron is in refractory period
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();         // Call every timestep to decrement refractory
    
    // Firing rate (for rate-based computations)
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // ========== SPIKE DETECTION ==========
    // Check if neuron has reached firing threshold
    bool checkThreshold() const;
    float getLastSpikeTime() const;     // Timestamp of most recent spike
    
    // ========== LIF SIMULATION ==========
    // Core LIF step function - returns true if neuron fired this step
    // Implements real differential equation: dV/dt = (V_rest - V)/tau + I/C
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // ========== INPUT/OUTPUT ==========
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude);   // EPSP
    void receiveInhibitoryInput(MembranePotential amplitude);   // IPSP
    void receiveModulatoryInput(MembranePotential amplitude);    // Neuromodulation
    
    // Current injection (from external sources like sensory input)
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // ========== SPIKE HISTORY ==========
    // Record spike for STDP and adaptation
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // ========== SYNAPSE MANAGEMENT ==========
    // Internal connections: incoming and outgoing synapses
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // ========== PLASTICITY STATE ==========
    // Plasticity flags control which learning rules apply
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // ========== NEURON LOCATION ==========
    // Which region and population this neuron belongs to
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // ========== SIMULATION INTERFACE ==========
    // Update neuron for one simulation step (default: 1ms LIF)
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset neuron to initial state
    void reset();
    
    // Initialize neuron with biologically realistic random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
