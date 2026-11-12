#include "NeuralDynamics.hpp"
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
    if (!neuron) {
        NLM_LOG_ERROR("Null neuron pointer in updateNeuron");
        return;
    }
    
    const auto& state = neuron->getState();
    
    // Leaky integration: dV/dt = -(V - V_rest) / tau + I / C
    // V = membrane potential, V_rest = resting potential
    // tau = membrane time constant, I = total current, C = membrane capacitance
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Calculate derivative: (V_rest - V)/tau + I/R
    float dV_dt = (V_rest - V) / tau + I / R;
    
    // Update membrane potential with exponential Euler integration
    // V_new = V + dt * dV_dt
    V += static_cast<float>(dt) * 1000.0f * dV_dt;  // dt is in seconds, convert to ms
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (state.adaptationVariable > 0.0f) {
        V -= state.adaptationVariable * 0.01f;
        neuron->getState().adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    neuron->setMembranePotential(V);
    
    // Check for spike threshold crossing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) {
        NLM_LOG_ERROR("Null synapse pointer in updateSynapse");
        return;
    }
    
    // Simple exponential decay of synaptic weight
    synapse->step(static_cast<float>(dt));
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) {
        NLM_LOG_ERROR("Null neuron or synapse pointer in applySpikeInput");
        return;
    }
    
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else if (synapse->isInhibitory()) {
        neuron->receiveInhibitoryInput(std::abs(weight));
    } else if (synapse->isModulatory()) {
        neuron->receiveModulatoryInput(weight);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) {
        NLM_LOG_ERROR("Null neuron pointer in shouldFire");
        return false;
    }
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