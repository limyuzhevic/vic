#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>

namespace nlm {

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
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

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryRemaining = steps;
    if (steps > 0) {
        pImpl->state.firingState = FiringState::Refractory;
    }
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

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // TODO PHASE 2: Implement real synaptic input dynamics
    // PLACEHOLDER: Simple additive input
    pImpl->totalCurrent += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // TODO PHASE 2: Implement real inhibitory synaptic input
    // PLACEHOLDER: Simple subtractive input
    pImpl->totalCurrent -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // TODO PHASE 2: Implement modulatory input (does not sum linearly)
    // PLACEHOLDER: Modulatory input stored separately
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    pImpl->totalCurrent += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->totalCurrent = 0.0f;
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

void Neuron::step(Timestamp currentTime) {
    // TODO PHASE 2:
    // Implement real integrate-and-fire dynamics:
    // 1. If refractory, decrement and return
    // 2. Update membrane potential based on total current
    // 3. Apply spike-frequency adaptation
    // 4. Check threshold
    // 5. If firing, record spike and reset potential
    // 6. Clear total current for next step
    
    decrementRefractory();
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->totalCurrent = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // TODO PHASE 2: Implement proper random initialization
    // PLACEHOLDER: Initialize with small random variations
    pImpl->state.membranePotential = -70.0f + rng.uniformReal(-5.0, 5.0);
    pImpl->state.threshold = -55.0f + rng.uniformReal(-3.0, 3.0);
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0, 2.0);
}

} // namespace nlm
