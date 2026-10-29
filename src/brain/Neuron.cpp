// ==============================================================================
// NLM Brain Simulator - Neuron Implementation
// ==============================================================================
// This file implements the Leaky Integrate-and-Fire (LIF) neuron model for the
// NLM (Neural Learning Machine) brain simulation framework. Each neuron represents
// a single computational unit with biological properties including membrane
// dynamics, synaptic inputs, plasticity mechanisms, and firing behavior.
//
// Key Features:
// - LIF dynamics with biologically realistic parameters
// - Synaptic integration and spike generation
// - Plasticity support (Hebbian, STDP, reward-modulated)
// - Neuromodulation integration (dopamine, acetylcholine)
// - Refractory period management
// - Spike-frequency adaptation
// - Event-driven simulation architecture
//
// References:
// - Gerstner et al. "Neuronal Dynamics" (2014)
// - Song et al. "Systems neuroscience: Learning in integration and recall" (2000)
// - Sutton & Barto "Reinforcement Learning" (1998)
// ==============================================================================

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Private implementation details for Neuron.
// This Pimpl idiom implementation encapsulates all internal state and
// computational details of the neuron model, including membrane dynamics,
// synaptic processing, plasticity mechanisms, and firing behavior.
struct Neuron::Impl {
    // Unique identifier for this neuron in the brain network
    NeuronId id;
    
    // Type of neuron (excitatory, inhibitory, sensory, motor, modulatory, internal)
    NeuronType type;
    
    // Current dynamical state of the neuron
    NeuronState state;
    
    // Brain region and population classification
    RegionId regionId;
    PopulationId populationId;
    
    // Synaptic integration state
    MembranePotential totalCurrent;  // Total synaptic current input this step (nA)
    MembranePotential synapticInput;  // Accumulated synaptic input for integration (mV)
    
    // Historical and connectivity data
    std::vector<Timestamp> spikeHistory;      // Timestamps of recent spikes (ms)
    std::vector<SynapseHandle> incomingSynapses;  // Handles of incoming synaptic connections
    std::vector<SynapseHandle> outgoingSynapses; // Handles of outgoing synaptic connections
    PlasticityFlags plasticityFlags;           // Current plasticity mechanism states
    
    // LIF neuron model parameters (biologically realistic constants)
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;   // nF - Membrane capacitance (typical: 0.5-2nF)
    static constexpr float TIME_CONSTANT = 20.0f;       // ms - Membrane time constant τ = RC
    static constexpr size_t MAX_SPIKE_HISTORY = 100;     // Maximum number of stored spikes
    
    // Constructor initializes neuron state with biological defaults
    // Provides clean initialization with typical biological values
    Impl() : id(), 
             type(NeuronType::Internal), 
             state(), 
             regionId(INVALID_REGION_ID), 
             populationId(INVALID_POPULATION_ID),
             totalCurrent(0.0f), 
             synapticInput(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// Update neuron type (excitatory, inhibitory, sensory, motor, etc.)
// 
// Parameters:
//   type - New neuron type from NeuronType enumeration
void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

// Set absolute membrane potential
// 
// Parameters:
//   potential - Desired membrane potential in millivolts (mV)
void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
}

// Increment membrane potential by a delta value
// 
// Parameters:
//   delta - Value to add to current membrane potential (mV)
void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
}

// Set spike threshold voltage
// 
// Parameters:
//   threshold - Membrane potential at which neuron will fire (mV)
void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

// Set current firing state
// 
// Parameters:
//   state - Current firing state from FiringState enumeration
void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

// Decrement refractory period counter
// 
// If refractory period reaches zero, neuron returns to resting state.
// Note: This should be called once per simulation timestep.
void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

// Set neuron firing rate
// 
// Parameters:
//   rate - Firing rate in spikes per second (Hz)
void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

// Set leak conductance for membrane potential dynamics
// 
// Parameters:
//   conductance - Leak conductance parameter affecting membrane time constant (nS)
void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
}

// Get current leak conductance value
// 
// Returns:
//   Leak conductance in nS
MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

// Set refractory period duration
// 
// Parameters:
//   steps - Number of simulation steps neuron must remain refractory
void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
}

// Get current refractory period
// 
// Returns:
//   Number of remaining refractory steps
uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

// Set resting membrane potential
// 
// Parameters:
//   potential - Resting potential in mV (typically ~ -70mV)
void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = potential;
}

// Get resting membrane potential
// 
// Returns:
//   Resting potential in mV
MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

// Set reset membrane potential (after spike)
// 
// Parameters:
//   potential - Membrane potential to which neuron resets after firing (mV)
void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = potential;
}

// Check if membrane potential has reached spike threshold
// 
// Returns:
//   true if neuron should fire, false otherwise
bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

// Get time of last spike
// 
// Returns:
//   Timestamp of last spike in milliseconds, or -1.0 if no spike recorded
float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

// Add excitatory synaptic input to neuron
// 
// Excitatory synapses depolarize the neuron (make membrane potential more positive)
// 
// Parameters:
//   amplitude - Synaptic conductance * driving force contribution (mV)
void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

// Add inhibitory synaptic input to neuron
// 
// Inhibitory synapses hyperpolarize the neuron (make membrane potential more negative)
// 
// Parameters:
//   amplitude - Synaptic conductance * driving force contribution (mV)
void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

// Add modulatory synaptic input to neuron
// 
// Modulatory inputs affect plasticity mechanisms but are not directly integrated
// into membrane potential dynamics. Used for neuromodulation (dopamine, acetylcholine)
// 
// Parameters:
//   amplitude - Modulatory input strength affecting plasticity (arbitrary units)
void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

// Inject external current into neuron
// 
// Used for external stimulation or sensory input that directly affects membrane potential
// 
// Parameters:
//   current - External current injection (nA)
void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

// Clear accumulated synaptic input
// 
// Called at end of simulation step to prepare for next timestep.
void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

// Record a spike event in spike history
// 
// Parameters:
//   timestamp - Time at which spike occurred (ms)
void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

// Add incoming synaptic connection handle to neuron
// 
// Used for tracking connectivity and potential retrograde signaling
// 
// Parameters:
//   handle - Unique identifier for the incoming synapse
void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

// Add outgoing synaptic connection handle to neuron
// 
// Used for tracking connectivity and potential presynaptic signaling
// 
// Parameters:
//   handle - Unique identifier for the outgoing synapse
void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

// Get reference to neuron's dynamic state
// 
// Provides direct access to neuron state for simulation integration
// 
// Returns:
//   Reference to NeuronState structure containing all dynamic variables
NeuronState& Neuron::getState() {
    return pImpl->state;
}

// Get reference to neuron's plasticity flags
// 
// Provides direct access to plasticity mechanism states for external control
// 
// Returns:
//   Reference to PlasticityFlags structure containing plasticity mechanism states
PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

// Enable or disable specific plasticity mechanisms
// 
// Controls which synaptic plasticity rules are active for this neuron
// 
// Parameters:
//   hebbian - Whether Hebbian plasticity is enabled
//   stdp - Whether Spike-Timing-Dependent Plasticity is enabled
//   rewardModulated - Whether reward-modulated plasticity is enabled
void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

// Set neuron's brain region identifier
// 
// Classifies neuron within the brain's regional architecture
// 
// Parameters:
//   region - RegionId identifying the brain's region (e.g., cortex, thalamus)
void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

// Set neuron's population identifier
// 
// Classifies neuron within its brain region (e.g., pyramidal cells, interneurons)
// 
// Parameters:
//   population - PopulationId identifying the neuron's population type
void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

// Perform one Leaky Integrate-and-Fire simulation step
// 
// Core computational method that advances neuron state by a timestep
// Implements LIF dynamics with refractory period, adaptation, and spike reset
// 
// Parameters:
//   currentTime - Current simulation time in milliseconds
//   dt - Simulation timestep in seconds
// 
// Returns:
//   true if neuron fired during this timestep, false otherwise
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

// Perform standard LIF simulation step with default timestep
// 
// Convenience method that uses standard 1ms timestep for typical simulations
// 
// Parameters:
//   currentTime - Current simulation time in milliseconds
void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

// Reset neuron to initial state
// 
// Clears all dynamic state variables, preparing neuron for new simulation
// Preserves structural connectivity (incoming/outgoing synapses) and identity
void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

// Initialize neuron with biologically realistic random values
// 
// Sets initial membrane potential, threshold, and other parameters using
// random values drawn from biologically plausible distributions
// 
// Parameters:
//   rng - Random number generator for reproducible initialization
void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
