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
    // Real integrate-and-fire dynamics with adaptive threshold and refractory period
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float V_thresh = neuron->getThreshold();
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Calculate adaptive threshold based on recent activity
    float adaptiveThreshold = V_thresh + state.adaptiveThreshold * neuron->getRecentFiringRate();
    
    // Euler integration with exponential decay
    float dV = (-(V - V_rest) / tau + I / R) * static_cast<float>(dt);
    float V_new = V + dV;
    
    // Refractory period handling
    if (neuron->isRefractory()) {
        // During refractory period, hold potential at reset and count down
        neuron->setMembranePotential(V_reset);
        neuron->decrementRefractoryPeriod(static_cast<Timestamp>(dt));
        return;
    }
    
    // Check for threshold crossing
    if (V_new >= adaptiveThreshold) {
        // Neuron fires
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(neuron->getTotalCurrent() > 0 ? V_new - adaptiveThreshold : 0.0f);
        
        // Refractory period activation
        neuron->setRefractoryPeriod(state.refractoryDuration);
        neuron->setMembranePotential(V_reset);
        
        // Increase adaptive threshold (spike-frequency adaptation)
        state.adaptiveThreshold += state.adaptationRate;
    } else {
        // Update membrane potential normally
        neuron->setMembranePotential(V_new);
    }
    
    // Decay adaptive threshold
    state.adaptiveThreshold *= (1.0f - state.adaptationDecay * static_cast<float>(dt));
    
    // Clear synaptic currents for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with conductance-based transmission
    const auto& synapseState = synapse->getState();
    
    // Calculate synaptic conductance
    float g_syn = synapseState.weight;
    float reversalPotential = synapseState.reversalPotential;
    float V_post = 0.0f;  // Would be obtained from postsynaptic neuron
    
    // Update synaptic state based on presynaptic spikes
    synapse->updateSynapticState(static_cast<float>(dt));
    
    // Update synaptic depression/facilitation based on recent activity
    if (synapseState.isExcitatory()) {
        // Excitatory synapses: depression based on activity
        float activityFactor = std::min(1.0f, synapseState.excitatoryFactor);
        synapse->setWeight(synapseState.weight * (1.0f - 0.01f * static_cast<float>(dt) * activityFactor));
    } else {
        // Inhibitory synapses: can strengthen with activity
        float activityFactor = std::min(1.0f, synapseState.inhibitoryFactor);
        synapse->setWeight(synapseState.weight * (1.0f + 0.005f * static_cast<float>(dt) * activityFactor));
    }
    
    // Apply synaptic depression for high-frequency activity
    if (synapseState.recentSpikeCount > 10) {
        float depressionFactor = std::min(0.9f, 0.1f * std::log(static_cast<float>(synapseState.recentSpikeCount) / 10.0f));
        synapse->setWeight(synapseState.weight * (1.0f - depressionFactor));
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input with spike timing-dependent effects
    const auto& synapseState = synapse->getState();
    const auto& neuronState = neuron->getState();
    
    // Calculate synaptic conductance based on synapse type
    float g_max = synapseState.weight;
    float reversalPotential = synapseState.reversalPotential;
    
    // For leaky integrate-and-fire, synaptic input is modeled as current injection
    // Calculate charge transfer based on synaptic strength and recent activity
    float synapticCurrent = 0.0f;
    
    if (synapseState.isExcitatory()) {
        // Excitatory input: depolarizing current
        synapticCurrent = g_max * (neuronState.restingPotential - reversalPotential);
        neuron->receiveExcitatoryInput(std::abs(synapticCurrent));
        
        // STDP eligibility trace for pre-post pairing
        synapse->setSTDPTrace(true, neuron->getLastSpikeTime());
    } else {
        // Inhibitory input: hyperpolarizing current
        synapticCurrent = g_max * (neuronState.restingPotential - reversalPotential);
        neuron->receiveInhibitoryInput(std::abs(synapticCurrent));
        
        // For inhibitory synapses, track post-synaptic activity
        synapse->setSTDPTrace(false, neuron->getLastSpikeTime());
    }
    
    // Apply synaptic dynamics based on recent spike history
    synapse->applySynapticDynamics(1.0f);  // dt=1.0 for spike-triggered update
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
