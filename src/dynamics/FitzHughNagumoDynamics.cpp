// FitzHugh-Nagumo dynamics implementation
// Simplified excitation/inhibition model for spiking neuron dynamics

#include "FitzHughNagumoDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>

namespace nlm {

struct FitzHughNagumoDynamics::Impl {
    // FHN model parameters
    float maxRate;              // Maximum firing rate (Hz)
    float threshold;            // Firing threshold (mV)
    float recoveryTimeConstant; // Recovery time constant (ms)
    float resetPotential;       // Reset potential (mV)
    
    // Model coefficients
    float a;                    // Recovery variable coefficient
    float b;                    // Recovery variable coefficient
    float c;                    // Excitation coefficient
    float d;                    // Inhibition coefficient
    
    // State variables
    float V;                    // Membrane potential (mV)
    float w;                    // Recovery variable
    
    Impl() : maxRate(100.0f), threshold(20.0f), recoveryTimeConstant(10.0f),
             resetPotential(-65.0f), a(0.08f), b(0.7f), c(0.8f), d(0.08f),
             V(-65.0f), w(0.0f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    float V = pImpl->V;
    float w = pImpl->w;
    float I_total = neuron->getTotalCurrent();
    
    // FHN differential equations:
    // dV/dt = (V - V^3/3 - w + I_ext)/tau
    // dw/dt = (V + a - b*w)/tau_recovery
    
    float tau = 1.0f;  // Time constant for membrane potential
    float tau_rec = pImpl->recoveryTimeConstant;  // Recovery time constant
    
    // Compute derivatives
    float dV = (V - (V * V * V) / 3.0f - w + I_total) / tau * static_cast<float>(dt);
    float dw = (V + pImpl->a - pImpl->b * w) / tau_rec * static_cast<float>(dt);
    
    // Update state variables
    V += dV;
    w += dw;
    
    // Update pointers
    pImpl->V = V;
    pImpl->w = w;
    
    // Apply neuronal state updates
    neuron->setMembranePotential(V);
    
    // Check for firing threshold crossing (spike detection)
    if (V >= state.threshold && !neuron->isRefractory()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<Timestamp>(dt));
        neuron->setRefractory(state.refractoryDuration);
        
        // Reset membrane potential after spike
        neuron->setMembranePotential(pImpl->resetPotential);
        
        // Apply spike-triggered recovery
        pImpl->w += pImpl->d;
    }
    
    // Update refractory timer
    if (neuron->isRefractory()) {
        neuron->updateRefractory(dt);
        if (!neuron->isRefractory()) {
            neuron->setMembranePotential(pImpl->resetPotential);
        }
    }
    
    // Clear input current
    neuron->clearTotalCurrent();
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // FHN synapses modify the recovery variable
    SynapticWeight weight = synapse->getWeight();
    
    if (synapse->isExcitatory()) {
        synapse->getDestinationNeuron()->receiveExcitatoryInput(weight);
    } else {
        synapse->getDestinationNeuron()->receiveInhibitoryInput(weight);
    }
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Direct synaptic coupling in FHN model
    SynapticWeight weight = synapse->getWeight();
    
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold() && 
           !neuron->isRefractory();
}

void FitzHughNagumoDynamics::reset() {
    pImpl->V = pImpl->resetPotential;
    pImpl->w = 0.0f;
}

void FitzHughNagumoDynamics::setMaxRate(float maxRate) {
    pImpl->maxRate = maxRate;
}

float FitzHughNagumoDynamics::getMaxRate() const {
    return pImpl->maxRate;
}

void FitzHughNagumoDynamics::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float FitzHughNagumoDynamics::getThreshold() const {
    return pImpl->threshold;
}

void FitzHughNagumoDynamics::setRecoveryTimeConstant(float tau) {
    pImpl->recoveryTimeConstant = tau;
}

float FitzHughNagumoDynamics::getRecoveryTimeConstant() const {
    return pImpl->recoveryTimeConstant;
}

} // namespace nlm