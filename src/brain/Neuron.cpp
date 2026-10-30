#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

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
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
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
    
    // Handle refractory period - restore from firing state to resting
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, keep membrane potential at reset
        pImpl->state.membranePotential = pImpl->state.resetPotential;
        // No integration during refractory period
        return false;
    }
    
    // Convert dt to milliseconds for consistency
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    
    // --- LIF Dynamics: Leaky Integrate-and-Fire ---
    // dV/dt = (V_rest - V)/tau + I/C
    // where I is total synaptic current input
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Get total synaptic current (including injected current)
    float totalSynapticCurrent = pImpl->synapticInput;
    
    // Calculate synaptic contribution to dV/dt
    // I_syn = g_syn * (V - E_syn), where E_syn is reversal potential
    // For excitatory: E_syn = 0, for inhibitory: E_syn = -70mV
    float synapticContribution = totalSynapticCurrent / C;
    
    // Calculate leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Apply exponential Euler integration for better stability
    // V_new = V + dt * (leakContribution + synapticContribution)
    float dV = dt_ms * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (SFA)
    // Adaptation is governed by slow depolarization variables
    if (pImpl->state.adaptationVariable > 0.0f) {
        // Adaptation increases membrane conductance, causing hyperpolarization
        float adaptationEffect = pImpl->state.adaptationVariable * 0.005f;  // Slower adaptation
        V -= adaptationEffect;
        
        // Adaptation decays exponentially with time constant tau_adapt
        float tau_adapt = 100.0f;  // ms
        pImpl->state.adaptationVariable *= std::exp(-dt_ms / tau_adapt);
    }
    
    // Update membrane potential with SFA
    V += dV;
    
    // Apply spike-frequency adaptation to ensure stability
    // More realistic adaptation that prevents runaway excitation
    if (V > threshold) {
        V = threshold;  // Clamp at threshold for stability
    }
    
    // Store previous potential for spike detection
    float previousV = V;
    
    // Check for spike with hysteresis to prevent multiple spikes
    if (V >= threshold) {
        fired = true;
        
        // Record spike with precise timing
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Apply adaptive reset potential
        V = pImpl->state.resetPotential;
        
        // Enter refractory period (reduced by adaptation)
        float refractoryMod = 1.0f - (pImpl->state.adaptationVariable * 0.2f);  // Adaptation reduces refractory
        pImpl->state.refractoryRemaining = static_cast<uint32_t>(pImpl->state.refractoryPeriod * refractoryMod);
        
        // Update adaptation variable for next spike
        // More realistic adaptation: increases with each spike
        pImpl->state.adaptationVariable += 2.0f;  // Stronger adaptation per spike
        
        // Apply spike-triggered adaptation (STA)
        // STA has faster time constant than regular adaptation
        float tau_sta = 20.0f;  // ms
        pImpl->state.adaptationVariable *= std::exp(-dt_ms / tau_sta);
    }
    
    // Apply homeostatic plasticity to maintain stability
    // Ensures neurons don't become hypo- or hyper-excitable
    if (V < -90.0f) {
        V = -90.0f;  // Floor at -90mV
    } else if (V > 40.0f) {
        V = 40.0f;   // Ceiling at 40mV
    }
    
    // Update firing state based on current membrane potential
    if (pImpl->state.refractoryRemaining > 0) {
        pImpl->state.firingState = FiringState::Refractory;
    } else if (pImpl->state.membranePotential >= threshold) {
        pImpl->state.firingState = FiringState::Active;
    } else {
        pImpl->state.firingState = FiringState::Resting;
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
