// Enhanced Neuron implementation with improved error handling and memory management
#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>

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
             totalCurrent(0.0f), synapticInput(0.0f) {
        // Initialize state with default values
        state = NeuronState();
        state.refractoryRemaining = 0;
        state.lastSpikeTime = -1.0f;
    }
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl()) {
    if (!pImpl) {
        throw std::runtime_error("Failed to allocate neuron memory");
    }
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    // Note: synapticInput is already initialized in Impl constructor
}

Neuron::~Neuron() {
    delete pImpl;
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
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    if (!pImpl) {
        return; // Or throw exception
    }
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    if (!pImpl) {
        return false;
    }
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    if (!pImpl) {
        return -1.0f;
    }
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (!pImpl) {
        return;
    }
    // Validate input amplitude
    if (std::isnan(amplitude) || std::isinf(amplitude)) {
        return; // Ignore invalid input
    }
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (!pImpl) {
        return;
    }
    // Validate input amplitude
    if (std::isnan(amplitude) || std::isinf(amplitude)) {
        return; // Ignore invalid input
    }
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    if (!pImpl) {
        return;
    }
    // Validate input amplitude
    if (std::isnan(amplitude) || std::isinf(amplitude)) {
        return; // Ignore invalid input
    }
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    if (!pImpl) {
        return;
    }
    // Validate input current
    if (std::isnan(current) || std::isinf(current)) {
        return; // Ignore invalid input
    }
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    if (!pImpl) {
        return;
    }
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (!pImpl) {
        return;
    }
    // Validate timestamp
    if (std::isnan(timestamp) || std::isinf(timestamp)) {
        return;
    }
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    if (!pImpl) {
        return;
    }
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (!pImpl) {
        return;
    }
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (!pImpl) {
        return;
    }
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    if (!pImpl) {
        throw std::runtime_error("Neuron implementation is null");
    }
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (!pImpl) {
        return;
    }
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    if (!pImpl) {
        return;
    }
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    if (!pImpl) {
        return;
    }
    pImpl->populationId = population;
}

NeuronId Neuron::getId() const {
    if (!pImpl) {
        return NeuronId();
    }
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    if (!pImpl) {
        return NeuronType::Internal;
    }
    return pImpl->type;
}

const NeuronState& Neuron::getState() const {
    if (!pImpl) {
        static const NeuronState emptyState;
        return emptyState;
    }
    return pImpl->state;
}

MembranePotential Neuron::getMembranePotential() const {
    if (!pImpl) {
        return 0.0f;
    }
    return pImpl->state.membranePotential;
}

MembranePotential Neuron::getThreshold() const {
    if (!pImpl) {
        return 0.0f;
    }
    return pImpl->state.threshold;
}

bool Neuron::isFiring() const {
    if (!pImpl) {
        return false;
    }
    return pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    if (!pImpl) {
        return false;
    }
    return pImpl->state.refractoryRemaining > 0;
}

FiringRate Neuron::getFiringRate() const {
    if (!pImpl) {
        return 0.0f;
    }
    return pImpl->state.firingRate;
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    if (!pImpl) {
        static const std::vector<Timestamp> emptyVector;
        return emptyVector;
    }
    return pImpl->spikeHistory;
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    if (!pImpl) {
        static const std::vector<SynapseHandle> emptyVector;
        return emptyVector;
    }
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    if (!pImpl) {
        static const std::vector<SynapseHandle> emptyVector;
        return emptyVector;
    }
    return pImpl->outgoingSynapses;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    if (!pImpl) {
        static const PlasticityFlags emptyFlags;
        return emptyFlags;
    }
    return pImpl->plasticityFlags;
}

RegionId Neuron::getRegionId() const {
    if (!pImpl) {
        return INVALID_REGION_ID;
    }
    return pImpl->regionId;
}

PopulationId Neuron::getPopulationId() const {
    if (!pImpl) {
        return INVALID_POPULATION_ID;
    }
    return pImpl->populationId;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
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

void Neuron::step(Timestamp currentTime) {
    if (!pImpl) {
        return;
    }
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    if (!pImpl) {
        return;
    }
    pImpl->state = NeuronState();
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.lastSpikeTime = -1.0f;
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->incomingSynapses.clear();
    pImpl->outgoingSynapses.clear();
    pImpl->plasticityFlags = PlasticityFlags();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    if (!pImpl) {
        return;
    }
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