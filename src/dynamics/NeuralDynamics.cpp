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
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float C = R * tau;  // Membrane capacitance
    
    // Enhanced leaky integrate-and-fire with biological parameters
    // dV/dt = -(V - V_rest) / tau + I / C
    float alpha = std::exp(-static_cast<float>(dt) / tau);
    float beta = R * (1.0f - alpha) / C;
    
    // Update membrane potential with exponential integration (more stable)
    V = alpha * V + beta * I;
    neuron->setMembranePotential(V);
    
    // Adaptive threshold based on recent activity (prevents runaway excitation)
    if (state.lastSpikeTime > 0.0f) {
        float timeSinceLastSpike = static_cast<float>(state.currentTime) - state.lastSpikeTime;
        if (timeSinceLastSpike < 10.0f) {  // Within last 10ms
            // Increase threshold after recent spikes to allow recovery
            float thresholdIncrease = 0.5f * std::exp(-timeSinceLastSpike / 5.0f);
            neuron->setThreshold(state.threshold + thresholdIncrease);
        } else {
            // Slowly return to baseline threshold
            neuron->setThreshold(state.threshold);
        }
    }
    
    // Enhanced refractory mechanism with absolute and relative components
    if (neuron->isRefractory()) {
        float refractoryRemaining = neuron->getRefractoryRemaining();
        if (refractoryRemaining > 0.0f) {
            // Gradually recover from absolute refractory period
            float recovery = static_cast<float>(dt) / state.refractoryPeriod;
            refractoryRemaining = std::max(0.0f, refractoryRemaining - recovery);
            neuron->setRefractoryRemaining(refractoryRemaining);
            
            // During partial refractory period, membrane potential is clamped
            if (refractoryRemaining > 0.0f) {
                neuron->setMembranePotential(state.resetPotential);
            }
        }
    }
    
    // Check for firing with enhanced criteria
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<float>(state.currentTime));
        
        // Reset membrane potential after spike
        neuron->setMembranePotential(state.resetPotential);
        
        // Start refractory period
        neuron->setRefractoryRemaining(state.refractoryPeriod);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Enhanced synaptic dynamics with realistic time constants
    const auto& state = synapse->getState();
    
    // Synaptic dynamics with facilitation/depression based on recent activity
    float facilitation = std::exp(-static_cast<float>(dt) / 100.0f);  // 100ms time constant
    float depression = std::exp(-static_cast<float>(dt) / 50.0f);   // 50ms time constant
    
    // Update synaptic efficacy
    float efficacy = synapse->getEfficacy();
    float eligibilityTrace = synapse->getEligibilityTrace();
    
    // Simple dynamics: decay toward baseline with activity-dependent modulation
    efficacy *= facilitation;
    eligibilityTrace *= depression;
    
    // Apply short-term plasticity
    synapse->setEfficacy(efficacy);
    synapse->setEligibilityTrace(eligibilityTrace);
    
    // Synapse step function for short-term changes
    synapse->step(static_cast<float>(dt));
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Realistic synaptic input with synaptic dynamics
    SynapticWeight weight = synapse->getWeight();
    float delay = synapse->getDelay();
    float synapticTau = synapse->getSynapticTimeConstant();  // Synaptic time constant
    
    if (synapse->isExcitatory()) {
        // Excitatory input: AMPA-like dynamics
        float rise = 1.0f - std::exp(-static_cast<float>(delay) / synapticTau);
        float input = weight * rise;
        neuron->receiveExcitatoryInput(input);
    } else {
        // Inhibitory input: GABA-like dynamics with faster rise
        float rise = 1.0f - std::exp(-static_cast<float>(delay) / (synapticTau * 0.5f));
        float input = weight * rise;
        neuron->receiveInhibitoryInput(std::abs(input));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    float V_threshold = neuron->getThreshold();
    
    // Enhanced firing condition with noise to prevent synchronization
    // Add small amount of noise to prevent all neurons from firing simultaneously
    float noise = static_cast<float>(rand()) / RAND_MAX * 0.02f - 0.01f;
    
    return (V + noise) >= V_threshold && !neuron->isRefractory();
}

void IntegrateAndFireDynamics::reset() {
    // No internal state to reset (state is managed by neuron/synapse)
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
