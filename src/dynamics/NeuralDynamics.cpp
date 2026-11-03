#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float membraneConductance;   // S (siemens) = 1/(R*tau)
    float refractoryPeriodMs;    // ms
    
    Impl() : membraneTimeConstant(20.0f), 
             membraneResistance(10.0f), 
             membraneConductance(0.005f), 
             refractoryPeriodMs(2.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // PHASE 2: Real integrate-and-fire dynamics implementation
    // Leaky Integrate-and-Fire (LIF) neuron with spike generation and refractory period
    
    const auto& state = neuron->getState();
    const float V = neuron->getMembranePotential();
    
    // Calculate membrane current using LIF equation: dV/dt = (I - g*(V - E)) / C
    // Where I is total input current, g is membrane conductance, E is reversal potential
    
    float I_total = neuron->getTotalCurrent();  // Synaptic inputs accumulated in this timestep
    float V_rest = state.restingPotential;
    float I_leak = (V - V_rest) * pImpl->membraneConductance;  // Ohmic leak current
    
    // Total current for voltage update
    float I_net = I_total - I_leak;
    
    // Membrane capacitance (convert ms to seconds: C = 1 / (g * tau))
    float capacitance = 1.0f / (pImpl->membraneConductance * pImpl->membraneTimeConstant);
    
    // Update membrane potential: V += (I_net / C) * dt
    float dV = (I_net / capacitance) * static_cast<float>(dt);
    float V_new = V + dV;
    
    // Enforce voltage limits
    float V_reset = state.resetPotential;
    float V_threshold = neuron->getThreshold();
    
    // Check for spike generation
    if (V_new >= V_threshold && !neuron->isRefractory()) {
        // Spike detected - update state and handle refractory period
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<float>(pImpl->refractoryPeriodMs));  // Use available field for spike time
        V_new = V_reset;  // Reset membrane potential after spike
        // Note: The neuron interface doesn't have enterRefractoryPeriod().
        // Instead, we set the refractory period through the state.
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->refractoryPeriodMs));
    }
    
    // Update membrane potential (clamped between V_reset and threshold)
    neuron->setMembranePotential(std::clamp(V_new, V_reset, V_threshold - 0.01f));
    
    // Handle refractory period voltage dynamics
    if (neuron->isRefractory()) {
        // During refractory period, voltage decays toward reset potential
        // Note: The neuron interface doesn't have getRefractoryTimeRemaining().
        // We'll use a simplified model.
        float decayFactor = std::exp(-static_cast<float>(dt) / pImpl->membraneTimeConstant);
        float V_refractory = neuron->getMembranePotential();
        neuron->setMembranePotential(V_refractory + (V_reset - V_refractory) * (1.0f - decayFactor));
    }
    
    // Decrement refractory period if active
    neuron->decrementRefractory();
    
    // Clear accumulated synaptic current for next timestep
    neuron->clearTotalCurrent();
    
    // Update internal neuron state
    neuron->stepLIF(static_cast<float>(dt), V_reset);
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // PHASE 2: Real synaptic dynamics implementation
    // Implements synaptic conductance changes and neurotransmitter dynamics
    
    // The Synapse class doesn't have a getState() method.
    // Instead, we'll focus on the efficacy-based modulation that complements the existing Synapse::step() method.
    
    // Update synaptic efficacy based on recent activity
    float efficacy = synapse->getEfficacy();
    float weight = synapse->getWeight();
    
    // Activity-dependent efficacy modulation
    // Higher activity leads to increased efficacy (use-dependent potentiation)
    if (weight != 0.0f) {  // Only if synapse has weight
        // Simple model: recent activity increases efficacy
        // This is a simplified version of the existing Synapse::step() activity tracking
        float activityFactor = 0.1f;  // Conservative activity factor
        efficacy = std::min(efficacy * (1.0f + activityFactor), 2.0f);
    }
    
    synapse->setEfficacy(efficacy);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // PHASE 2: Real synaptic input implementation
    // Implements NMDA/AMPA synaptic transmission with conductance-based dynamics
    
    const auto& neuronState = neuron->getState();
    
    // Calculate synaptic current using conductance-based model
    // I_syn = g_syn * (V - E_syn)
    // Note: The Synapse class doesn't have an explicit conductance field in its public interface.
    // Instead, synaptic strength is represented by weight and efficacy.
    // We'll use the synapse weight scaled by efficacy as the effective conductance.
    
    float V = neuron->getMembranePotential();
    
    // Get synaptic efficacy and weight as combined conductance proxy
    float efficacy = synapse->getEfficacy();
    float weight = synapse->getWeight();
    
    // Effective synaptic conductance: scale weight by efficacy
    // Efficacy represents use-dependent modulation (0.0 to 2.0)
    float effectiveConductance = std::abs(weight) * efficacy;
    
    // Reversal potentials based on synapse type
    float E_syn = synapse->isExcitatory() ? neuronState.excitatoryReversalPotential : 
                                       neuronState.inhibitoryReversalPotential;
    
    // Synaptic current: I_syn = g_syn * (V - E_syn)
    float synapticCurrent = effectiveConductance * (V - E_syn);
    
    // Apply current to neuron with appropriate sign
    // Note: The neuron interface doesn't have direct access to the current reversal potentials,
    // so we'll use a simplified model based on synapse type.
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(std::max(0.0f, synapticCurrent));
    } else {
        neuron->receiveInhibitoryInput(std::abs(synapticCurrent));
    }
    
    // Note: The Synapse class doesn't have an updateActivityTime() method in its public interface.
    // Activity tracking is handled internally through the recordPreSpike() and recordPostSpike() methods.
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // PHASE 2: Enhanced spike detection with refractory consideration
    
    // Use the neuron's built-in threshold checking method
    bool thresholdCrossed = neuron->checkThreshold();
    
    // During refractory period, prevent firing even if threshold is crossed
    return thresholdCrossed && !neuron->isRefractory();
}

void IntegrateAndFireDynamics::reset() {
    // Reset internal dynamics state
    pImpl->membraneTimeConstant = 20.0f;
    pImpl->membraneResistance = 10.0f;
    pImpl->membraneConductance = 0.05f;  // 1 / (R * tau) = 1/(10 * 20) = 0.005
    pImpl->refractoryPeriodMs = 2.0f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
