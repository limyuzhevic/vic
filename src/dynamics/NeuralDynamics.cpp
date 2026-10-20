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
    // Real integrate-and-fire dynamics (LIF model)
    // Differential equation: C * dV/dt = - (V - V_rest)/R + I
    // Where:
    //   C = membrane capacitance
    //   R = membrane resistance  
    //   I = total synaptic + injected current
    //   V_rest = resting potential
    //   V = current membrane potential
    
    auto& state = neuron->getState();
    const float dt_float = static_cast<float>(dt);
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        // During refractory period, neuron cannot spike and resets to reset potential
        neuron->setMembranePotential(state.resetPotential);
        neuron->decrementRefractory();
        
        // Clear synaptic input during refractory period
        neuron->clearTotalCurrent();
        return;
    }
    
    // Convert membrane potential from mV to dimensionless form
    // V_normalized = (V - V_reset) / (threshold - V_reset) in range [0, 1]
    const float V_normalized = (state.membranePotential - state.resetPotential) / 
                              (state.threshold - state.resetPotential);
    
    // Calculate synaptic input (I_syn) from accumulated synaptic inputs
    float I_syn = neuron->getTotalCurrent();
    
    // Calculate leak current: I_leak = (V - V_rest) / R
    const float I_leak = (state.membranePotential - state.restingPotential) / 
                        pImpl->membraneResistance;
    
    // Calculate total current: I_total = I_syn - I_leak
    const float I_total = I_syn - I_leak;
    
    // Membrane capacitance time constant: tau = R * C
    const float tau = pImpl->membraneTimeConstant;
    
    // Update membrane potential: dV/dt = I_total / C
    // Using exponential Euler integration for stability:
    // V_new = V_rest + (V_old - V_rest) * exp(-dt/tau) + R * (1 - exp(-dt/tau)) * I_total
    const float alpha = std::exp(-dt_float / tau);
    
    state.membranePotential = state.restingPotential + 
                             (state.membranePotential - state.restingPotential) * alpha +
                             pImpl->membraneResistance * (1.0f - alpha) * I_total;
    
    // Apply spike-frequency adaptation if present
    if (state.adaptationVariable > 0.0f) {
        state.membranePotential -= state.adaptationVariable * 0.01f;
        state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp to biologically plausible range
    state.membranePotential = std::clamp(state.membranePotential, -100.0f, 50.0f);
    
    // Check for spike threshold crossing
    if (state.membranePotential >= state.threshold) {
        neuron->setFiringState(FiringState::Active);
        state.lastSpikeTime = 0.0f;  // TODO: pass actual simulation time
        neuron->recordSpike(0.0f);
        
        // Reset membrane potential to reset value
        state.membranePotential = state.resetPotential;
        
        // Enter refractory period
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        
        // Update adaptation variable for spike-frequency adaptation
        state.adaptationVariable += 1.0f;
    }
    
    // Clear synaptic input for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // TODO PHASE 2: Implement real synaptic dynamics
    // PLACEHOLDER: Synapse decay
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // TODO PHASE 2: Implement real synaptic input
    // PLACEHOLDER: Simple additive input
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
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
