#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Config/Config.hpp"
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
    
    // LIF parameters - CONFIGURABLE
    Impl(std::shared_ptr<Config> config) : id(), type(NeuronType::Internal), regionId(), populationId(),
                                          totalCurrent(0.0f), synapticInput(0.0f), config(config) {}
    
    std::shared_ptr<Config> config;
};

Neuron::Neuron(NeuronId id, std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
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
    if (!pImpl) return;
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    if (!pImpl) return;
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    if (!pImpl) return;
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (!pImpl) return;
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    if (!pImpl) return;
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    if (!pImpl) return;
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    if (!pImpl) return;
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (!pImpl) return;
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl ? pImpl->state.leakConductance : 0.0f;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (!pImpl) return;
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl ? pImpl->state.refractoryPeriod : 0;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (!pImpl) return;
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl ? pImpl->state.restingPotential : -70.0f;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (!pImpl) return;
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    if (!pImpl) return false;
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl ? pImpl->state.lastSpikeTime : -1.0f;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (!pImpl) return;
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (!pImpl) return;
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    if (!pImpl) return;
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    if (!pImpl) return;
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    if (!pImpl) return;
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (!pImpl) return;
    pImpl->spikeHistory.push_back(timestamp);
    size_t maxSpikeHistory = pImpl->config->getOr<size_t>(Config::NEURON_MAX_SPIKE_HISTORY, 100);
    if (pImpl->spikeHistory.size() > maxSpikeHistory) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    if (!pImpl) return;
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (!pImpl) return;
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (!pImpl) return;
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    static NeuronState emptyState;
    return pImpl ? pImpl->state : emptyState;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    static PlasticityFlags emptyFlags;
    return pImpl ? pImpl->plasticityFlags : emptyFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (!pImpl) return;
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    if (!pImpl) return;
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    if (!pImpl) return;
    pImpl->populationId = population;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    // Check for null/empty neuron
    if (!pImpl) {
        return false;
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
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    
    // Get configurable parameters
    float tau = pImpl->config->getOr<float>(Config::NEURON_TIME_CONSTANT, 20.0f);  // ms
    float C = pImpl->config->getOr<float>(Config::NEURON_MEMBRANE_CAPACITANCE, 1.0f);  // nF
    float maxMembranePotential = pImpl->config->getOr<float>(Config::NEURON_MAX_MEMBRANE_POTENTIAL, 1000.0f);
    float minMembranePotential = pImpl->config->getOr<float>(Config::NEURON_MIN_MEMBRANE_POTENTIAL, -100.0f);
    float adaptationDecay = pImpl->config->getOr<float>(Config::NEURON_ADAPTATION_DECAY, 0.01f);
    float adaptationFactorDecay = pImpl->config->getOr<float>(Config::NEURON_ADAPTATION_FACTOR_DECAY, 0.95f);
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * adaptationDecay;
        pImpl->state.adaptationVariable *= adaptationFactorDecay;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, minMembranePotential, maxMembranePotential);
    
    
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
        pImpl->state.adaptationVariable += pImpl->config->getOr<float>(Config::NEURON_SPIKE_ADAPTATION_INCREMENT, 1.0f);
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
    if (!pImpl) return;
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    if (!pImpl) return;
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
