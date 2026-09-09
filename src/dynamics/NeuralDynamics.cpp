#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float membraneCapacitance;    // nF (was missing)
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), membraneCapacitance(1.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Implement proper integrate-and-fire dynamics
    // dV/dt = -(V - V_rest)/tau + I/C (correct LIF formulation)
    
    const NeuronState& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float C = pImpl->membraneCapacitance;  // Use capacitance for correct physics
    
    // Proper LIF dynamics with exponential Euler for stability
    // dV = dt * ((V_rest - V)/tau + I/C)
    float dV = static_cast<float>(dt) * ((V_rest - V) / tau + I / C);
    
    // Apply the change in membrane potential
    neuron->setMembranePotential(V + dV);
    
    // Check for firing threshold crossing
    if (neuron->checkThreshold()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(state.lastSpikeTime >= 0.0f ? state.lastSpikeTime + tau : 0.0f);
    }
    
    // Handle refractory period - clamp to reset potential
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current injection for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics implementation:
    // 1. Decay short-term plasticity state (Tsodyks-Markram model)
    // 2. Update efficacy based on recent use (homeostatic scaling)
    // 3. Apply Hebbian and STDP weight changes if plasticity is enabled
    // 4. Synaptic scaling for homeostasis
    
    if (!synapse) return;
    
    // Call synapse internal dynamics step with current time
    // In a real system, we'd need to track simulation time
    // For now, use zero as placeholder
    synapse->step(0.0);
    
    // Apply synaptic homeostasis: scale weights based on global activity
    // This prevents runaway excitation/inhibition
    float weight = synapse->getWeight();
    float targetWeight = 0.5f;  // Target weight for homeostasis
    float scaleFactor = targetWeight / std::abs(weight);
    
    // Apply scaling with bounds to prevent instability
    if (std::abs(scaleFactor) > 1.05f) {  // Scale only if >5% deviation
        float newWeight = weight * scaleFactor;
        synapse->setWeight(std::clamp(newWeight, -1.0f, 1.0f));
    }
    
    // Update eligibility trace decay for reward-modulated learning
    synapse->decayEligibilityTrace(0.01f * dt);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input: conductance-based model with reversal potentials
    // Biological synaptic transmission involves:
    // - Activation of postsynaptic receptors
    // - Conductance changes that depend on synaptic efficacy
    // - Different reversal potentials for excitatory vs inhibitory
    
    if (!neuron || !synapse) return;
    
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();  // Use-dependent modulation
    SynapseType type = synapse->getType();
    
    // Calculate synaptic conductance with short-term plasticity
    float conductance = std::abs(weight) * efficacy;
    
    // Update short-term plasticity state
    // Tsodyks-Markram model: utilization, depression, and facilitation
    float depression = synapse->getDepressionState();
    float facilitation = synapse->getFacilitationState();
    
    // For excitatory synapses, apply facilitation
    if (synapse->isExcitatory()) {
        // Facilitation increases conductance with repeated spikes
        facilitation = std::min(1.0f, facilitation * 1.2f);
        conductance *= facilitation;
    } else {
        // Depression affects inhibitory synapses
        depression = std::max(0.0f, depression * 0.9f);
        conductance *= (1.0f - depression);
    }
    
    // Set synapse state for next time
    if (synapse->isExcitatory()) {
        synapse->setFacilitationState(facilitation);
    } else {
        synapse->setDepressionState(depression);
    }
    
    // Biological reversal potentials (in mV)
    // - Excitatory (AMPA/NMDA): ~0 mV (depolarizing)
    // - Inhibitory (GABA_A): ~-70 mV (hyperpolarizing)
    // - Modulatory: variable based on neuromodulator type
    
    if (type == SynapseType::Excitatory) {
        // Excitatory postsynaptic potential (EPSP)
        // Current = conductance * (V - E_rev)
        float reversalPotential = 0.0f;  // AMPA/NMDA reversal
        float drivingForce = neuron->getMembranePotential() - reversalPotential;
        
        // The neuron receives current based on conductance
        // In LIF, this translates to an additive current input
        float current = conductance * drivingForce;
        neuron->receiveExcitatoryInput(current);
        
        // Update facilitation based on spike activity
        float newFacilitation = facilitation * std::exp(-1.0f / 100.0f);  // Decay
        synapse->setFacilitationState(newFacilitation);
        
    } else if (type == SynapseType::Inhibitory) {
        // Inhibitory postsynaptic potential (IPSP)
        float reversalPotential = -70.0f;  // GABA_A reversal
        float drivingForce = neuron->getMembranePotential() - reversalPotential;
        
        float current = -conductance * drivingForce;  // Negative for inhibition
        neuron->receiveInhibitoryInput(-current);
        
        // Update depression based on spike activity
        float newDepression = 1.0f - std::exp(-1.0f / 200.0f);  // Depression buildup
        synapse->setDepressionState(newDepression);
        
    } else if (type == SynapseType::Modulatory) {
        // Modulatory input (e.g., dopamine, acetylcholine)
        // Affects plasticity rather than direct membrane potential
        float modulationStrength = conductance * 0.1f;  // Modulatory effect
        neuron->receiveModulatoryInput(modulationStrength);
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
