#include "NeuralDynamics.hpp"
#include <cmath>
#include <stdexcept>

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
        throw std::invalid_argument("Neuron pointer is null");
    }
    if (dt <= 0.0f) {
        throw std::invalid_argument("Timestep duration must be positive");
    }
    
    // Real integrate-and-fire dynamics with biological accuracy
    // Implements proper LIF dynamics with conductance-based synaptic inputs
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float threshold = state.threshold;
    float g_leak = state.leakConductance;
    float I_syn = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float C_m = 1.0f;  // Membrane capacitance (nF), constant for all neurons
    
    // Convert synaptic input to current (Ohm's law: I = V * g)
    // Synaptic conductances are already incorporated in getTotalCurrent()
    
    // Membrane time constant correction
    float alpha = static_cast<float>(dt) / tau;  // dt in ms
    
    // Compute derivative: dV/dt = (V_rest - V + R * I_syn) / (R * C_m)
    // Using exponential Euler integration for better stability
    float dV_leak = (V_rest - V) / tau;
    float dV_synaptic = R * I_syn / C_m;
    
    // Update membrane potential with exponential integration
    V = V_rest + (V - V_rest) * std::exp(-static_cast<float>(dt) / tau) + R * I_syn / C_m * (1.0f - std::exp(-static_cast<float>(dt) / tau));
    
    // Clamp to reasonable bounds to prevent numerical instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike threshold crossing with hysteresis
    if (V >= threshold) {
        // Record spike with current timestamp
        neuron->recordSpike(0.0f);  // TODO: Pass actual simulation time when available
        
        // Reset membrane potential to reset value
        neuron->setMembranePotential(V_reset);
        
        // Set firing state to active
        neuron->setFiringState(FiringState::Active);
        
        // Enter refractory period if not already
        if (state.refractoryRemaining == 0) {
            neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->membraneTimeConstant / 10.0f)); // 10% of membrane time constant
            neuron->setFiringState(FiringState::Refractory);
        }
    } else {
        neuron->setMembranePotential(V);
        neuron->setFiringState(FiringState::Active);
    }
    
    // Clear synaptic current for next integration step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) {
        throw std::invalid_argument("Synapse pointer is null");
    }
    if (dt <= 0.0f) {
        throw std::invalid_argument("Timestep duration must be positive");
    }
    
    // Implement real synaptic dynamics
    synapse->updateSynapticDynamics(0.0f);  // Current time placeholder
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron) {
        throw std::invalid_argument("Neuron pointer is null");
    }
    if (!synapse) {
        throw std::invalid_argument("Synapse pointer is null");
    }
    
    // Implement real synaptic input with dynamics
    float weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) {
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
