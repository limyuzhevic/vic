#include "NeuralDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    auto& state = neuron->getState();
    
    // Check refractory period first
    if (state.refractoryRemaining > 0) {
        --state.refractoryRemaining;
        if (state.refractoryRemaining == 0) {
            state.firingState = FiringState::Resting;
        }
        // Clamp to reset potential during refractory period
        state.membranePotential = state.resetPotential;
        return;
    }
    
    // LIF dynamics: dV/dt = (V_rest - V) / tau + I / C
    float V = state.membranePotential;
    float V_rest = state.restingPotential;
    float I = state.synapseConductance;  // Current from synapses
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Euler integration
    float dV = (-(V - V_rest) / tau + I / R) * static_cast<float>(dt);
    
    // Update membrane potential
    V += dV;
    V = std::clamp(V, -100.0f, 50.0f);
    state.membranePotential = V;
    
    // Check for spike
    if (V >= state.threshold) {
        state.firingState = FiringState::Active;
        state.lastSpikeTime = 0.0f;  // Will be updated by spike handler
        neuron->recordSpike(0.0f);
        
        // Reset membrane potential
        state.membranePotential = state.resetPotential;
        
        // Enter refractory period
        state.refractoryRemaining = state.refractoryPeriod;
        state.firingState = FiringState::Refractory;
        
        // Spike-frequency adaptation
        state.adaptationVariable += 1.0f;
    }
    
    // Update firing rate based on recent activity
    state.firingRate = static_cast<FiringRate>(neuron->getSpikeCount() / 10.0f);
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Simple synapse dynamics: weight decay and stochastic transmission
    float currentWeight = synapse->getWeight();
    
    // Apply small weight decay for stability
    float decayFactor = 0.999f;
    float newWeight = currentWeight * decayFactor;
    synapse->setWeight(newWeight);
    
    // Stochastic transmission probability based on weight
    float transmissionProb = std::min(1.0f, currentWeight * 0.1f + 0.01f);
    // This would be used by spike handlers to determine transmission
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    float weight = synapse->getWeight();
    
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    const auto& state = neuron->getState();
    
    if (state.firingState == FiringState::Refractory) {
        return false;  // Cannot fire during refractory period
    }
    
    return state.membranePotential >= state.threshold;
}

void IntegrateAndFireDynamics::reset() {
    // Reset membrane time constant
    pImpl->membraneTimeConstant = 20.0f;
    pImpl->membraneResistance = 10.0f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = std::max(1.0f, std::min(tau, 100.0f));
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
