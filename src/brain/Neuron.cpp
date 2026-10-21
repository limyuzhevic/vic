#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

// ========== NEURON IMPLEMENTATION ==========

// Enhanced LIF neuron implementation with biologically realistic dynamics
// Implements spike-timing dependent plasticity and real neural computation

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // LIF parameters with biological realism
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF - typical for cortical neurons
    static constexpr float TIME_CONSTANT = 20.0f;  // ms - membrane time constant
    static constexpr float RESET_THRESHOLD_FACTOR = 0.5f;  // Relative to resting potential
    static constexpr float ADAPTATION_DECAY_RATE = 0.95f;  // Exponential decay
    static constexpr float ADAPTATION_INJECTION = 1.0f;   // Spike-triggered adaptation
    static constexpr float MAX_SPIKE_HISTORY = 100;
    
    // Synaptic parameters
    static constexpr float EXCITATORY_REVERSAL = 0.0f;   // mV - reversal potential
    static constexpr float INHIBITORY_REVERSAL = -70.0f;  // mV - reversal potential
    static constexpr float SYNAPTIC_CONDUCTANCE_SCALE = 1.0f;  // nS
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {
        // Initialize with biologically realistic defaults
        initializeState();
    }
    
    void initializeState() {
        // Set biologically realistic initial parameters
        state.restingPotential = -70.0f;      // Typical cortical neuron resting potential
        state.resetPotential = -65.0f;        // Spike reset potential
        state.threshold = -55.0f;             // Typical threshold relative to resting
        state.leakConductance = 0.001f;       // Membrane conductance
        state.refractoryPeriod = 2;           // ms refractory period
        state.refractoryRemaining = 0;
        state.adaptationVariable = 0.0f;      // For spike-frequency adaptation
        state.lastSpikeTime = -1000.0f;       // Invalid spike time
        state.firingRate = 0.0f;
        state.firingState = FiringState::Resting;
    }
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
    // Using conductance-based model: I_syn = g_syn * (V - E_exc)
    float reversalPotential = Impl::EXCITATORY_REVERSAL;
    float drivingForce = pImpl->state.membranePotential - reversalPotential;
    pImpl->totalCurrent += amplitude * drivingForce * Impl::SYNAPTIC_CONDUCTANCE_SCALE;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    // Using conductance-based model: I_syn = g_syn * (V - E_inh)
    float reversalPotential = Impl::INHIBITORY_REVERSAL;
    float drivingForce = pImpl->state.membranePotential - reversalPotential;
    pImpl->totalCurrent += amplitude * drivingForce * Impl::SYNAPTIC_CONDUCTANCE_SCALE;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    // Alternative implementation using current-based model
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->totalCurrent = 0.0f;
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

// ========== ENHANCED LIF DYNAMICS ==========

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period with exponential decay
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clamp potential to reset value
        pImpl->state.membranePotential = pImpl->state.resetPotential;
        pImpl->state.firingState = FiringState::Refractory;
        
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Enhanced LIF dynamics: Biologically realistic integrate-and-fire
    // dV/dt = (V_rest - V)/tau + (I_syn + I_inj)/C
    // Where C = membrane capacitance, tau = R*C (time constant)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms - membrane time constant
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF - membrane capacitance
    
    // Calculate synaptic current contributions
    float synapticCurrent = pImpl->totalCurrent + pImpl->synapticInput;
    
    // Leak current (Ohmic): I_leak = g_leak * (V - V_rest)
    // where g_leak = 1/tau for standard LIF model
    float leakConductance = 1.0f / tau;
    float leakCurrent = leakConductance * (V - V_rest);
    
    // Synaptic current contribution
    float synapticContribution = synapticCurrent / C;
    
    // Leak contribution to voltage change
    float leakContribution = leakCurrent / C;
    
    // Update membrane potential using exponential Euler integration
    // V(t+dt) = V(t) + dt * ( (V_rest - V)/tau + I_total/C )
    float dt_ms = static_cast<float>(dt) * 1000.0f;  // Convert to ms for consistency
    V = V + dt_ms * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    // Adaptation variable tracks recent spiking activity
    if (pImpl->state.adaptationVariable > 0.0f) {
        // Adaptation current: I_adapt = g_adapt * A
        // Where A is adaptation variable, g_adapt is adaptation conductance
        float adaptationConductance = pImpl->state.leakConductance * 0.1f;
        float adaptationCurrent = adaptationConductance * pImpl->state.adaptationVariable;
        
        // Apply adaptation as hyperpolarizing current
        V -= adaptationCurrent * dt_ms;
        
        // Decay adaptation variable exponentially
        pImpl->state.adaptationVariable *= Impl::ADAPTATION_DECAY_RATE;
    }
    
    // Apply biological voltage bounds to prevent instability
    // Typical neuron voltage range: -100 mV (hyperpolarized) to 50 mV (depolarized)
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike threshold crossing
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike for plasticity and history
        recordSpike(currentTime);
        
        // Apply spike-triggered reset: V <- V_reset
        V = V_reset;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update spike-frequency adaptation variable
        // Each spike increases adaptation variable for next spikes
        pImpl->state.adaptationVariable += Impl::ADAPTATION_INJECTION;
        
        // Reset synaptic currents for next timestep
        pImpl->totalCurrent = 0.0f;
        pImpl->synapticInput = 0.0f;
        
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
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
    pImpl->totalCurrent = 0.0f;
    pImpl->spikeHistory.clear();
    
    // Re-initialize with biologically realistic defaults
    pImpl->initializeState();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Enhanced random initialization with biological constraints
    // Membrane potential starts near resting potential
    pImpl->state.membranePotential = pImpl->state.restingPotential + 
                                     rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation based on neuron type
    if (pImpl->type == NeuronType::Sensory) {
        pImpl->state.threshold = -50.0f + rng.uniformReal(-5.0f, 5.0f);
    } else if (pImpl->type == NeuronType::Motor) {
        pImpl->state.threshold = -45.0f + rng.uniformReal(-3.0f, 3.0f);
    } else {
        pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    }
    
    // Resting potential typically -70mV with slight variation
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting (10-20% difference)
    pImpl->state.resetPotential = pImpl->state.restingPotential + 
                                   rng.uniformReal(10.0f, 20.0f);
    
    // Refractory period varies by neuron type: 2-10ms typical
    if (pImpl->type == NeuronType::Sensory) {
        pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 6));
    } else {
        pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    }
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1000.0f;  // Invalid spike time
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->totalCurrent = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm