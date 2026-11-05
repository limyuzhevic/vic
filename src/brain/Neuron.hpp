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

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
// 
// This is the core computational unit of the NLM brain. Each neuron:
// - Receives excitatory and inhibitory inputs
// - Maintains membrane potential with exponential decay
// - Spikes when threshold is reached, generating output signals
// - Has refractory periods and adaptation mechanisms
// - Supports plasticity rules for learning
// - Maintains spike history for STDP computation
//
// Key behaviors:
// 1. Input integration: sums excitatory/inhibitory currents
// 2. Membrane dynamics: exponential decay toward resting potential
// 3. Threshold crossing: generates spikes when V >= threshold
// 4. Refractory period: prevents immediate re-firing
// 5. Reset: membrane potential is reset after spiking
// 6. Adaptation: adaptation variable modulates firing rate over time
//
// This implementation follows the standard LIF model used in neural
// simulations, with extensions for spike-based plasticity and
// neuromodulation effects common in biological neurons.
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
    
    // ========== IDENTITY ==========
    
    // Get unique neuron identifier
    NeuronId getId() const;
    
    // ========== NEURON TYPE ==========
    
    // Get neuron type (Sensory, Motor, Internal, Inhibitory, etc.)
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // ========== STATE ACCESS ==========
    
    // Get full neuron state (for inspection/debugging)
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // ========== MEMBRANE DYNAMICS ==========
    
    // Get current membrane potential
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // ========== THRESHOLD AND FIRING ==========
    
    // Get firing threshold potential
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Check if neuron is currently firing or has fired this step
    bool isFiring() const;
    
    // Check if neuron is in refractory period
    bool isRefractory() const;
    
    // Set firing state (used for refractory management)
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Get firing rate (Hz)
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // ========== LIF PARAMETERS ==========
    
    // Leak conductance (nS) - controls exponential decay rate
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    
    // Refractory period in steps (see NeuronState for details)
    uint32_t getRefractoryPeriod() const;
    
    // Resting and reset potentials
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // ========== SPIKE DETECTION ==========
    
    // Check if membrane potential exceeds threshold
    bool checkThreshold() const;
    
    // Get timestamp of last spike (-1 if none)
    float getLastSpikeTime() const;
    
    // ========== LIF STEP FUNCTION ==========
    
    // Integrate and fire simulation step - returns true if neuron fired
    // This implements the core LIF dynamics: V(t+dt) = V(t) * exp(-dt/tau) + I*C
    // where C = (1 - exp(-dt/tau)) / membrane_capacitance
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // ========== INPUT HANDLING ==========
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources like sensory input)
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // ========== SPIKE HISTORY ==========
    
    // Record spike for plasticity (STDP)
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // ========== SYNAPSE MANAGEMENT ==========
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // ========== PLASTICITY STATE ==========
    
    // Get plasticity flags for this neuron
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // ========== REGION/MEMBERSHIP ==========
    
    // Region/population membership (for network organization)
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // ========== SIMULATION ==========
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state (used between simulation runs)
    void reset();
    
    // Initialize with random parameters (for network creation)
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
