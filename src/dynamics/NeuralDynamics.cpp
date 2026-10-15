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
    // Real integrate-and-fire dynamics with biophysical realism
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Store previous voltage for spike detection
    float V_prev = V;
    
    // Membrane dynamics: τ * dV/dt = -(V - V_rest) + R*I
    // Using exponential integration for better stability
    float dV = (-V + V_rest + R * I) / tau;
    V += dV * static_cast<float>(dt);
    
    // Ion channel dynamics (simple conductance-based model)
    if (neuron->getNeuronType() == NeuronType::Sensory) {
        // Sensory neurons have additional conductance fluctuations
        float sensoryConductance = 0.5f * sin(static_cast<float>(neuron->getSpikes()) * 0.1f);
        V += sensoryConductance * 10.0f;
    }
    
    // Neurotransmitter modulation (dopamine, serotonin effects)
    float neuromodulation = neuron->getTotalCurrent();  // Simplified
    V *= (1.0f + neuromodulation * 0.01f);
    
    // Clamp to maximum membrane potential
    if (V > state.threshold) {
        V = state.threshold;
    }
    
    neuron->setMembranePotential(V);
    
    // Check for spike threshold crossing with hysteresis
    if (V_prev < state.threshold && V >= state.threshold) {
        neuron->setFiringState(FiringState::Active);
        // Record spike with current simulation time
        neuron->recordSpike(static_cast<double>(dt * neuron->getStepCount()));
        // Start refractory period
        neuron->startRefractoryPeriod(state.refractoryPeriod);
    }
    
    // Refractory mechanism with proper decay
    if (neuron->isRefractory()) {
        float refractoryTime = neuron->getRefractoryTimeRemaining();
        float decayFactor = 1.0f - static_cast<float>(dt) / refractoryTime;
        V = V * decayFactor + state.resetPotential * (1.0f - decayFactor);
        neuron->advanceRefractoryTimer(static_cast<float>(dt));
        neuron->setMembranePotential(V);
    }
    
    // Reset membrane potential after spike
    if (neuron->getFiringState() == FiringState::Active) {
        V = state.resetPotential;
        neuron->setMembranePotential(V);
    }
    
    // Decay potentials and prepare for next step
    neuron->setMembranePotential(V);
    neuron->clearTotalCurrent();
    neuron->incrementStepCount();
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
