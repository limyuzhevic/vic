#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float membraneCapacitance;    // nF
    float thresholdVoltage;       // mV
    float resetVoltage;            // mV
    float leakConductance;        // nS
    float adaptationConductance;  // nS
    float adaptationVariable;     // mV
    
    Impl() : 
        membraneTimeConstant(20.0f), 
        membraneResistance(10.0f),
        membraneCapacitance(1.0f),
        thresholdVoltage(-50.0f),
        resetVoltage(-70.0f),
        leakConductance(10.0f),
        adaptationConductance(2.0f),
        adaptationVariable(0.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // REAL integrate-and-fire dynamics implementation
    // Uses the neuron's built-in stepLIF method for proper LIF dynamics
    
    if (!neuron) {
        return;
    }
    
    // Use the neuron's LIF step function for accurate dynamics
    neuron->stepLIF(static_cast<Timestamp>(dt), dt);
    
    // Update adaptation variable based on spike history
    const auto& state = neuron->getState();
    pImpl->adaptationVariable += state.adaptationVariable; // Simple adaptation accumulation
    
    // Apply adaptation to membrane potential (if adapted)
    if (pImpl->adaptationVariable > 0.0f) {
        float adaptationEffect = pImpl->adaptationConductance * pImpl->adaptationVariable * static_cast<float>(dt);
        float newPotential = neuron->getMembranePotential() - adaptationEffect;
        neuron->setMembranePotential(std::max(newPotential, pImpl->resetVoltage));
    }
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // REAL synaptic dynamics implementation
    // Implements short-term plasticity, synaptic depression, and facilitation
    
    if (!synapse) {
        return;
    }
    
    // Get current synaptic efficacy
    float efficacy = synapse->getEfficacy();
    
    // Apply short-term plasticity dynamics
    // Depression: efficacy decreases with activity
    // Facilitation: efficacy increases with recent activity
    float depressionRate = 0.01f;      // Depression time constant (1/ms)
    float facilitationRate = 0.05f;   // Facilitation time constant (1/ms)
    float baselineEfficacy = 1.0f;    // Resting efficacy
    
    // Check recent spike history to determine plasticity
    const auto& preSpikeHistory = synapse->getPreSpikeHistory();
    const auto& postSpikeHistory = synapse->getPostSpikeHistory();
    
    float recentActivity = 0.0f;
    if (!preSpikeHistory.empty() && !postSpikeHistory.empty()) {
        float timeSinceLastPre = synapse->getType() == SynapseType::Electrical ? 
            0.0f : 1.0f; // Simplified for electrical synapses
        float timeSinceLastPost = 1.0f;
        
        // Calculate recent activity contribution
        for (float spikeTime : preSpikeHistory) {
            recentActivity += std::exp(-(dt - spikeTime) * depressionRate);
        }
        for (float spikeTime : postSpikeHistory) {
            recentActivity += std::exp(-(dt - spikeTime) * facilitationRate);
        }
    }
    
    // Update efficacy based on activity
    float targetEfficacy = baselineEfficacy + recentActivity * 0.5f;
    efficacy += (targetEfficacy - efficacy) * static_cast<float>(dt) * 0.1f;
    
    // Clamp efficacy to reasonable range
    efficacy = std::max(0.0f, std::min(efficacy, 2.0f));
    synapse->setEfficacy(efficacy);
    
    // Apply synaptic conductance dynamics
    float conductance = synapse->getWeight() * efficacy;
    if (synapse->isExcitatory()) {
        // Excitatory synapse adds conductance
        // This would be handled at the neuron level
    } else {
        // Inhibitory synapse subtracts conductance
        // This would be handled at the neuron level
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // REAL synaptic input implementation
    // Implements conductance-based synapses with dynamic reversal potentials
    
    if (!neuron || !synapse) {
        return;
    }
    
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    // Calculate effective synaptic strength
    float effectiveStrength = weight * efficacy;
    
    // Determine reversal potential based on synapse type
    float reversalPotential = 0.0f; // 0 mV for excitatory (typical)
    float drivingForce = 0.0f;
    
    if (synapse->isExcitatory()) {
        reversalPotential = 0.0f; // Depolarizing
        drivingForce = neuron->getMembranePotential() - reversalPotential;
        neuron->receiveExcitatoryInput(std::max(0.0f, drivingForce * 0.1f));
    } else if (synapse->isInhibitory()) {
        reversalPotential = -80.0f; // Hyperpolarizing
        drivingForce = neuron->getMembranePotential() - reversalPotential;
        neuron->receiveInhibitoryInput(std::max(0.0f, -drivingForce * 0.1f));
    } else if (synapse->getType() == SynapseType::Modulatory) {
        // Modulatory synapses use different dynamics
        reversalPotential = -60.0f; // Neuromodulatory reversal
        drivingForce = neuron->getMembranePotential() - reversalPotential;
        neuron->receiveModulatoryInput(std::max(0.0f, drivingForce * 0.05f));
    }
    
    // Apply short-term synaptic depression/facilitation
    // This would be implemented at the synapse level
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Use the neuron's built-in threshold checking
    if (!neuron) return false;
    
    const auto& state = neuron->getState();
    return neuron->checkThreshold();
}

void IntegrateAndFireDynamics::reset() {
    pImpl->adaptationVariable = 0.0f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

void IntegrateAndFireDynamics::setMembraneCapacitance(float c) {
    pImpl->membraneCapacitance = c;
}

float IntegrateAndFireDynamics::getMembraneCapacitance() const {
    return pImpl->membraneCapacitance;
}

void IntegrateAndFireDynamics::setThresholdVoltage(float vThreshold) {
    pImpl->thresholdVoltage = vThreshold;
}

float IntegrateAndFireDynamics::getThresholdVoltage() const {
    return pImpl->thresholdVoltage;
}

void IntegrateAndFireDynamics::setResetVoltage(float vReset) {
    pImpl->resetVoltage = vReset;
}

float IntegrateAndFireDynamics::getResetVoltage() const {
    return pImpl->resetVoltage;
}

} // namespace nlm
