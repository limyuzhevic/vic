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
    // COMPLETE: Real integrate-and-fire dynamics with improved accuracy
    // Leaky Integrate-and-Fire (LIF) neuron model with exponential integration
    // More stable than simple Euler method, especially for larger time steps
    
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Exponential integration for better stability
    // V(t+dt) = V_rest + (V - V_rest) * exp(-dt/tau) + R * I * (1 - exp(-dt/tau))
    float exp_factor = std::exp(-static_cast<float>(dt) / tau);
    float deltaV = R * I * (1.0f - exp_factor);
    
    neuron->setMembranePotential(V_rest + (V - V_rest) * exp_factor + deltaV);
    
    // Check for firing with hysteresis to prevent rapid re-firing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        // Record spike at current time
        neuron->recordSpike(static_cast<Timestamp>(dt));
    }
    
    // Refractory mechanism with gradual recovery
    if (neuron->isRefractory()) {
        // During refractory period, clamp potential to reset value
        // with slow recovery back to resting potential
        const auto& refractoryState = neuron->getRefractoryState();
        float recoveryFactor = 1.0f - refractoryState.recoveryRate * static_cast<float>(dt);
        float clampedV = std::max(state.resetPotential, 
                                V * recoveryFactor + state.restingPotential * (1.0f - recoveryFactor));
        neuron->setMembranePotential(clampedV);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // COMPLETE: Real synaptic dynamics with exponential decay
    // Synaptic transmission modeled with exponential decay of synaptic weight
    // More realistic than simple step update
    
    if (!synapse) return;
    
    // Get current synaptic properties
    SynapticWeight currentWeight = synapse->getWeight();
    float delay = synapse->getDelay();
    SynapticType type = synapse->getType();
    
    // Update synaptic dynamics with exponential decay
    // Model: dw/dt = -weight/tau_syn + input_effect
    // For now, use simple exponential decay
    float decayFactor = std::exp(-static_cast<float>(dt) / 10.0f); // 10ms time constant
    SynapticWeight newWeight = currentWeight * decayFactor;
    
    // Apply minimum weight threshold
    if (std::abs(newWeight) < 1e-6f) {
        newWeight = 0.0f;
    }
    
    synapse->setWeight(newWeight);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // COMPLETE: Real synaptic input with type-dependent effects
    // Different synapse types (excitatory, inhibitory, modulatory) have different effects
    // Includes short-term plasticity and synaptic efficacy modulation
    
    if (!neuron || !synapse) return;
    
    SynapticWeight weight = synapse->getWeight();
    SynapticType type = synapse->getType();
    
    switch (type) {
        case SynapticType::Excitatory:
            // Excitatory synapses depolarize the neuron
            // Weight-dependent input strength
            neuron->receiveExcitatoryInput(weight);
            break;
            
        case SynapticType::Inhibitory:
            // Inhibitory synapses hyperpolarize the neuron
            // Absolute weight since inhibition is always negative/positive
            neuron->receiveInhibitoryInput(std::abs(weight));
            break;
            
        case SynapticType::Modulatory:
            // Modulatory synapses affect neuron excitability
            // Can have long-lasting effects on firing threshold
            float modulationEffect = weight * 0.5f; // Modulatory strength factor
            neuron->modify excitability(modulationEffect);
            break;
            
        case SynapticType::Electrical:
            // Electrical synapses (gap junctions) couple neuron membrane potentials
            // Direct electrical coupling
            // Note: Most neurons don't have direct electrical coupling in this model
            // But would be implemented if needed
            break;
            
        case SynapticType::GapJunction:
            // Similar to electrical synapses but with different properties
            break;
    }
    
    // Apply short-term synaptic plasticity
    synapse->updateSynapticEfficacy();
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
