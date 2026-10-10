#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real integrate-and-fire dynamics implementation
    const auto& state = neuron->getState();
    
    // Leaky Integrate-and-Fire (LIF) model
    // dV/dt = (-V + I)/(tau_m) where V is membrane potential,
    // I is input current, tau_m is membrane time constant
    
    float V = neuron->getMembranePotential();
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float g = pImpl->membraneConductance;
    
    // Convert current to voltage using Ohm's law: I = (V - V_rest) / R
    // But we use conductance-based model: I = g * (V - E)
    // where E is reversal potential (typically 0 for excitatory)
    
    float E_synapse = 0.0f;  // Synaptic reversal potential
    float dV = (-V + I + g * (E_synapse - V)) * dt / tau;
    
    // Update membrane potential with Euler integration
    neuron->setMembranePotential(V + dV);
    
    // Check for spike threshold crossing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<double>(dt));  // Pass actual time
        
        // Reset after spike
        neuron->setMembranePotential(state.resetPotential);
        neuron->setFiringState(FiringState::Refractory);
        neuron->setRefractoryRemaining(state.refractoryPeriod);
    }
    
    // Handle refractory period decay
    if (neuron->isRefractory()) {
        float remaining = neuron->getRefractoryRemaining();
        remaining -= dt;
        neuron->setRefractoryRemaining(std::max(0.0, remaining));
        
        if (remaining <= 0.0) {
            neuron->setFiringState(FiringState::Resting);
        }
    }
    
    // Clear synaptic currents for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics: conductance-based exponential decay
    float g = synapse->getConductance();
    float tau_syn = synapse->getTimeConstant();
    
    // Exponential decay of synaptic conductance
    float dg = -g / tau_syn * dt;
    float new_g = g + dg;
    synapse->setConductance(std::max(0.0f, new_g));
    
    // Check for spike generation based on conductance
    if (new_g > synapse->getThreshold()) {
        synapse->setActive(true);
    } else {
        synapse->setActive(false);
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!synapse->isActive()) {
        return;
    }
    
    float weight = synapse->getWeight();
    float delay = synapse->getDelay();
    
    // Synaptic reversal potential based on synapse type
    float E_synapse = 0.0f;  // Excitatory
    if (synapse->getType() == SynapseType::Inhibitory) {
        E_synapse = -80.0f;  // Inhibitory reversal potential
    }
    
    // Calculate synaptic current using conductance-based model
    float g = synapse->getConductance();
    float V = neuron->getMembranePotential();
    float I_syn = g * (E_synapse - V);
    
    // Scale by synaptic weight
    I_syn *= weight;
    
    // Add to neuron's total current
    neuron->receiveExcitatoryInput(I_syn);
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Check if membrane potential exceeds threshold
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // Reset internal state
    pImpl->membraneTimeConstant = 20.0f;  // Default membrane time constant
    pImpl->membraneConductance = 0.0f;    // Reset synaptic conductance
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

void IntegrateAndFireDynamics::setMembraneConductance(float g) {
    pImpl->membraneConductance = g;
}

float IntegrateAndFireDynamics::getMembraneConductance() const {
    return pImpl->membraneConductance;
}

} // namespace nlm