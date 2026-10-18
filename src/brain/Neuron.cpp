#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Neuron::Impl {
    // ========== NEURON IDENTITY ==========
    NeuronId id;                           // Unique identifier for this neuron
    NeuronType type;                      // Neuron functional type (Excitatory, Inhibitory, etc.)
    NeuronState state;                    // Complete neuron state for LIF dynamics
    RegionId regionId;                    // Parent brain region identifier
    PopulationId populationId;            // Population identifier within region
    
    // ========== SYNAPTIC INPUT ==========
    MembranePotential totalCurrent;       // Total synaptic current input this step (mV)
    MembranePotential synapticInput;      // Accumulated synaptic input (mV) for LIF integration
    
    // ========== SPIKE HISTORY ==========
    std::vector<Timestamp> spikeHistory;  // Recent spike timestamps for plasticity (STDP)
    std::vector<SynapseHandle> incomingSynapses;  // Handles of incoming connections
    std::vector<SynapseHandle> outgoingSynapses; // Handles of outgoing connections
    PlasticityFlags plasticityFlags;      // STDP, Hebbian, and reward-modulated plasticity flags
    
    // ========== LIF PARAMETERS ==========
    // Leaky Integrate-and-Fire neuron model parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // Membrane capacitance (nF)
    static constexpr float TIME_CONSTANT = 20.0f;      // Membrane time constant (ms)
    static constexpr size_t MAX_SPIKE_HISTORY = 100;   // Maximum spike history entries
    
    // ========== INITIALIZATION ==========
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
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

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

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
    // Core equation: dV/dt = (V_rest - V)/τ + I/C
    // Where:
    // - V: Membrane potential (mV)
    // - V_rest: Resting potential (typically -70 mV)
    // - τ: Membrane time constant (typically 20 ms)
    // - I: Total synaptic current (nA)
    // - C: Membrane capacitance (nF)
    
    // Discrete approximation using exponential Euler integration:
    // V_new = V + dt * ((V_rest - V)/τ + I/C)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    // Convert synaptic current to voltage change using membrane capacitance
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution (passive membrane properties)
    // Current proportional to difference between current V and V_rest
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    // dt is converted from ms to seconds (1000 factor) for correct units
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    // Adaptation variable represents calcium-dependent potassium currents
    if (pImpl->state.adaptationVariable > 0.0f) {
        // Each unit of adaptation variable subtracts 0.01 mV from membrane potential
        V -= pImpl->state.adaptationVariable * 0.01f;
        // Adaptation decays exponentially with time constant of 20 ms
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    // Biologically plausible limits prevent runaway excitation
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike threshold crossing
    // Neuron fires when membrane potential reaches or exceeds threshold
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike for plasticity and statistics
        recordSpike(currentTime);
        
        // Reset membrane potential to reset potential (typically near resting)
        V = V_reset;
        
        // Enter refractory period to prevent immediate re-firing
        // Refractory period mimics absolute refractory period in biological neurons
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        // Each spike increases adaptation by 1.0 unit
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        // Neuron is in active state but hasn't crossed threshold
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    // Prevents double-counting of synaptic contributions
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Biological initialization with realistic parameter ranges
    // Membrane potential starts near resting potential (-70 mV)
    // ±3 mV variation represents biological noise and initial conditions
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold typically -55 mV (±2 mV variation)
    // Represents the voltage at which voltage-gated sodium channels activate
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70 mV (±2 mV variation)
    // Represents the equilibrium potential for potassium ions
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential usually close to resting (0-5 mV above resting)
    // Post-spike reset typically moves membrane potential toward resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10 ms typical range
    // Absolute refractory period prevents immediate re-firing after spike
    // Duration depends on sodium channel inactivation recovery
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state values
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;  // No adaptation initially
    pImpl->state.lastSpikeTime = -1.0f;  // No previous spikes
    
    // Clear any residual state from previous use
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
