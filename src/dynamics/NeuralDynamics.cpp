#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    Timestamp currentTime;        // Current simulation time
    
    // NMDA/AMPA receptor dynamics
    float nmdaConductance;        // NMDA receptor conductance (nS)
    float ampaConductance;        // AMPA receptor conductance (nS)
    float nmdaReversalPotential;   // NMDA reversal potential (mV)
    float ampaReversalPotential;   // AMPA reversal potential (mV)
    
    // Short-term plasticity parameters
    float depressionRate;         // Depression rate constant
    float facilitationRate;       // Facilitation rate constant
    float utilization;             // Max utilization factor
    float shortTermDepression;    // Current depression state
    float shortTermFacilitation;  // Current facilitation state
    
    // Synaptic input state
    float synapticConductance;    // Total synaptic conductance
    float synapticCurrent;        // Total synaptic current
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), currentTime(0.0f),
             nmdaConductance(50.0f), ampaConductance(100.0f),
             nmdaReversalPotential(0.0f), ampaReversalPotential(0.0f),
             depressionRate(0.05f), facilitationRate(1.0f), utilization(0.5f),
             shortTermDepression(1.0f), shortTermFacilitation(0.0f),
             synapticConductance(0.0f), synapticCurrent(0.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real integrate-and-fire dynamics with spike-frequency adaptation
    const auto& state = neuron->getState();
    
    // Get current membrane potential and synaptic inputs
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float C = state.leakConductance;  // Membrane capacitance approximation
    
    // Convert current to voltage change: dV = I * R
    float synapticVoltage = I * R;
    
    // Leaky integration using exponential Euler method (more stable)
    // V(t+dt) = V_rest + (V(t) - V_rest) * exp(-dt/tau) + synapticVoltage * (1 - exp(-dt/tau))
    float decayFactor = std::exp(-static_cast<float>(dt) / tau);
    float V_new = V_rest + (V - V_rest) * decayFactor + synapticVoltage * (1.0f - decayFactor);
    
    // Apply spike-frequency adaptation
    if (state.adaptationVariable > 0.0f) {
        V_new -= state.adaptationVariable * 0.01f;
        state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V_new = std::clamp(V_new, -100.0f, 50.0f);
    
    neuron->setMembranePotential(V_new);
    
    // Check for firing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        // Record spike with actual time
        neuron->recordSpike(static_cast<float>(pImpl->currentTime));
        // Enter refractory period
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        // Increment adaptation variable for spike-frequency adaptation
        state.adaptationVariable += 1.0f;
        // Reset membrane potential
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with NMDA/AMPA receptor models and short-term plasticity
    TimestepDuration currentTime = pImpl->currentTime;
    
    // Get synaptic parameters
    float weight = synapse->getWeight();
    SynapseType type = synapse->getType();
    float efficacy = synapse->getEfficacy();
    
    // Update short-term plasticity (Tsodyks-Markram model)
    float depression = pImpl->shortTermDepression;
    float facilitation = pImpl->shortTermFacilitation;
    float utilization = pImpl->utilization;
    
    // Update synaptic state based on pre- and post-synaptic activity
    if (synapse->getLastPreSpikeTime() >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - synapse->getLastPreSpikeTime());
        
        // Depression recovers over time
        depression += (1.0f - depression) * (1.0f - std::exp(-timeSincePre / pImpl->depressionRate));
        
        // Facilitation decays over time
        if (type == SynapseType::Excitatory || type == SynapseType::Inhibitory) {
            facilitation *= std::exp(-timeSincePre / pImpl->facilitationRate);
        }
    }
    
    // Store updated short-term plasticity state
    pImpl->shortTermDepression = depression;
    pImpl->shortTermFacilitation = facilitation;
    
    // Calculate effective conductance based on receptor types
    float effectiveConductance = 0.0f;
    if (type == SynapseType::Excitatory) {
        // AMPA fast component
        effectiveConductance += pImpl->ampaConductance * efficacy;
        // NMDA slow component (Mg2+ block modeled)
        float nmdaMgBlock = 1.0f / (1.0f + std::exp(0.062f * (-V + 60.0f)));
        effectiveConductance += pImpl->nmdaConductance * efficacy * nmdaMgBlock;
    } else if (type == SynapseType::Inhibitory) {
        effectiveConductance += pImpl->ampaConductance * efficacy * 0.5f;
    }
    
    // Update synaptic conductance
    pImpl->synapticConductance = effectiveConductance * depression;
    
    // Calculate synaptic current
    pImpl->synapticCurrent = pImpl->synapticConductance * (getReversalPotential(type) - V);
    
    // Apply synaptic dynamics
    synapse->setWeight(weight);
    synapse->setEfficacy(efficacy);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input with NMDA/AMPA receptor dynamics
    float weight = synapse->getWeight();
    SynapseType type = synapse->getType();
    float efficacy = synapse->getEfficacy();
    
    // Get current membrane potential
    float V = neuron->getMembranePotential();
    
    // Calculate synaptic current based on receptor dynamics
    float reversalPotential = getReversalPotential(type);
    float drivingForce = reversalPotential - V;
    
    // NMDA receptors have voltage-dependent magnesium block
    if (type == SynapseType::Excitatory) {
        float nmdaMgBlock = 1.0f / (1.0f + std::exp(0.062f * (-V + 60.0f)));
        drivingForce *= nmdaMgBlock;
    }
    
    // Apply short-term plasticity modulation
    float utilization = pImpl->utilization;
    float depression = pImpl->shortTermDepression;
    float facilitation = pImpl->shortTermFacilitation;
    
    float effectiveWeight = weight * efficacy * utilization * depression * facilitation;
    
    // Calculate synaptic current
    float synapticCurrent = effectiveWeight * drivingForce;
    
    // Apply to neuron based on type
    if (type == SynapseType::Excitatory) {
        neuron->receiveExcitatoryInput(synapticCurrent);
    } else if (type == SynapseType::Inhibitory) {
        neuron->receiveInhibitoryInput(-synapticCurrent);
    } else {
        neuron->receiveModulatoryInput(synapticCurrent);
    }
}

float IntegrateAndFireDynamics::getReversalPotential(SynapseType type) const {
    switch (type) {
        case SynapseType::Excitatory:
            return pImpl->ampaReversalPotential;
        case SynapseType::Inhibitory:
            return -70.0f;  // GABA reversal potential
        case SynapseType::Modulatory:
            return -60.0f;  // AMPA reversal potential
        case SynapseType::Electrical:
            return -70.0f;  // Similar to resting potential
        case SynapseType::GapJunction:
            return -70.0f;  // Electrical synapse
        default:
            return 0.0f;
    }
}

void IntegrateAndFireDynamics::reset() {
    // Reset all synaptic dynamics state
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->synapticConductance = 0.0f;
    pImpl->synapticCurrent = 0.0f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
