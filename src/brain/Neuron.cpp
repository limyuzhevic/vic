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
    // Fast path for non-full history
    if (pImpl->spikeHistory.size() < Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.push_back(timestamp);
    } else {
        // Circular buffer implementation for fixed-size history
        // Rotate the history by removing first element and adding new one
        std::rotate(pImpl->spikeHistory.begin(), pImpl->spikeHistory.begin() + 1, pImpl->spikeHistory.end());
        pImpl->spikeHistory.back() = timestamp;
    }
}

void Neuron::clearSpikeHistory() {
    // Optimized clear using shrink_to_fit if available
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    // Reserve space for better performance if growing
    if (pImpl->incomingSynapses.size() >= pImpl->incomingSynapses.capacity()) {
        pImpl->incomingSynapses.reserve(pImpl->incomingSynapses.size() * 1.5);
    }
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    // Reserve space for better performance if growing
    if (pImpl->outgoingSynapses.size() >= pImpl->outgoingSynapses.capacity()) {
        pImpl->outgoingSynapses.reserve(pImpl->outgoingSynapses.size() * 1.5);
    }
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
    
    // Handle refractory period - early exit for common case
    if (pImpl->state.refractoryRemaining == 0) {
        // LIF dynamics: Leaky Integrate-and-Fire
        // dV/dt = (V_rest - V)/tau + I/C
        // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
        
        MembranePotential& V = pImpl->state.membranePotential;
        MembranePotential V_rest = pImpl->state.restingPotential;
        MembranePotential V_reset = pImpl->state.resetPotential;
        MembranePotential threshold = pImpl->state.threshold;
        
        // Pre-calculate constants for better performance
        static constexpr float inv_tau = 1.0f / Impl::TIME_CONSTANT;  // 1/tau
        static constexpr float inv_C = 1.0f / Impl::MEMBRANE_CAPACITANCE;  // 1/C
        static constexpr float dt_ms = 1000.0f * dt;  // Convert to milliseconds
        
        // Synaptic input contributes to membrane potential change
        float synapticContribution = pImpl->synapticInput * inv_C;
        
        // Leak contribution - optimized using distributivity
        float leakContribution = inv_tau * (V_rest - V);
        
        // Update membrane potential using exponential Euler integration
        V += dt_ms * (leakContribution + synapticContribution);
        
        // Apply spike-frequency adaptation (slow hyperpolarization after spike)
        if (pImpl->state.adaptationVariable > 0.0f) {
            V -= pImpl->state.adaptationVariable * 0.01f;
            pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
        }
        
        // Check for spike - optimize threshold comparison
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
        
        // Clamp membrane potential to prevent instability - optimize
        if (V > 50.0f) V = 50.0f;
        else if (V < -100.0f) V = -100.0f;
        
        // Clear synaptic input for next step
        pImpl->synapticInput = 0.0f;
        
        return fired;
    }
    
    // Refractory period handling
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    return false;
}

void Neuron::step(Timestamp currentTime) {
    // Fast path for default timestep - avoid multiplication in common case
    static constexpr TimestepDuration default_dt = 0.001;  // 1ms default
    stepLIF(currentTime, default_dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Optimized random initialization with biological constraints
    // Generate all random values first to minimize RNG calls
    
    float membraneOffset = rng.uniformReal(-3.0f, 3.0f);
    float thresholdOffset = rng.uniformReal(-2.0f, 2.0f);
    float restingOffset = rng.uniformReal(-2.0f, 2.0f);
    float resetOffset = rng.uniformReal(0.0f, 5.0f);
    uint32_t refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Set resting potential first (many values depend on it)
    pImpl->state.restingPotential = -70.0f + restingOffset;
    
    // Initialize membrane potential based on resting potential
    pImpl->state.membranePotential = pImpl->state.restingPotential + membraneOffset;
    
    // Set threshold
    pImpl->state.threshold = -55.0f + thresholdOffset;
    
    // Set reset potential relative to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + resetOffset;
    
    // Set refractory period
    pImpl->state.refractoryPeriod = refractoryPeriod;
    
    // Set initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear spike history and synaptic input
    clearSpikeHistory();
    pImpl->synapticInput = 0.0f;
}

} // namespace nlm
