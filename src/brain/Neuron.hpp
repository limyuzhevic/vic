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
// Implements Leaky Integrate-and-Fire (LIF) dynamics with spike-frequency adaptation
// This class models individual neurons with biological properties including
// membrane potential dynamics, refractory periods, and synaptic inputs
class Neuron {
public:
    // Create neuron with unique ID
    explicit Neuron(NeuronId id);
    
    // Destructor
    ~Neuron();
    
    // Disable copying for memory safety, enable moving semantics
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity management
    // Returns unique neuron identifier
    NeuronId getId() const;
    
    // Neuron type classification
    // Returns neuron type (Sensory, Motor, Excitatory, Inhibitory, etc.)
    NeuronType getType() const;
    // Sets neuron type for specialized behavior
    void setType(NeuronType type);
    
    // State access methods
    // Returns constant reference to neuron state (read-only access)
    const NeuronState& getState() const;
    // Returns mutable reference to neuron state (allows modification)
    NeuronState& getState();
    
    // Membrane potential manipulation
    // Gets current membrane potential (mV)
    MembranePotential getMembranePotential() const;
    // Sets absolute membrane potential (used for reset)
    void setMembranePotential(MembranePotential potential);
    // Adds delta to membrane potential (incremental updates)
    void addToMembranePotential(MembranePotential delta);
    
    // Firing threshold management
    // Gets current firing threshold (mV) - spike when potential exceeds this
    MembranePotential getThreshold() const;
    // Sets new firing threshold (typically -55mV for typical neurons)
    void setThreshold(MembranePotential threshold);
    
    // Firing state management
    // Checks if neuron is currently firing (Active state)
    bool isFiring() const;
    // Checks if neuron is in refractory period (cannot fire)
    bool isRefractory() const;
    // Sets firing state for state machine control
    void setFiringState(FiringState state);
    // Sets refractory period duration in simulation steps
    void setRefractoryPeriod(uint32_t steps);
    // Decrements refractory counter (called each simulation step)
    void decrementRefractory();
    
    // Firing rate tracking (for rate-based computations)
    // Gets current firing rate (Hz)
    FiringRate getFiringRate() const;
    // Sets firing rate (used for rate-based plasticity)
    void setFiringRate(FiringRate rate);
    
    // LIF neuron biophysical parameters
    // Sets membrane leak conductance (nS) - determines time constant
    void setLeakConductance(MembranePotential conductance);
    // Gets membrane leak conductance
    MembranePotential getLeakConductance() const;
    // Gets refractory period duration in steps
    uint32_t getRefractoryPeriod() const;
    // Sets resting potential (typical -70mV)
    void setRestingPotential(MembranePotential potential);
    // Gets resting potential
    MembranePotential getRestingPotential() const;
    // Sets reset potential after spike (typically -70mV)
    void setResetPotential(MembranePotential potential);
    
    // Spike detection and timing
    // Checks if membrane potential has exceeded firing threshold
    bool checkThreshold() const;
    // Gets timestamp of last spike (-1.0 if no spikes yet)
    float getLastSpikeTime() const;
    
    // LIF step function - implements discrete-time LIF dynamics
    // Integrates membrane potential using exponential Euler method
    // Returns true if neuron crossed threshold and fired a spike
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming synaptic signals (post-synaptic potentials)
    // Excitatory input: depolarizes membrane (increases potential)
    void receiveExcitatoryInput(MembranePotential amplitude);
    // Inhibitory input: hyperpolarizes membrane (decreases potential)
    void receiveInhibitoryInput(MembranePotential amplitude);
    // Modulatory input: affects plasticity but not directly integrated
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // External current injection (from sensory input or external sources)
    // Direct current injection for simulating external driving inputs
    void injectCurrent(MembranePotential current);
    // Gets total current input this simulation step
    MembranePotential getTotalCurrent() const;
    // Clears accumulated current for next simulation step
    void clearTotalCurrent();
    
    // Spike history for spike-timing-dependent plasticity (STDP)
    // Records spike timestamp for synaptic plasticity calculations
    void recordSpike(Timestamp timestamp);
    // Returns recent spike history (last ~100 spikes)
    const std::vector<Timestamp>& getSpikeHistory() const;
    // Clears spike history (used for resetting learning)
    void clearSpikeHistory();
    
    // Synapse management for connectivity
    // Adds incoming synapse handle for plasticity calculations
    void addIncomingSynapse(SynapseHandle handle);
    // Adds outgoing synapse handle for spike propagation
    void addOutgoingSynapse(SynapseHandle handle);
    // Returns list of incoming synapses for plasticity
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    // Returns list of outgoing synapses for spike propagation
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity configuration and state
    // Returns plasticity flags for synaptic modification rules
    const PlasticityFlags& getPlasticityFlags() const;
    // Returns mutable plasticity flags for modification
    PlasticityFlags& getPlasticityFlags();
    // Enables specific plasticity mechanisms
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Brain region and population membership
    // Sets which brain region this neuron belongs to
    void setRegionId(RegionId region);
    // Gets neuron region ID
    RegionId getRegionId() const;
    // Sets which population within region this neuron belongs to
    void setPopulationId(PopulationId population);
    // Gets neuron population ID
    PopulationId getPopulationId() const;
    
    // Update neuron for one complete simulation step
    // Implements full neuron dynamics including LIF integration,
    // spike detection, refractory management, and adaptation
    void step(Timestamp currentTime);
    
    // Reset neuron to initial physiological state
    // Clears all state variables and prepares for new simulation
    void reset();
    
    // Initialize neuron with biologically realistic random parameters
    // Sets random membrane properties, thresholds, and states
    void initializeRandom(RandomGenerator& rng);
    
private:
    // Pimpl idiom for implementation hiding
    struct Impl;
    Impl* pImpl;  // Pointer to implementation details
};

} // namespace nlm
