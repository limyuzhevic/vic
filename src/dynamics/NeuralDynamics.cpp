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
    bool fired = false;
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        --neuron->getState().refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        neuron->setMembranePotential(neuron->getState().resetPotential);
        if (neuron->getState().refractoryRemaining == 0) {
            neuron->setFiringState(FiringState::Resting);
        }
        return fired;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = neuron->getState().membranePotential;
    MembranePotential V_rest = neuron->getState().restingPotential;
    MembranePotential V_reset = neuron->getState().resetPotential;
    MembranePotential threshold = neuron->getState().threshold;
    float tau = pImpl->membraneTimeConstant;
    float C = pImpl->membraneResistance;
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = neuron->getTotalCurrent() / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (neuron->getState().adaptationVariable > 0.0f) {
        V -= neuron->getState().adaptationVariable * 0.01f;
        neuron->getState().adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        neuron->setFiringState(FiringState::Active);
        neuron->setLastSpikeTime(static_cast<float>(currentTime));
        
        // Record spike with actual timestamp
        neuron->recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        neuron->setRefractoryPeriod(neuron->getState().refractoryPeriod);
        neuron->setFiringState(FiringState::Refractory);
        
        // Update adaptation for spike-frequency adaptation
        neuron->getState().adaptationVariable += 1.0f;
    } else {
        neuron->setFiringState(FiringState::Active);
    }
    
    // Clear synaptic input for next step
    neuron->clearTotalCurrent();
    
    return fired;
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics for event-driven simulation
    // Synapses decay over time and can be affected by neuromodulation
    if (synapse->getType() == SynapseType::Excitatory) {
        // Excitatory synapses exhibit depression with repeated activity
        float decayFactor = std::exp(-dt / 50.0f);  // Time constant of 50ms
        float weight = synapse->getWeight();
        synapse->setWeight(weight * decayFactor);
    } else {
        // Inhibitory synapses show potentiation with activity
        float potentiation = std::exp(dt / 30.0f);  // Potentiation time constant
        float weight = synapse->getWeight();
        synapse->setWeight(weight * potentiation);
    }
    
    // Synapse timing updates for spike delivery
    synapse->step(dt);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input with temporal dynamics and neuromodulation
    // Excitatory synapses cause depolarization, inhibitory cause hyperpolarization
    // Input strength depends on synapse weight, type, and recent history
    
    SynapticWeight weight = synapse->getWeight();
    
    // Modulate input strength based on neuromodulators (dopamine, acetylcholine)
    // This would integrate with the neuromodulation systems in a full implementation
    float neuromodulationFactor = 1.0f;
    
    // Apply synaptic input based on type
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight * neuromodulationFactor);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight) * neuromodulationFactor);
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
