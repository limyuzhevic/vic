#pragma once

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
    MembranePotential totalCurrent;
    MembranePotential synapticInput;
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    static constexpr float TIME_CONSTANT = 20.0f;
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
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
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
    pImpl->state.lastSpikeTime = static_cast<float>(timestamp);
    pImpl->state.firingState = FiringState::Active;
}

void Neuron::step(float dt) {
    // LIF dynamics simulation
    if (pImpl->state.refractoryRemaining > 0) {
        pImpl->state.refractoryRemaining--;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return;
    }
    
    // Leak current
    float leakCurrent = pImpl->state.leakConductance * 
                        (pImpl->state.restingPotential - pImpl->state.membranePotential);
    
    // Total current includes synaptic input
    float totalCurrent = pImpl->state.leakConductance * pImpl->synapticInput;
    
    // Update membrane potential
    float dV = (totalCurrent - leakCurrent) * dt / pImpl->state.leakConductance;
    pImpl->state.membranePotential += dV;
    
    // Apply adaptation
    if (pImpl->state.adaptationVariable > 0.0f) {
        pImpl->state.membranePotential -= pImpl->state.adaptationVariable * dt;
        pImpl->state.adaptationVariable *= std::exp(-dt / 100.0f);
    }
    
    // Update firing rate (simple rate model)
    pImpl->state.firingRate = std::max(0.0f, 
        std::min(100.0f, pImpl->state.membranePotential / 20.0f));
    
    // Reset if threshold exceeded
    if (pImpl->state.membranePotential >= pImpl->state.threshold) {
        pImpl->state.membranePotential = pImpl->state.resetPotential;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
    }
}

NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
}

const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    return pImpl->state.refractoryRemaining > 0;
}

FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
}

MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->totalCurrent;
}

void Neuron::setTotalCurrent(MembranePotential current) {
    pImpl->totalCurrent = current;
}

void Neuron::clearTotalCurrent() {
    pImpl->totalCurrent = 0.0f;
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

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Neuron::setRegionId(RegionId id) {
    pImpl->regionId = id;
}

RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId id) {
    pImpl->populationId = id;
}

PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}
