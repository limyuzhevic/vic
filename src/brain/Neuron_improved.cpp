// NLM Phase 2 - Enhanced Neuron Implementation
// Improved Neuron class with better modularity, error handling, and documentation

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>

namespace nlm {

// Neuron Implementation

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    if (id == INVALID_NEURON_ID) {
        throw std::invalid_argument("Neuron ID cannot be INVALID_NEURON_ID");
    }
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    pImpl->synapticInput = 0.0f;
    NLM_LOG_DEBUG("Neuron created with ID: " + std::to_string(id.index()));
}

Neuron::~Neuron() {
    NLM_LOG_DEBUG("Neuron destroyed: " + std::to_string(pImpl->id.index()));
}

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

NeuronType Neuron::getType() const {
    return pImpl->type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Validate potential range
    if (potential < -100.0f || potential > 50.0f) {
        NLM_LOG_WARNING("Membrane potential out of range: " + std::to_string(potential));
    }
    pImpl->state.membranePotential = potential;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
    
    // Clamp to reasonable range
    pImpl->state.membranePotential = std::clamp(
        pImpl->state.membranePotential, -100.0f, 50.0f
    );
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (threshold <= 0.0f) {
        throw std::invalid_argument("Threshold must be positive");
    }
    pImpl->state.threshold = threshold;
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

FiringState Neuron::getFiringState() const {
    return pImpl->state.firingState;
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
    if (rate < 0.0f || rate > 1000.0f) {
        NLM_LOG_WARNING("Firing rate out of range: " + std::to_string(rate));
    }
    pImpl->state.firingRate = rate;
}

FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (conductance < 0.0f) {
        throw std::invalid_argument("Leak conductance must be non-negative");
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (steps > 1000) {
        NLM_LOG_WARNING("Refractory period very large: " + std::to_string(steps));
    }
    pImpl->state.refractoryPeriod = steps;
    pImpl->state.refractoryRemaining = steps; // Set remaining initially
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

MembranePotential Neuron::getResetPotential() const {
    return pImpl->state.resetPotential;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (amplitude < 0.0f) {
        throw std::invalid_argument("Excitatory input amplitude must be positive");
    }
    pImpl->synapticInput += amplitude;
    NLM_LOG_DEBUG("Excitatory input received: " + std::to_string(amplitude));
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (amplitude < 0.0f) {
        throw std::invalid_argument("Inhibitory input amplitude must be positive");
    }
    pImpl->synapticInput -= amplitude;
    NLM_LOG_DEBUG("Inhibitory input received: " + std::to_string(amplitude));
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
    if (amplitude != 0.0f) {
        NLM_LOG_DEBUG("Modulatory input received: " + std::to_string(amplitude));
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    if (current < -100.0f || current > 100.0f) {
        NLM_LOG_WARNING("Current injection out of range: " + std::to_string(current));
    }
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
    NLM_LOG_DEBUG("Spike recorded at timestamp: " + std::to_string(timestamp));
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

const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
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

RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}

NeuronId Neuron::getId() const {
    return pImpl->id;
}

bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active ||
           pImpl->state.firingState == FiringState::Refractory;
}

void Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    if (dt <= 0.0f) {
        NLM_LOG_WARNING("Invalid timestep duration: " + std::to_string(dt));
        return;
    }
    
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        // Keep the firing-rate metric decaying even while refractory
        pImpl->state.firingRate *= 0.9f;
        return;
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
    
    // Update the firing-rate metric from counted spikes, in Hz.
    if (dt > 0.0) {
        pImpl->state.firingRate = 0.9f * pImpl->state.firingRate;
    }
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        if (dt > 0.0) {
            pImpl->state.firingRate += 0.1f * static_cast<float>(1.0 / dt);
        }
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
    if (!rng.isValid()) {
        NLM_LOG_ERROR("Invalid random generator");
        return;
    }
    
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential typically -70mV (same as resting)
    pImpl->state.resetPotential = pImpl->state.restingPotential;
    
    // Leak conductance: moderate
    pImpl->state.leakConductance = 0.5f + rng.uniformReal(0.0f, 0.5f);
    
    // Refractory period: 2-10 ms
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(2.0f + rng.uniformReal(0.0f, 8.0f));
    pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
    
    // Adaptation variable: small initial value
    pImpl->state.adaptationVariable = rng.uniformReal(0.0f, 0.5f);
    
    // Firing rate: start at zero
    pImpl->state.firingRate = 0.0f;
    
    // Firing state: resting
    pImpl->state.firingState = FiringState::Resting;
    
    // Last spike time: never spiked
    pImpl->state.lastSpikeTime = -1.0f;
    
    NLM_LOG_INFO("Neuron " + std::to_string(pImpl->id.index()) + " initialized randomly");
}

} // namespace nlm
