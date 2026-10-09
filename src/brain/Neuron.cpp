#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Create neuron with ID
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    // Initialize neuron identity and state
    pImpl->id = id;                    // Unique neuron identifier
    pImpl->type = NeuronType::Internal;  // Default to internal neuron type
    pImpl->regionId = INVALID_REGION_ID;  // Not assigned to any region yet
    pImpl->populationId = INVALID_POPULATION_ID;  // Not assigned to any population
    pImpl->totalCurrent = 0.0f;  // No external current injection
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    // Move implementation pointer, invalidate source
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    // Move assignment: clean up current impl, take ownership of other
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    // Set the neuron type classification
    // Type determines neuron's role in network (sensory, internal, motor, etc.)
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Set membrane potential directly (bypassing integration)
    // Sets absolute membrane potential value without integrating synaptic inputs
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    // Increment membrane potential by delta (mV)
    // Used for fast synaptic inputs or direct manipulation of membrane potential
    // Positive delta depolarizes (raises potential), negative hyperpolarizes
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Set firing threshold (mV). Neuron fires when membrane potential >= threshold
    // Threshold determines neuron excitability - higher threshold = less excitable
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    // Set neuron firing state directly
    // State represents current behavioral mode (resting, active, refractory)
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    // Decrement refractory state counter
    // Returns true when neuron exits refractory period
    // Allows neuron to fire again after spike-induced refractory period
    if (pImpl->refractoryState.isActive()) {
        if (pImpl->refractoryState.decrement()) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    // Set firing rate (Hz) for rate-based neurons
    // Controls output firing frequency for rate coding schemes
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    // Set membrane leak conductance (nS) for LIF dynamics
    // Controls rate of membrane potential decay toward resting potential
    // Higher conductance = faster decay, lower conductance = slower decay
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    // Get membrane leak conductance (nS)
    // Returns conductance value controlling membrane potential dynamics
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    // Set refractory period duration (in simulation steps)
    // Neuron cannot fire during this period after a spike
    // Controls minimum interval between successive spikes
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    // Get refractory period duration (steps)
    // Returns remaining steps until neuron can fire again after spike
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    // Set resting membrane potential (mV)
    // Long-term equilibrium value without inputs (typically ~ -70mV)
    // Influences neuron's baseline electrical state and excitability
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    // Get resting membrane potential (mV)
    // Returns equilibrium potential toward which membrane potential decays
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    // Set post-spike reset potential (mV)
    // Membrane potential is set to this value after firing
    // Controls post-spike hyperpolarization level
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    // Check if neuron has reached firing threshold
    // Returns true if membrane potential >= threshold, else false
    // Implements threshold detection for spike generation
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    // Get timestamp of last spike (seconds)
    // Returns -1 if neuron has never spiked
    // Used for spike-timing dependent plasticity and learning
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Receive excitatory synaptic input
    // Amplitude is synaptic conductance × reversal potential contribution (nS·mV)
    // Excitatory inputs depolarize neuron (raise membrane potential)
    // Typically positive values increase membrane potential
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Receive inhibitory synaptic input
    // Amplitude is synaptic conductance × reversal potential contribution (nS·mV)
    // Inhibitory inputs hyperpolarize neuron (lower membrane potential)
    // Typically negative values decrease membrane potential
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Receive neuromodulatory input (e.g., dopamine, acetylcholine)
    // Affects plasticity but not direct membrane potential integration
    // Modulators influence synaptic strength and learning rates
    // Used for reward prediction error and other learning signals
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Inject external current (nA) directly into neuron
    // Models sensory input or external stimulation
    // Added to synaptic input for LIF integration
    // External inputs can override synaptic dynamics
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    // Clear accumulated synaptic input and external current
    // Reset synaptic input for next time step
    // Ensures proper dynamics between integration steps
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    // Record a spike event at given timestamp
    // Updates spike history, last spike time, and enters refractory period
    // Essential for spike-timing dependent plasticity (STDP)
    pImpl->spikeHistoryManager.recordSpike(timestamp);
    pImpl->state.lastSpikeTime = static_cast<float>(timestamp);
    pImpl->state.firingState = FiringState::Active;
    pImpl->state.refractoryState.setRemainingSteps(pImpl->state.refractoryPeriod);
}

void Neuron::clearSpikeHistory() {
    // Clear all recorded spike history
    // Used for memory reset or learning reinitialization
    // Resets STDP traces and spike timing information
    pImpl->spikeHistoryManager.clearHistory();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    // Add an incoming synaptic connection
    // Synapses deliver excitatory/inhibitory input to this neuron
    // Establishes connectivity from other neurons to this neuron
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    // Add an outgoing synaptic connection
    // This neuron delivers input to other neurons via these synapses
    // Establishes connectivity from this neuron to other neurons
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    // Get mutable reference to neuron state
    // Allows direct modification of neuron internal state
    // Note: Direct access may bypass normal setters and validation
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    // Get mutable reference to plasticity flags
    // Controls which plasticity mechanisms are enabled
    // Direct access allows runtime modification of learning capabilities
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    // Enable specific plasticity mechanisms
    // hebbian: Enable Hebbian (co-activation) learning
    // stdp: Enable Spike-Timing-Dependent Plasticity
    // rewardModulated: Enable reward-modulated plasticity
    // Allows selective control of learning rules per neuron
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    // Assign neuron to a brain region
    // Regions organize neurons by functional groups
    // Influences global network topology and communication patterns
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    // Assign neuron to a population within its region
    // Populations contain similar neuron types
    // Groups neurons with similar functional characteristics
    pImpl->populationId = population;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    // Leaky Integrate-and-Fire (LIF) dynamics integration step
    // Advances neuron state by dt time using mathematical model
    // Returns true if neuron fired during this step
    // 
    // LIF model equation: dV/dt = (V_rest - V)/τ + I/C
    // Where V = membrane potential, τ = time constant, C = capacitance, I = current
    // Discretized: V_new = V + dt * ((V_rest - V)/τ + I/C)
    bool fired = false;
    
    // Handle refractory period using RefractoryState
    if (pImpl->state.refractoryState.isActive()) {
        if (pImpl->state.refractoryState.decrement()) {
            pImpl->state.firingState = FiringState::Resting;
        }
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
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
        pImpl->state.refractoryState.setRemainingSteps(pImpl->state.refractoryPeriod);
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

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    // Reset neuron to initial default state
    // Clears membrane potential, synaptic input, spike history, and refractory state
    // Restores neuron to biologically realistic initial conditions
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistoryManager.clearHistory();
    pImpl->refractoryState.reset();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Initialize neuron with biologically realistic random parameters
    // Sets membrane potential, threshold, and other parameters with species-typical variations
    // Membrane potential starts near resting potential
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical
    uint32_t refractorySteps = static_cast<uint32_t>(rng.uniformInt(2, 10));
    pImpl->state.refractoryPeriod = refractorySteps;
    pImpl->refractoryState.setRefractoryPeriod(refractorySteps);
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->refractoryState.setRemainingSteps(0);
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistoryManager.clearHistory();
}

} // namespace nlm
