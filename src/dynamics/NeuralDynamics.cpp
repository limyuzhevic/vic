#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float capacitance;            // nF
    float restingPotential;       // mV
    float thresholdPotential;     // mV
    float resetPotential;         // mV
    float refractoryPeriod;        // ms
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), capacitance(1.0f),
             restingPotential(-70.0f), thresholdPotential(-50.0f), resetPotential(-75.0f),
             refractoryPeriod(2.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real integrate-and-fire dynamics
    // dV/dt = -(V - V_rest) / tau + I / C
    // Where tau = R * C (membrane time constant)
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    
    // Compute membrane time constant tau = R * C
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float C = pImpl->capacitance;
    
    // Ensure tau is positive to avoid division by zero
    if (tau <= 0.0f) tau = 0.001f;
    
    // Update membrane potential using Euler integration
    // V_new = V_old + dt * (-(V_old - V_rest) / tau + I / C)
    float dV = (-(V - V_rest) / tau + I / C) * static_cast<float>(dt);
    V += dV;
    
    // Refractory period handling
    if (neuron->isRefractory()) {
        V = pImpl->resetPotential;
        neuron->decrementRefractoryTime(dt);
        neuron->setMembranePotential(V);
        neuron->clearTotalCurrent();
        return;
    }
    
    // Apply reset after spike
    if (neuron->getFiringState() == FiringState::Active) {
        V = pImpl->resetPotential;
        neuron->setFiringState(FiringState::Refractory);
        neuron->setRefractoryTime(pImpl->refractoryPeriod);
    }
    
    // Update membrane potential
    neuron->setMembranePotential(V);
    
    // Check for firing threshold crossing (only if not in refractory)
    if (neuron->getFiringState() != FiringState::Refractory && 
        V >= neuron->getThreshold()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with exponential decay
    if (!synapse) return;
    
    // Get current weight
    SynapticWeight weight = synapse->getWeight();
    
    // Compute synaptic decay factor
    // Standard exponential decay: w(t) = w(0) * exp(-t/tau_syn)
    float tau_syn = 5.0f; // Synaptic time constant (ms)
    float decayFactor = std::exp(-static_cast<float>(dt) / tau_syn);
    
    // Apply decay
    float newWeight = weight * decayFactor;
    synapse->setWeight(newWeight);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input dynamics with synaptic efficacy
    if (!neuron || !synapse) return;
    
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    // Modified synaptic current calculation
    // I = weight * efficacy * (1 + temporal_filter)
    float temporalFilter = 1.0f; // For now, could be expanded to include dynamics
    
    float synapticCurrent = weight * efficacy * temporalFilter;
    
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(synapticCurrent);
    } else {
        neuron->receiveInhibitoryInput(std::abs(synapticCurrent));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Check if neuron has reached firing threshold
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // Reset to default parameters
    pImpl->membraneTimeConstant = 20.0f;
    pImpl->membraneResistance = 10.0f;
    pImpl->capacitance = 1.0f;
    pImpl->restingPotential = -70.0f;
    pImpl->thresholdPotential = -50.0f;
    pImpl->resetPotential = -75.0f;
    pImpl->refractoryPeriod = 2.0f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = std::max(1.0f, std::min(tau, 100.0f));
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

void IntegrateAndFireDynamics::setMembraneResistance(float R) {
    pImpl->membraneResistance = std::max(1.0f, std::min(R, 1000.0f));
}

float IntegrateAndFireDynamics::getMembraneResistance() const {
    return pImpl->membraneResistance;
}

void IntegrateAndFireDynamics::setCapacitance(float C) {
    pImpl->capacitance = std::max(0.1f, std::min(C, 10.0f));
}

float IntegrateAndFireDynamics::getCapacitance() const {
    return pImpl->capacitance;
}

void IntegrateAndFireDynamics::setRestingPotential(float V_rest) {
    pImpl->restingPotential = std::max(-100.0f, std::min(V_rest, -40.0f));
}

float IntegrateAndFireDynamics::getRestingPotential() const {
    return pImpl->restingPotential;
}

void IntegrateAndFireDynamics::setThresholdPotential(float V_thresh) {
    pImpl->thresholdPotential = std::max(-90.0f, std::min(V_thresh, -30.0f));
}

float IntegrateAndFireDynamics::getThresholdPotential() const {
    return pImpl->thresholdPotential;
}

void IntegrateAndFireDynamics::setResetPotential(float V_reset) {
    pImpl->resetPotential = std::max(-100.0f, std::min(V_reset, -50.0f));
}

float IntegrateAndFireDynamics::getResetPotential() const {
    return pImpl->resetPotential;
}

void IntegrateAndFireDynamics::setRefractoryPeriod(float duration) {
    pImpl->refractoryPeriod = std::max(0.5f, std::min(duration, 50.0f));
}

float IntegrateAndFireDynamics::getRefractoryPeriod() const {
    return pImpl->refractoryPeriod;
}

} // namespace nlm
