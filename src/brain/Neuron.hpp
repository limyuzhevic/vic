#pragma once

// Neuron.hpp - NLM Neuron Header File
//
// Copyright: 2020-2026 NLM Research Team
// License: MIT License
//
// Description:
// This header file defines the Neuron class representing a single neuron in the NLM brain.
// The Neuron class implements Leaky Integrate-and-Fire (LIF) dynamics for spiking neural computation.
// LIF neurons are the fundamental building blocks of the NLM brain system, providing realistic
// biological dynamics for neural signal processing and learning.
//
// Neuron Model:
// ============
// The neuron implements the classic LIF model with enhanced biological features:
// - Membrane potential integrates incoming currents with exponential decay
// - When threshold is reached, neuron fires a spike and enters refractory period
// - Synaptic inputs are integrated continuously with precise timing
// - Spike-frequency adaptation prevents unrealistic firing rates
// - Refractory period prevents immediate re-firing after spike
//
// Key Biological Features:
// ====================
// - Membrane potential dynamics: dV/dt = (V_rest - V)/τ + I/C
// - Spike-frequency adaptation for realistic firing patterns
// - Refractory period modeling (9 steps per Phase 3)
// - Synaptic integration with conductance-based currents
// - Spike history recording for STDP learning
// - Type-specific behavior (sensory, motor, internal, inhibitory)
//
// Integration Architecture:
// ===================
// The neuron integrates with the broader brain system through:
// - Synaptic connections with other neurons (incoming/outgoing)
// - Signal integration from sensory, motor, and modulatory inputs
// - Plasticity mechanisms (STDP, Hebbian, reward-modulated)
// - Working memory storage for active traces
// - Neuromodulation effects on excitability
//
// LIF Dynamics Implementation:
// =======================
// stepLIF() implements the discrete-time version of:
// V_new = V + dt * 1000 * ((V_rest - V)/τ + I/C)
// where τ = 20ms (membrane time constant) and C = 1nF (capacitance)
//
// Electrical properties:
// - Resting potential: -70mV (default)
// - Firing threshold: -55mV (default)
// - Reset potential: -70mV (default)
// - Membrane time constant: 20ms
// - Refractory period: 9 steps (≈9ms at 1ms timestep)
// - Leak conductance: 10 nS (default)
//
// Performance Considerations:
// ========================
// - Neurons are stateless during simulation; only state changes via LIF step
// - Spike history maintains recent spikes for plasticity rules
// - Refractory period prevents runaway excitation
// - Membrane potential clamping prevents numerical instability
// - Efficient spike delivery via pre-built neuronById lookup
//
// Neuron Types:
// ============
// - SENSORY: Receive external input, relay to internal network
// - MOTOR: Generate action outputs, controlled by internal network
// - INTERNAL: Core computation, maintain working memory traces
// - INHIBITORY: Provide lateral inhibition, balance excitation
// - MODULATORY: Fine-tune plasticity and excitability
//
// Learning Integration:
// ============
// Neurons participate in multiple learning mechanisms:
// - STDP: Spike timing determines synaptic weight changes
// - Hebbian: Correlated pre/post activity strengthens connections
// - Reward-modulated: Neuromodulators scale plasticity based on outcomes
// - Structural: Synaptogenesis and pruning change connectivity
// - Development: Plasticity rates change with neural age
//
// Files in this module:
// ===================
// Neuron.cpp - LIF dynamics implementation
// Synapse.cpp - Synaptic connectivity and plasticity
// NeuralRegion.cpp - Region organization and management
// Brain.cpp - Brain initialization and coordination
// pynlm bindings - Python interface
// tests - Unit testing and validation

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

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
