#include "NeuralDynamics.hpp"
#include <cmath>

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
    
    const auto& state = neuron->getState();
    
    // Leaky integration: dV/dt = -(V - V_rest) / tau + I / R
    // Using explicit Euler for stability and performance
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Ensure dt is positive to prevent numerical issues
    float dtFloat = std::max(static_cast<float>(dt), 1e-6f);
    
    // Apply leak current: I_leak = (V - V_rest) / R
    // Apply input current: I_inj = I
    // Total current drives voltage change: dV/dt = -(I_leak) / C + I_inj / C
    // where C is membrane capacitance (implicit in R and tau)
    
    float dV = (-(V - V_rest) / tau + I / R) * dtFloat;
    
    // Update membrane potential with bounds checking
    float newV = V + dV;
    neuron->setMembranePotential(newV);
    
    // Check for threshold crossing and handle firing
    if (shouldFire(neuron)) {
        // Record precise spike time for STDP calculations
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(dt);  // Use current simulation time
        
        // Immediately reset membrane potential to avoid multiple spikes
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Handle refractory period if neuron is refractory
    if (neuron->isRefractory()) {
        // Force membrane potential to reset value during refractory period
        neuron->setMembranePotential(state.resetPotential);
        neuron->decrementRefractory();
    }
    
    // Clear injected current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Apply synaptic dynamics for one timestep
    // This includes short-term plasticity effects and temporal filtering
    synapse->step(static_cast<Timestamp>(dt));
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // Apply synaptic current to neuron based on synapse weight and type
    SynapticWeight weight = synapse->getWeight();
    
    // Excitatory synapses depolarize neuron (positive current)
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } 
    // Inhibitory synapses hyperpolarize neuron (negative current)
    else if (synapse->isInhibitory()) {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
    
    // Modulatory synapses affect neuron excitability without directly causing spikes
    else if (synapse->getType() == SynapseType::Modulatory) {
        neuron->receiveModulatoryInput(std::abs(weight));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // No internal state to reset
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
