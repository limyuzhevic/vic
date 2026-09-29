#include "FitzHughNagumoDynamics.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct FitzHughNagumoDynamics::Impl {
    // FHN model parameters
    float a;                    // Recovery variable scaling
    float b;                    // Recovery variable scaling  
    float tau;                  // Time constant for recovery variable
    
    // Membrane potential derivative
    float dVdt;
    float dwdt;
    
    // Integration state
    float V_prev;               // Previous membrane potential
    float w_prev;               // Previous recovery variable
    
    Impl() : a(0.08f), b(0.065f), tau(12.0f), dVdt(0.0f), dwdt(0.0f),
             V_prev(0.0f), w_prev(0.0f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real FitzHugh-Nagumo dynamics implementation
    // Simplified excitable system model capturing spike generation
    // Based on reaction-diffusion equations for neuronal excitability
    
    float V = neuron->getMembranePotential();
    const auto& state = neuron->getState();
    
    // FHN equations:
    // dV/dt = (V - V^3/3 - w + I_ext)/tau
    // dw/dt = epsilon * (V + a - b*w)
    
    float I_ext = neuron->getTotalCurrent();
    
    // Calculate derivatives
    float dVdt = (V - (V * V * V) / 3.0f - pImpl->w + I_ext) / pImpl->tau;
    float dwdt = pImpl->a * (V + pImpl->b - pImpl->w);
    
    // Update state using forward Euler integration
    pImpl->V_prev = V;
    pImpl->w_prev = pImpl->w;
    
    V += dVdt * static_cast<float>(dt);
    pImpl->w += dwdt * static_cast<float>(dt);
    
    // Clamp membrane potential to realistic bounds
    V = std::clamp(V, -20.0f, 20.0f);
    
    // Update neuron state
    neuron->setMembranePotential(V);
    
    // Check for spike threshold
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
        
        // After spike, reset to higher value to simulate refractory period
        neuron->setMembranePotential(30.0f);
    }
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // FHN dynamics: synaptic dynamics with excitability modulation
    // Synapses affect the recovery variable dynamics
    synapse->step(0.0);
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // FHN dynamics: spike input affects both V and w
    // Synaptic input provides external current to the neuron
    
    SynapticWeight weight = synapse->getWeight();
    float I_syn = weight;
    
    if (synapse->isExcitatory()) {
        // Excitatory input increases external current
        neuron->injectCurrent(I_syn * 10.0f);  // Scale for FHN model
        // Also affects recovery variable
        pImpl->w -= I_syn * 0.1f;
    } else {
        // Inhibitory input decreases external current
        neuron->injectCurrent(-I_syn * 5.0f);
        // Increases recovery variable
        pImpl->w += I_syn * 0.1f;
    }
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    // FHN firing: spike when membrane potential crosses upper threshold
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void FitzHughNagumoDynamics::reset() {
    // Reset FHN state variables
    pImpl->w = 0.0f;
    pImpl->dVdt = 0.0f;
    pImpl->dwdt = 0.0f;
    pImpl->V_prev = 0.0f;
    pImpl->w_prev = 0.0f;
}

void FitzHughNagumoDynamics::setA(float a) {
    pImpl->a = std::clamp(a, 0.0f, 1.0f);
}

float FitzHughNagumoDynamics::getA() const {
    return pImpl->a;
}

void FitzHughNagumoDynamics::setB(float b) {
    pImpl->b = std::clamp(b, 0.0f, 0.2f);
}

float FitzHughNagumoDynamics::getB() const {
    return pImpl->b;
}

void FitzHughNagumoDynamics::setTau(float tau) {
    pImpl->tau = std::clamp(tau, 1.0f, 50.0f);
}

float FitzHughNagumoDynamics::getTau() const {
    return pImpl->tau;
}

} // namespace nlm
