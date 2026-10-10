// Neuron.hpp - Leaky Integrate-and-Fire (LIF) Neuron Implementation
// ====================================================================
// 
// This file defines the core Neuron class for the NLM (Neural Learning Machine) project.
// The neuron implements the Leaky Integrate-and-Fire (LIF) model, which is the
// standard model for simulating biological neurons in computational neuroscience.
// 
// Key Features:
// - Membrane potential integration with leak dynamics
// - Spike detection and refractory period management
// - Synaptic input handling (excitatory, inhibitory, modulatory)
// - Spike-frequency adaptation mechanisms
// - State management for neural dynamics
// - Memory management via pImpl pattern for ABI stability
// 
// Biological Correspondence:
// - Membrane potential: Electrical charge across cell membrane
// - Resting potential (~ -70mV): Baseline electrical state
// - Threshold potential (~ -55mV): Spike initiation trigger
// - Reset potential: Post-spike membrane voltage
// - Refractory period: Absolute refractory (~2-10ms) and relative refractory periods
// - Adaptation variable: Spike-frequency adaptation (slow hyperpolarization)
// 
// Physics/Math:
// dV/dt = (V_rest - V)/tau + I/C
// Where:
//   V = membrane potential
//   V_rest = resting potential
//   tau = time constant (20ms for NLM)
//   I = total synaptic current
//   C = membrane capacitance (1.0nF for NLM)
// 
// Implementation Details:
// - Uses event-driven spike propagation for efficiency
// - Stores spike history for STDP (Spike-Timing Dependent Plasticity)
// - Manages synaptic connections through incoming/outgoing synapse lists
// - Supports plasticity flags for different learning mechanisms
// - Fully thread-safe for parallel simulation
// 
// Memory Management:
// - pImpl pattern: Neuron class uses pointer-to-implementation
// - Move semantics enabled (copy operations disabled for efficiency)
// - NeuronId serves as unique identifier (not raw pointers)
// - No memory leaks with proper RAII and destructors

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
// Contains all dynamic state variables for a single neuron
struct NeuronState {
    // Membrane potential dynamics
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    
    // Firing and timing
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    // Constructor with biologically realistic defaults
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
// Implements Leaky Integrate-and-Fire (LIF) dynamics
class Neuron {
public:
    // === Construction and Destruction ===
    
    // Create neuron with unique ID
    // Neurons are uniquely identified by their ID for connection management
    explicit Neuron(NeuronId id);
    
    // Default destructor (pImpl pattern)
    ~Neuron();
    
    // === Copy Semantics (Disabled) ===
    // Copy operations are disabled for efficiency and safety
    // Neurons cannot be copied directly - use move semantics instead
    Neuron(const Neuron&) = delete;                    // No copying
    Neuron& operator=(const Neuron&) = delete;         // No assignment
    
    // === Move Semantics (Enabled) ===
    // Move constructor transfers ownership of internal implementation
    Neuron(Neuron&&) noexcept;
    
    // Move assignment transfers ownership and cleans up source
    Neuron& operator=(Neuron&&) noexcept;
    
    // === Identity ===
    
    // Get the neuron's unique identifier
    NeuronId getId() const;
    
    // === Type Management ===
    
    // Get neuron type classification
    NeuronType getType() const;
    
    // Set neuron type (e.g., Excitatory, Inhibitory, Sensory, etc.)
    void setType(NeuronType type);
    
    // === State Access ===
    
    // Get constant reference to neuron state (read-only access)
    const NeuronState& getState() const;
    
    // Get mutable reference to neuron state (controlled modifications)
    NeuronState& getState();
    
    // === Membrane Potential Control ===
    
    // Get current membrane potential
    MembranePotential getMembranePotential() const;
    
    // Set membrane potential directly (e.g., for initialization)
    void setMembranePotential(MembranePotential potential);
    
    // Add to membrane potential (synaptic input accumulation)
    void addToMembranePotential(MembranePotential delta);
    
    // === Threshold Management ===
    
    // Get firing threshold potential
    MembranePotential getThreshold() const;
    
    // Set firing threshold
    void setThreshold(MembranePotential threshold);
    
    // === Firing State Management ===
    
    // Check if neuron is currently firing (active)
    bool isFiring() const;
    
    // Check if neuron is in refractory period
    bool isRefractory() const;
    
    // Set firing state (internal use)
    void setFiringState(FiringState state);
    
    // Set refractory period duration
    void setRefractoryPeriod(uint32_t steps);
    
    // Advance refractory period counter
    void decrementRefractory();
    
    // === Firing Rate Management ===
    
    // Get current firing rate
    FiringRate getFiringRate() const;
    
    // Set firing rate (for rate-based computation)
    void setFiringRate(FiringRate rate);
    
    // === LIF Neuron Parameters ===
    
    // Get/set leak conductance (controls membrane time constant)
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    
    // Get refractory period in steps
    uint32_t getRefractoryPeriod() const;
    
    // Set resting potential
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    
    // Set reset potential (post-spike voltage)
    void setResetPotential(MembranePotential potential);
    MembranePotential getResetPotential() const;
    
    // === Spike Detection and Recording ===
    
    // Check if membrane potential exceeds firing threshold
    bool checkThreshold() const;
    
    // Get timestamp of last spike (returns -1.0 if no spikes)
    float getLastSpikeTime() const;
    
    // === Synaptic Input Management ===
    
    // LIF step function - updates neuron for one timestep
    // Returns true if neuron fired (crossed threshold)
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // === Incoming Signals (post-synaptic potentials) ===
    
    // Receive excitatory input (depolarizing current)
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    // Receive inhibitory input (hyperpolarizing current)
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    // Receive modulatory input (affects plasticity, not integration)
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // === Current Injection (from external sources) ===
    
    // Inject direct current (e.g., from sensory input or external source)
    void injectCurrent(MembranePotential current);
    
    // Get total current (for debugging/monitoring)
    MembranePotential getTotalCurrent() const;
    
    // Clear accumulated synaptic input
    void clearTotalCurrent();
    
    // === Spike History (for STDP) ===
    
    // Record a spike event with timestamp
    void recordSpike(Timestamp timestamp);
    
    // Get recent spike history (for STDP calculations)
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    // Clear spike history
    void clearSpikeHistory();
    
    // === Synapse Management ===
    
    // Add incoming synapse connection
    void addIncomingSynapse(SynapseHandle handle);
    
    // Add outgoing synapse connection  
    void addOutgoingSynapse(SynapseHandle handle);
    
    // Get all incoming synapses
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    // Get all outgoing synapses
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // === Plasticity Management ===
    
    // Get plasticity flags (Hebbian, STDP, reward-modulated)
    const PlasticityFlags& getPlasticityFlags() const;
    
    // Get mutable plasticity flags (for enabling/disabling plasticity)
    PlasticityFlags& getPlasticityFlags();
    
    // Enable specific plasticity mechanisms
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // === Region/Population Management ===
    
    // Set brain region membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    
    // Set neuron population membership
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // === Update and Reset ===
    
    // Default LIF step with standard timestep (1ms)
    // Most simulation frameworks use this as the default update method
    void step(Timestamp currentTime);
    
    // Reset neuron to initial state
    void reset();
    
    // Initialize neuron with biologically realistic random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    // pImpl pattern for ABI stability and encapsulation
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
