#include "FitzHughNagumoDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct FitzHughNagumoDynamics::Impl {
    // FHN model parameters
    float a;           // Recovery variable sensitivity
    float b;           // Recovery variable coupling
    float c;           // External current parameter
    float I_app;       // Applied current
    float tau;         // Time constant
    
    // Resting state values
    const float V_rest;  // Resting membrane potential
    const float w_rest;  // Resting recovery variable
    
    Impl() : a(0.08f), b(0.7f), c(0.8f), I_app(0.0f), tau(12.5f), 
             V_rest(-1.2f), w_rest(0.26f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {
    reset();
}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() {
    delete pImpl;
}

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    // Update time step
    pImpl->tau = static_cast<float>(dt);
    
    // Get current state
    float V = neuron->getMembranePotential();
    float I_inj = neuron->getTotalCurrent();
    neuron->clearTotalCurrent();
    
    // Compute derivatives for FHN model
    // dV/dt = (V - V^3/3 + w + I_app + I_inj) / tau
    // dw/dt = (a * (V + b - w)) / tau
    
    float dV_dt = (V - V*V*V/3.0f + pImpl->w_ + pImpl->I_app + I_inj) / pImpl->tau;
    float dw_dt = pImpl->a * (V + pImpl->b - pImpl->w_) / pImpl->tau;
    
    // Update state variables using Euler integration
    pImpl->v_ += dV_dt * pImpl->tau;
    pImpl->w_ += dw_dt * pImpl->tau;
    
    // Set neuron membrane potential from FHN model variable
    neuron->setMembranePotential(pImpl->v_);
    
    // Check for firing threshold
    if (shouldFire(neuron) && !neuron->isRefractory()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: use actual time
    }
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // FHN synapses can have nonlinear dynamics
    // For now, use standard synaptic dynamics
    synapse->step(static_cast<double>(dt));
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // FHN neurons have different integration of synaptic inputs
    // Synaptic input affects both V and w variables
    SynapticWeight weight = synapse->getWeight();
    
    if (synapse->isExcitatory()) {
        // Excitatory input increases V
        float current = weight * 0.5f;  // Scale factor
        pImpl->v_ += current;
        neuron->setMembranePotential(pImpl->v_);
    } else {
        // Inhibitory input decreases V
        float current = weight * 0.5f;
        pImpl->v_ -= current;
        neuron->setMembranePotential(pImpl->v_);
        
        // Also affects recovery variable
        pImpl->w_ -= current * 0.1f;
    }
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    float V = neuron->getMembranePotential();
    // FHN firing threshold: V >= threshold (V_rest + 0.5)
    return V >= (pImpl->V_rest + 0.5f);
}

void FitzHughNagumoDynamics::reset() {
    pImpl->v_ = pImpl->V_rest;
    pImpl->w_ = pImpl->w_rest;
    pImpl->I_app = 0.0f;
}

} // namespace nlm