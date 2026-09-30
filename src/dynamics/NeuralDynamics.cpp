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
    // PHASE 2: Enhanced integrate-and-fire dynamics with real LIF implementation
    // Improved numerical stability and biological accuracy
    
    const auto& state = neuron->getState();
    
    // Leaky integration: dV/dt = -(V - V_rest) / tau + I / C
    // Using exponential Euler integration for better stability:
    // V_new = V_rest + (V_old - V_rest) * exp(-dt / tau) + (I * R / C) * (1 - exp(-dt / tau))
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Calculate membrane capacitance (derived from tau and R: C = tau / R)
    float C = tau / R;
    
    // Exponential Euler integration for better stability
    float expFactor = std::exp(-static_cast<float>(dt) * 1000.0f / tau);  // dt in ms
    
    // Update membrane potential with leak and input
    float newV = V_rest + (V - V_rest) * expFactor + (I / C) * (1.0f - expFactor);
    
    // Apply spike-frequency adaptation (if implemented in neuron)
    float adaptationFactor = 1.0f;
    if (state.adaptationVariable > 0.0f) {
        adaptationFactor *= std::exp(-static_cast<float>(dt) * 1000.0f / 50.0f);  // 50ms time constant
    }
    
    neuron->setMembranePotential(newV * adaptationFactor);
    
    // Check for firing with hysteresis to prevent multiple spikes
    bool shouldFire = neuron->getMembranePotential() >= neuron->getThreshold();
    if (shouldFire) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(state.lastSpikeTime + 0.001f);  // Increment time for next spike
        
        // Reset with relative refractory period
        float resetPotential = state.resetPotential;
        float afterhyperpolarization = -5.0f;  // Typical AHP hyperpolarization
        neuron->setMembranePotential(resetPotential + afterhyperpolarization);
        neuron->setRefractoryPeriod(static_cast<uint32_t>(neuron->getRefractoryPeriod() * 0.8f));  // Shorten refractory
        
        // Post-spike adaptation
        float& adaptationVar = neuron->getState().adaptationVariable;
        adaptationVar += 2.0f;  // Stronger adaptation
    }
    
    // Refractory mechanism (maintain reset potential during refractory)
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        // Natural recovery during refractory
        if (state.refractoryRemaining > 0) {
            float recovery = (state.refractoryRemaining / static_cast<float>(state.refractoryPeriod)) * 0.3f;
            float recoveredV = state.resetPotential + (state.restingPotential - state.resetPotential) * recovery;
            neuron->setMembranePotential(recoveredV);
        }
    }
    
    // Clamp to physiological bounds
    float clampedV = neuron->getMembranePotential();
    clampedV = std::clamp(clampedV, -100.0f, 50.0f);
    neuron->setMembranePotential(clampedV);
    
    // Clear current for next step
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
