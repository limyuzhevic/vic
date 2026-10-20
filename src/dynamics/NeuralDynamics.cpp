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
    if (!neuron) {
        return;
    }
    
    const auto& state = neuron->getState();
    Timestep currentTime = neuron->getCurrentTime();
    
    // Get membrane properties
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float threshold = neuron->getThreshold();
    float resetPotential = state.resetPotential;
    float capacitance = state.capacitance;
    
    // Calculate total current components
    float I_leak = (V - V_rest) / pImpl->membraneResistance;  // Ohmic leak current
    float I_syn = neuron->getTotalCurrent();  // Synaptic current
    float I_adaptation = neuron->getAdaptationCurrent();  // Spike adaptation current
    
    // Total input current
    float I_total = I_syn + I_adaptation;
    
    // Membrane time constant tau_m = R * C
    float tau_m = pImpl->membraneTimeConstant;
    
    // Leaky integrate-and-fire with adaptation:
    // dV/dt = -(V - V_rest)/tau_m + (I_total - I_leak) / C
    // Simplified: dV/dt = -V/tau_m + V_rest/tau_m + I_total/C
    
    float dV_leak = -(V - V_rest) / tau_m * static_cast<float>(dt);
    float dV_input = I_total / capacitance * static_cast<float>(dt);
    
    // Add stochastic noise for biological realism (10% of input)
    float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2f * std::abs(dV_input);
    
    // Update membrane potential
    V += dV_leak + dV_input + noise;
    
    // Apply refractory period effect
    if (neuron->isRefractory()) {
        V = resetPotential;
    }
    
    // Update neuron state
    neuron->setMembranePotential(V);
    
    // Check for spike threshold crossing
    bool shouldFire = (V >= threshold) && !neuron->isRefractory();
    
    if (shouldFire) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(currentTime);
        
        // Apply spike adaptation
        neuron->updateAdaptationCurrent(currentTime);
        
        // Enter refractory period
        neuron->enterRefractoryPeriod(state.refractoryDuration);
        
        // Reset membrane potential after spike
        neuron->setMembranePotential(resetPotential);
    }
    
    // Clear synaptic and adaptation currents for next step
    neuron->clearTotalCurrent();
    neuron->resetAdaptationCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    Timestep currentTime = synapse->getCurrentTime();
    
    // Get synaptic properties
    float conductance = synapse->getConductance();
    float reversalPotential = synapse->getReversalPotential();
    float synapticTimeConstant = synapse->getTimeConstant();
    
    // Get neuron membrane potential for driving force
    float V_pre = synapse->getPreNeuronMembranePotential();
    float V_post = synapse->getPostNeuronMembranePotential();
    
    // Calculate driving force
    float drivingForce = V_post - reversalPotential;
    
    // Synaptic conductance dynamics with exponential decay
    float conductanceDecay = std::exp(-static_cast<float>(dt) / synapticTimeConstant);
    float newConductance = conductance * conductanceDecay;
    
    // Apply synaptic depression/facilitation based on recent activity
    float activityFactor = synapse->getActivityFactor();
    newConductance *= activityFactor;
    
    synapse->setConductance(newConductance);
    
    // Apply synaptic depression for high-frequency activity
    if (synapse->getRecentSpikeCount() > 0) {
        float depressionFactor = 0.95f + 0.05f * std::exp(-synapse->getRecentSpikeCount() / 10.0f);
        newConductance *= depressionFactor;
        synapse->setRecentSpikeCount(0);
    }
    
    // Update synaptic weights with STDP
    synapse->updateWeight(currentTime);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) {
        return;
    }
    
    // Get synaptic properties
    SynapticWeight weight = synapse->getWeight();
    float reversalPotential = synapse->getReversalPotential();
    bool isExcitatory = synapse->isExcitatory();
    
    // Calculate synaptic current using conductance-based model
    float V_neuron = neuron->getMembranePotential();
    float drivingForce = V_neuron - reversalPotential;
    
    // Get current synaptic conductance
    float conductance = synapse->getConductance();
    
    // Calculate synaptic current: I_syn = g_syn * (V - E_syn)
    float synapticCurrent = conductance * drivingForce;
    
    if (isExcitatory) {
        neuron->receiveExcitatoryInput(synapticCurrent);
    } else {
        neuron->receiveInhibitoryInput(std::abs(synapticCurrent));
    }
    
    // Update synaptic activity history
    synapse->recordRecentSpike();
    
    // Apply spike timing dependent plasticity (STDP)
    Timestep spikeTime = neuron->getCurrentTime();
    synapse->applySTDP(spikeTime, isExcitatory);
    
    // Update synaptic facilitation/depression
    if (isExcitatory) {
        float facilitationFactor = 1.1f;  // 10% increase per spike
        synapse->updateFacilitationFactor(facilitationFactor);
    } else {
        float depressionFactor = 0.9f;  // 10% decrease per spike
        synapse->updateDepressionFactor(depressionFactor);
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
