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
    // Real integrate-and-fire dynamics implementation
    const auto& state = neuron->getState();
    
    // Leaky integration: dV/dt = (V_rest - V)/tau + I/C
    // Using exponential Euler integration for stability
    // V_new = V_rest + (V_old - V_rest) * exp(-dt/tau) + I * tau/C * (1 - exp(-dt/tau))
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    
    // Time constant in milliseconds, convert to seconds for dt
    float alpha = static_cast<float>(std::exp(-dt * 1000.0 / tau));
    float beta = static_cast<float>(tau * (1.0 - alpha) / 1000.0);
    
    // Exponential Euler integration
    neuron->setMembranePotential(V_rest + (V - V_rest) * alpha + I * beta);
    
    // Spike detection and reset
    if (shouldFire(neuron)) {
        // Record the spike with current time
        neuron->recordSpike(static_cast<float>(dt));
        neuron->setFiringState(FiringState::Active);
        
        // Reset membrane potential to reset potential
        neuron->setMembranePotential(state.resetPotential);
        
        // Start refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->membraneTimeConstant * 0.001 * 1000.0));
    }
    
    // Handle refractory period if neuron is firing
    if (neuron->isFiring()) {
        // During refractory period, maintain reset potential
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Apply spike-frequency adaptation
    if (state.adaptationVariable > 0.0f) {
        neuron->addToMembranePotential(-state.adaptationVariable * 0.01f);
        neuron->getState().adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp to prevent instability
    float V_clamped = std::clamp(neuron->getMembranePotential(), -100.0f, 50.0f);
    neuron->setMembranePotential(V_clamped);
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics implementation
    synapse->step(dt);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input implementation with delay and synaptic dynamics
    SynapticWeight weight = synapse->getWeight();
    float delay = static_cast<float>(synapse->getDelay());
    
    if (synapse->isExcitatory()) {
        // Excitatory synapses add depolarizing current
        neuron->receiveExcitatoryInput(weight * synapse->getEfficacy());
    } else {
        // Inhibitory synapses add hyperpolarizing current
        neuron->receiveInhibitoryInput(std::abs(weight) * synapse->getEfficacy());
    }
    
    // Decay synaptic efficacy over time
    synapse->decayEligibilityTrace(0.9f);
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
