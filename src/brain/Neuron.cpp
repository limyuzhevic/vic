#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <numeric>

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
    // Validate neuron ID
    if (id == INVALID_NEURON_ID) {
        NLM_LOG_ERROR("Neuron: Cannot create neuron with invalid ID");
        throw std::invalid_argument("Neuron ID is invalid");
    }
    
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    
    // Initialize with default parameters
    pImpl->state.membranePotential = -70.0f;  // resting potential
    pImpl->state.restingPotential = -70.0f;
    pImpl->state.threshold = -55.0f;
    pImpl->state.resetPotential = -70.0f;
    pImpl->state.leakConductance = 0.1f;
    pImpl->state.refractoryPeriod = 5;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    pImpl->state.firingRate = 0.0f;
    
    NLM_LOG_DEBUG("Neuron: Created neuron " + std::to_string(id.index()) + " with default parameters");
}

Neuron::~Neuron() {
    // Cleanup resources if any
    pImpl.reset();
}

Neuron::Neuron(Neuron&& other) noexcept : pImpl(std::move(other.pImpl)) {
    other.pImpl = nullptr;
    NLM_LOG_DEBUG("Neuron: Move constructor completed");
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = std::move(other.pImpl);
        other.pImpl = nullptr;
        NLM_LOG_DEBUG("Neuron: Move assignment completed");
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    // Validate input parameter
    if (type == NeuronType::Modulatory) {
        NLM_LOG_WARNING("Neuron: Modulatory neuron type is not fully supported in LIF implementation");
    }
    
    pImpl->type = type;
    NLM_LOG_DEBUG("Neuron: Set type to " + std::to_string(static_cast<int>(type)));
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Validate input
    if (std::isnan(potential)) {
        NLM_LOG_ERROR("Neuron: Cannot set NaN membrane potential");
        return;
    }
    
    if (std::isinf(potential)) {
        NLM_LOG_ERROR("Neuron: Cannot set infinite membrane potential");
        return;
    }
    
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    // Validate input
    if (std::isnan(delta) || std::isinf(delta)) {
        NLM_LOG_ERROR("Neuron: Invalid delta for membrane potential change");
        return;
    }
    
    MembranePotential newPotential = pImpl->state.membranePotential + delta;
    
    // Check for overflow/underflow
    if (std::abs(newPotential) > 1000.0f) {
        NLM_LOG_WARNING("Neuron: Membrane potential change may cause instability (new value: " + std::to_string(newPotential) + ")");
    }
    
    pImpl->state.membranePotential = newPotential;
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Validate input
    if (std::isnan(threshold) || std::isinf(threshold)) {
        NLM_LOG_ERROR("Neuron: Invalid threshold value");
        return;
    }
    
    // Ensure threshold is reasonable for resting potential
    if (threshold < -100.0f || threshold > 50.0f) {
        NLM_LOG_WARNING("Neuron: Threshold outside typical range: " + std::to_string(threshold) + "mV");
    }
    
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
    NLM_LOG_DEBUG("Neuron: Set firing state to " + std::to_string(static_cast<int>(state)));
}

void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
            NLM_LOG_DEBUG("Neuron: Refractory period ended");
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    if (std::isnan(rate) || std::isinf(rate) || rate < 0.0f) {
        NLM_LOG_ERROR("Neuron: Invalid firing rate: " + std::to_string(rate));
        return;
    }
    
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (std::isnan(conductance) || std::isinf(conductance) || conductance < 0.0f) {
        NLM_LOG_ERROR("Neuron: Invalid leak conductance: " + std::to_string(conductance));
        return;
    }
    
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (steps == 0) {
        NLM_LOG_DEBUG("Neuron: Refractory period set to 0 (no refractory)");
    } else if (steps > 100) {  // Reasonable upper bound
        NLM_LOG_WARNING("Neuron: Refractory period unusually high: " + std::to_string(steps) + " steps");
    }
    
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (std::isnan(potential) || std::isinf(potential)) {
        NLM_LOG_ERROR("Neuron: Invalid resting potential");
        return;
    }
    
    pImpl->state.restingPotential = potential;
    NLM_LOG_DEBUG("Neuron: Set resting potential to " + std::to_string(potential) + "mV");
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (std::isnan(potential) || std::isinf(potential)) {
        NLM_LOG_ERROR("Neuron: Invalid reset potential");
        return;
    }
    
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Validate input amplitude
    if (std::isnan(amplitude) || std::isinf(amplitude)) {
        NLM_LOG_ERROR("Neuron: Invalid excitatory input amplitude");
        return;
    }
    
    if (amplitude < 0.0f) {
        NLM_LOG_WARNING("Neuron: Negative excitatory input (may indicate error): " + std::to_string(amplitude));
    }
    
    pImpl->synapticInput += amplitude;
    
    NLM_LOG_DEBUG("Neuron: Received excitatory input of " + std::to_string(amplitude) + " mV");
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Validate input amplitude
    if (std::isnan(amplitude) || std::isinf(amplitude)) {
        NLM_LOG_ERROR("Neuron: Invalid inhibitory input amplitude");
        return;
    }
    
    if (amplitude < 0.0f) {
        NLM_LOG_WARNING("Neuron: Negative inhibitory input (may indicate error): " + std::to_string(amplitude));
    }
    
    pImpl->synapticInput -= amplitude;
    
    NLM_LOG_DEBUG("Neuron: Received inhibitory input of " + std::to_string(amplitude) + " mV");
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Validate input amplitude
    if (std::isnan(amplitude) || std::isinf(amplitude)) {
        NLM_LOG_ERROR("Neuron: Invalid modulatory input amplitude");
        return;
    }
    
    pImpl->state.adaptationVariable += amplitude * 0.1f;
    
    NLM_LOG_DEBUG("Neuron: Received modulatory input of " + std::to_string(amplitude) + " mV");
}

void Neuron::injectCurrent(MembranePotential current) {
    // Validate input current
    if (std::isnan(current) || std::isinf(current)) {
        NLM_LOG_ERROR("Neuron: Invalid current injection amount");
        return;
    }
    
    pImpl->synapticInput += current;
    
    NLM_LOG_DEBUG("Neuron: Injected current of " + std::to_string(current) + " mV");
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (std::isnan(timestamp) || std::isinf(timestamp)) {
        NLM_LOG_ERROR("Neuron: Invalid spike timestamp");
        return;
    }
    
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
    
    NLM_LOG_DEBUG("Neuron: Spike recorded at timestamp " + std::to_string(timestamp));
}

void Neuron::clearSpikeHistory() {
    size_t oldSize = pImpl->spikeHistory.size();
    pImpl->spikeHistory.clear();
    NLM_LOG_DEBUG("Neuron: Cleared " + std::to_string(oldSize) + " spike records");
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (pImpl) {
        pImpl->incomingSynapses.push_back(handle);
        NLM_LOG_DEBUG("Neuron: Added incoming synapse " + std::to_string(handle.index()));
    } else {
        NLM_LOG_ERROR("Neuron: Cannot add incoming synapse - pImpl is null");
    }
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (pImpl) {
        pImpl->outgoingSynapses.push_back(handle);
        NLM_LOG_DEBUG("Neuron: Added outgoing synapse " + std::to_string(handle.index()));
    } else {
        NLM_LOG_ERROR("Neuron: Cannot add outgoing synapse - pImpl is null");
    }
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

    NLM_LOG_DEBUG("Neuron: LIF step at time " + std::to_string(currentTime) + 
                 " (dt=" + std::to_string(dt) + ", fired=" + std::to_string(fired) + ")");
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    NLM_LOG_DEBUG("Neuron: Resetting neuron state");
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Validate random generator
    if (!rng.isValid()) {
        NLM_LOG_ERROR("Neuron: Invalid random generator for initialization");
        return;
    }
    
    NLM_LOG_DEBUG("Neuron: Initializing with random parameters");
    
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
    pImpl->state.firingRate = rng.uniformReal(0.0f, 50.0f);  // Hz
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    
    NLM_LOG_DEBUG("Neuron: Random initialization complete (threshold=" + std::to_string(pImpl->state.threshold) + 
                 "mV, resting=" + std::to_string(pImpl->state.restingPotential) + "mV, refractory=" + std::to_string(pImpl->state.refractoryPeriod) + ")");
}

NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

MembranePotential Neuron::getResetPotential() const {
    return pImpl->state.resetPotential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

FiringState Neuron::getFiringState() const {
    return pImpl->state.firingState;
}

bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Refractory;
}

size_t Neuron::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

float Neuron::getAdaptationVariable() const {
    return pImpl->state.adaptationVariable;
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}
