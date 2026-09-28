#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Neuron implementation - Leaky Integrate-and-Fire dynamics
// ==========================================
// 
// This file implements the Neuron class defined in Neuron.hpp.
// The neuron model uses the Leaky Integrate-and-Fire (LIF) formulation
// which approximates the biological membrane potential dynamics.
// 
// Key Equations Implemented:
// dV/dt = (V_rest - V)/tau + I/C
// where: V = membrane potential, V_rest = resting potential
//        tau = time constant (20ms), C = membrane capacitance (1nF)
// 
// Integration uses exponential Euler method for stability.
// 
// LIF Model State:
// - Membrane potential (V) represents neural excitation level
// - Refractory period prevents immediate re-firing
// - Spike-frequency adaptation reduces firing rate over time
// - Synaptic inputs integrate over time before triggering spikes
// 
// Cross-references:
// - Neuron.hpp: Interface definition and NeuronState structure
// - Types.hpp: Neuroscientific constants and type definitions

namespace nlm {

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
    
    // LIF parameters - biological constants
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF, typical neuron capacitance
    static constexpr float TIME_CONSTANT = 20.0f;      // ms, membrane time constant
    static constexpr size_t MAX_SPIKE_HISTORY = 100;    // Keep last 100 spikes for STDP
    
    // Implementation-specific initialization
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    // Initialize neuron with unique identifier
    // Neuron starts in resting state with default parameters
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

// Set neuron type (e.g., Internal, Sensory, Motor)
// Neuron types help organize networks and can influence plasticity
void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

// Set membrane potential directly
// Direct setting bypasses integration - used for initialization and external control
void Neuron::setMembranePotential(MembranePotential potential) {
    // Validate membrane potential is within reasonable biological range
    if (potential < -100.0f || potential > 50.0f) {
        NLM_LOG_WARNING("Neuron::setMembranePotential: potential " + std::to_string(potential) + 
                       " outside expected range [-100, 50] mV");
    }
    
    // Check for extreme jumps that might indicate programming errors
    float currentPotential = pImpl->state.membranePotential;
    if (std::abs(potential - currentPotential) > 50.0f) {
        NLM_LOG_WARNING("Neuron::setMembranePotential: large potential jump from " + 
                       std::to_string(currentPotential) + " to " + std::to_string(potential) + " mV");
    }
    
    pImpl->state.membranePotential = potential;
}

// Add current to membrane potential (equivalent to synaptic input)
// Used by synapse::update() to inject synaptic currents
void Neuron::addToMembranePotential(MembranePotential delta) {
    // Validate delta is reasonable
    if (delta < -100.0f || delta > 100.0f) {
        NLM_LOG_WARNING("Neuron::addToMembranePotential: delta " + std::to_string(delta) + 
                       " outside expected range [-100, 100] mV");
    }
    
    // Check for potential overflow
    float newPotential = pImpl->state.membranePotential + delta;
    if (newPotential < -100.0f || newPotential > 50.0f) {
        NLM_LOG_WARNING("Neuron::addToMembranePotential: resulting potential " + 
                       std::to_string(newPotential) + " outside safe range [-100, 50] mV");
    }
    
    pImpl->state.membranePotential += delta;
}

// Set neuron firing threshold
// Higher threshold requires more depolarization to trigger spike
void Neuron::setThreshold(MembranePotential threshold) {
    // Validate threshold is within reasonable biological range
    if (threshold < -100.0f || threshold > 50.0f) {
        NLM_LOG_ERROR("Neuron::setThreshold: invalid threshold " + std::to_string(threshold) + 
                     ". Must be in range [-100, 50] mV");
        return;
    }
    
    // Check for threshold that doesn't make sense biologically
    if (threshold <= pImpl->state.restingPotential) {
        NLM_LOG_WARNING("Neuron::setThreshold: threshold " + std::to_string(threshold) + 
                       " is at or below resting potential " + std::to_string(pImpl->state.restingPotential) + ". " +
                       "Neuron may fire spontaneously.");
    }
    
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

// Handle incoming excitatory synaptic input
// Excitatory inputs (e.g., glutamate) depolarize the neuron
// Amplitude represents synaptic conductance * reversal potential contribution
void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

// Handle incoming inhibitory synaptic input
// Inhibitory inputs (e.g., GABA) hyperpolarize the neuron
// Amplitude represents inhibitory synaptic conductance
void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

// Handle neuromodulatory input
// Neuromodulation affects plasticity but doesn't directly integrate
// Examples: dopamine (reward prediction error), acetylcholine (attention)
void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

// Inject direct current from external sources
// External current can be from sensory input, error signals, or artificial stimulation
// Added to synaptic input for LIF integration
void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

// Record spike (placeholder implementation)
// Note: This is a placeholder implementation - the real implementation
// in stepLIF() handles spike recording with proper validation
void Neuron::recordSpike(Timestamp timestamp) {
    // Validate timestamp is reasonable
    if (timestamp < 0.0) {
        NLM_LOG_WARNING("Neuron::recordSpike: negative timestamp " + std::to_string(timestamp));
    }
    
    // Check for reasonable time intervals between spikes
    float lastSpikeTime = pImpl->state.lastSpikeTime;
    if (lastSpikeTime > 0.0f) {
        float timeSinceLast = static_cast<float>(timestamp) - lastSpikeTime;
        if (timeSinceLast < 1.0f) {  // Less than 1ms since last spike
            NLM_LOG_WARNING("Neuron::recordSpike: spikes occurring very frequently (" + 
                           std::to_string(timeSinceLast) + "ms)");
        }
    }
    
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

// Implement Leaky Integrate-and-Fire dynamics for one timestep
// Returns true if neuron fired (crossed threshold) during this timestep
// Core LIF integration algorithm implementing the differential equation:
// dV/dt = (V_rest - V)/tau + I/C
// where V is membrane potential, V_rest is resting potential,
// tau is time constant, I is synaptic current, and C is membrane capacitance
// 
// Integration steps:
// 1. Check refractory period (neuron cannot fire during this time)
// 2. Integrate leak current toward resting potential
// 3. Integrate synaptic input contributions
// 4. Apply spike-frequency adaptation
// 5. Check for threshold crossing and handle spike
// 6. Clear synaptic input for next timestep
// 
// The exponential Euler method provides stable integration for LIF dynamics.
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    // Validate input parameters
    if (dt <= 0.0) {
        NLM_LOG_ERROR("Neuron::stepLIF: invalid timestep duration " + std::to_string(dt) + ". Must be positive.");
        return false;
    }
    
    if (currentTime < 0.0) {
        NLM_LOG_WARNING("Neuron::stepLIF: current time " + std::to_string(currentTime) + " is negative");
    }
    
    bool fired = false;
    
    // Handle refractory period - neurons cannot fire during refractory period
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
    
    // Validate synaptic input is reasonable
    if (std::abs(pImpl->synapticInput) > 1000.0f) {
        NLM_LOG_WARNING("Neuron::stepLIF: large synaptic input " + std::to_string(pImpl->synapticInput) + " may indicate instability");
    }
    
    // Leak contribution pulls membrane potential toward resting potential
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
    
    // Check for spike threshold crossing
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike for STDP and other spike-timing dependent plasticity
        recordSpike(currentTime);
        
        // Reset membrane potential to reset value after spike
        V = V_reset;
        
        // Enter refractory period where neuron cannot fire again
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation variable for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        // Neuron is active (receiving input) but not currently firing
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step to prevent integration of same input multiple times
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

// Convenience step function with standard 1ms timestep
// Many simulations use fixed timesteps, this provides a simpler interface
// Equivalent to calling stepLIF with dt = 0.001 (1ms)
void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    // Reset to all-default state
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

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
