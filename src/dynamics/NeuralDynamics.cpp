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
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    // Enhanced with realistic LIF dynamics and spike frequency adaptation
    
    const auto& state = neuron->getState();
    
    // Leaky integration: dV/dt = -(V - V_rest) / tau + I / C
    // For simplicity using explicit Euler:
    // V_new = V_old + dt * (-(V_old - V_rest) / tau + I / R)
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Enhanced integration with neuromodulation factors
    float leak = -(V - V_rest) / tau;
    
    // Apply neuromodulatory effects on membrane resistance
    float modFactor = 1.0f;
    if (neuron->getModulatorLevel() > 0.0f) {
        modFactor *= (1.0f + neuron->getModulatorLevel() * 0.5f);
    }
    
    float synapticCurrent = I / R * modFactor;
    
    // Adaptive threshold: increases with firing rate, decreases with hyperpolarization
    float threshold = neuron->getThreshold();
    if (state.firingRate > 0.0f) {
        // Spike frequency adaptation
        float adaptation = state.firingRate * 0.5f;
        threshold += adaptation * static_cast<float>(dt);
    }
    
    // Reset adapted threshold if neuron hyperpolarizes
    if (V < state.restingPotential) {
        threshold = std::max(threshold - 1.0f * static_cast<float>(dt), state.resetPotential);
    }
    
    // Updated threshold with membrane dynamics
    neuron->setThreshold(threshold);
    
    // Update membrane potential
    float dV = (leak + synapticCurrent) * static_cast<float>(dt);
    float newV = V + dV;
    
    // Apply biophysical constraints
    newV = std::max(newV, state.resetPotential);  // Minimum voltage
    newV = std::min(newV, 50.0f);  // Maximum voltage (more realistic than typical -70 to 40)
    
    neuron->setMembranePotential(newV);
    
    // Check for firing with refractory period consideration
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
        
        // Apply synaptic effects of spike
        neuron->setMembranePotential(state.resetPotential);  // Reset voltage
        
        // Increase firing rate adaptation
        if (state.firingRate > 0.0f) {
            float adaptationIncrease = state.firingRate * 0.8f;
            // Note: Firing rate is stored in state, handled by neuron class
        }
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        
        // Decrease refractoriness
        float refracctionDecrease = state.refractoryRemaining * 0.01f * static_cast<float>(dt);
        if (state.refractoryRemaining > 0.0f) {
            neuron->setRefractoryRemaining(std::max(0.0f, state.refractoryRemaining - refracctionDecrease));
        }
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // TODO PHASE 2: Implement real synaptic dynamics
    // Enhanced with synaptic adaptation and neuromodulation
    
    if (!synapse) return;
    
    // Basic synaptic dynamics with more realistic modeling
    float weight = synapse->getWeight();
    
    // Apply neuromodulatory effects on synaptic strength
    float neuromodulatoryEffect = 1.0f;
    float modulatorLevel = synapse->getModulatorLevel();
    if (modulatorLevel > 0.0f) {
        // Different neuromodulators have different effects
        neuromodulatoryEffect *= (1.0f + modulatorLevel * 0.3f);
    }
    
    // Apply synaptic dynamics with more sophisticated modeling:
    // 1. Short-term plasticity (vesicle depletion, facilitation)
    // 2. Synaptic depression
    // 3. Activity-dependent plasticity
    
    // Store current state
    float eligibility = synapse->getEligibilityTrace();
    float efficacy = synapse->getEfficacy();
    
    // Update eligibility trace (representing synaptic modification potential)
    float eligibilityDecay = 0.95f;  // Decay rate per time step
    eligibility *= eligibilityDecay;
    
    // Apply neuromodulatory effects on eligibility trace
    if (modulatorLevel > 0.0f) {
        eligibility *= (1.0f + modulatorLevel * 0.5f);
    }
    
    synapse->setEligibilityTrace(eligibility);
    
    // Update synaptic efficacy with homeostatic mechanisms
    float homeostasisTarget = 0.5f;  // Target weight
    float homeostaticForce = 0.001f;  // Rate of homeostatic adjustment
    
    // Apply homeostatic plasticity to prevent runaway excitation/inhibition
    if (efficacy > homeostasisTarget) {
        efficacy -= homeostaticForce * static_cast<float>(dt);
    } else if (efficacy < homeostasisTarget) {
        efficacy += homeostaticForce * static_cast<float>(dt);
    }
    
    // Clamp to reasonable range
    efficacy = std::max(0.0f, std::min(efficacy, 2.0f));
    
    // Apply short-term plasticity if enabled
    if (synapse->getShortTermPlasticityEnabled()) {
        float u = synapse->getUtilization();  // Current utilization (0-1)
        float U = synapse->getU();  // Maximum utilization
        float tau_f = synapse->getFacilitationTimeConstant();  // Facilitation time constant
        float tau_d = synapse->getDepressionTimeConstant();   // Depression time constant
        
        // Update utilization
        if (u < U) {
            u += (1.0f - u) * (1.0f - std::exp(-static_cast<float>(dt) / tau_f));
        }
        
        // Apply depression after spike
        if (synapse->getLastSpikeTime() > 0.0f) {
            u *= (1.0f - std::exp(-static_cast<float>(dt) / tau_d));
        }
        
        synapse->setUtilization(u);
        
        // Scale weight by utilization
        weight *= u;
    }
    
    synapse->setEfficacy(efficacy);
    synapse->step(static_cast<float>(dt));
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // TODO PHASE 2: Implement real synaptic input
    // Enhanced with synaptic dynamics and neuromodulation
    
    if (!neuron || !synapse) return;
    
    SynapticWeight weight = synapse->getWeight();
    bool isExcitatory = synapse->isExcitatory();
    
    // Calculate synaptic efficacy with multiple factors
    float efficacy = synapse->getEfficacy();
    float utilization = synapse->getUtilization();
    
    // Apply neuromodulatory effects
    float modulatorLevel = synapse->getModulatorLevel();
    float neuromodulatoryEffect = 1.0f;
    if (modulatorLevel > 0.0f) {
        // Excitatory synapses are enhanced by certain neuromodulators
        // Inhibitory synapses are suppressed by others
        neuromodulatoryEffect *= (1.0f + modulatorLevel * (isExcitatory ? 0.5f : -0.3f));
    }
    
    // Calculate effective synaptic strength
    float effectiveWeight = weight * efficacy * utilization * neuromodulatoryEffect;
    
    // Temporal dynamics - synaptic response depends on recent activity
    float timeSinceLastSpike = 0.0f;
    if (synapse->getLastSpikeTime() > 0.0f) {
        // Time since last spike affects synaptic response
        // Recent spikes lead to depression (short-term depression)
        timeSinceLastSpike = static_cast<float>(synapse->getTimeSinceLastSpike());
        float depressionFactor = 1.0f / (1.0f + timeSinceLastSpike * 0.1f);
        effectiveWeight *= depressionFactor;
    }
    
    // Apply the synaptic input
    if (isExcitatory) {
        // Enhanced excitatory input
        float excitatoryInput = effectiveWeight * 10.0f;  // Scale factor for LIF neurons
        neuron->receiveExcitatoryInput(excitatoryInput);
    } else {
        // Enhanced inhibitory input
        float inhibitoryInput = std::abs(effectiveWeight) * 10.0f;  // Scale factor
        neuron->receiveInhibitoryInput(inhibitoryInput);
    }
    
    // Record synaptic activity for plasticity
    synapse->recordPostSpike(0.0);  // TODO: pass actual time
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
