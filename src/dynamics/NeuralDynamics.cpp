#include "NeuralDynamics.hpp"
#include "FitzHughNagumoDynamics.hpp"
#include "HodgkinHuxleyDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float threshold;              // mV (threshold for firing)
    float reset_potential;        // mV (reset after spike)
    float leak_reversal;          // mV (resting potential)
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f),
             threshold(-50.0f), reset_potential(-70.0f), leak_reversal(-70.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron || dt <= 0.0f) return;
    
    const auto& state = neuron->getState();
    
    // Leaky integration: dV/dt = -(V - V_rest) / tau + I / C
    // For simplicity using explicit Euler:
    // V_new = V_old + dt * (-(V_old - V_rest) / tau + I / C)
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Simple Euler integration
    float dV = (-(V - V_rest) / tau + I / R) * static_cast<float>(dt);
    V += dV;
    
    // Update neuron membrane potential
    neuron->setMembranePotential(V);
    
    // Check for firing threshold
    if (V >= pImpl->threshold) {
        neuron->setFiringState(FiringState::Active);
        neuron->setMembranePotential(pImpl->reset_potential);
        neuron->recordSpike(0.0); // TODO: pass actual time
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(pImpl->reset_potential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse || dt <= 0.0f) return;
    
    // Exponential decay of synaptic conductance
    synapse->step(static_cast<float>(dt));
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // Simple additive synaptic input
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    return neuron->getMembranePotential() >= pImpl->threshold;
}

void IntegrateAndFireDynamics::reset() {
    // No internal state to reset
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    if (tau > 0) pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm