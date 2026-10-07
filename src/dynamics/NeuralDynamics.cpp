#include "NeuralDynamics.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    // Membrane parameters
    float membraneTimeConstant;  // ms - time constant for voltage decay
    float membraneResistance;     // MOhm - input resistance
    float capacitance;           // pF - membrane capacitance
    float restingPotential;      // mV - typical -70mV
    float thresholdPotential;    // mV - typical -55mV
    float resetPotential;        // mV - typical -65mV
    
    // Synaptic parameters
    float synapticTimeConstant;   // ms - synaptic current decay
    float synapticWeightScale;    // scaling factor for synaptic weights
    float maxSynapticConductance; // mS - maximum conductance
    
    // Refractory parameters
    float refractoryPeriod;      // ms - absolute refractory period
    float relativeRefractorySlope; // slope for relative refractory
    
    // Spike timing dynamics
    std::unordered_map<NeuronId, Timestamp> lastSpikeTimes;
    std::unordered_map<SynapseId, Timestamp> lastSynapseSpikes;
    
    // Conductance-based model parameters
    float leakConductance;       // mS - baseline membrane conductance
    float maxConductance;        // mS - maximum synaptic conductance
    
    Impl() : 
        membraneTimeConstant(20.0f),   // 20ms time constant
        membraneResistance(10000.0f),   // 10 MOhm
        capacitance(250.0f),           // 250 pF
        restingPotential(-70.0f),      // -70 mV
        thresholdPotential(-55.0f),    // -55 mV
        resetPotential(-65.0f),        // -65 mV
        synapticTimeConstant(5.0f),     // 5ms synaptic time constant
        synapticWeightScale(0.1f),      // weight scaling
        maxSynapticConductance(1000.0f), // 1 mS max
        refractoryPeriod(2.0f),        // 2ms absolute refractory
        relativeRefractorySlope(0.1f),
        leakConductance(0.001f),       // baseline conductance
        maxConductance(10.0f)           // 10 mS max
    {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    float dtFloat = static_cast<float>(dt);
    const auto& state = neuron->getState();
    
    // Check if neuron is in refractory period
    if (state.refractoryRemaining > 0.0f) {
        // Refractory period: voltage stays at reset potential
        neuron->setMembranePotential(state.resetPotential);
        state.refractoryRemaining -= dtFloat;
        if (state.refractoryRemaining <= 0.0f) {
            neuron->setMembranePotential(state.resetPotential);
        }
        neuron->clearTotalCurrent();
        return;
    }
    
    // Get current state
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_total = neuron->getTotalCurrent();
    
    // Conductance-based membrane equation: C * dV/dt = -g*(V - E) + I_syn
    // where g is total conductance, E is reversal potential
    
    float totalConductance = pImpl->leakConductance;
    float reversalPotential = V_rest;
    
    // Add synaptic conductances (if we had access to synaptic currents)
    // For now, use simplified model: g_syn * (V - E_syn)
    
    // Calculate membrane current
    float membraneCurrent = totalConductance * (V - reversalPotential);
    
    // Net current: I_net = I_input - membrane_current
    float I_net = I_total - membraneCurrent;
    
    // Update voltage using Euler integration
    float dV = I_net / pImpl->capacitance * dtFloat;
    float V_new = V + dV;
    
    // Check for spike
    bool shouldFire = V_new >= state.threshold;
    
    if (shouldFire) {
        // Spike detected
        neuron->setFiringState(FiringState::Active);
        
        // Record spike time
        Timestamp spikeTime = static_cast<Timestamp>(dtFloat); // Simplified: use current timestep
        pImpl->lastSpikeTimes[neuron->getId()] = spikeTime;
        neuron->recordSpike(spikeTime);
        
        // Reset membrane potential to reset value
        neuron->setMembranePotential(state.resetPotential);
        
        // Start refractory period
        neuron->setRefractoryPeriod(pImpl->refractoryPeriod);
        
        // Apply afterhyperpolarization (AHP) current
        // AHP current reduces excitability after spike
        float ahpCurrent = -state.adaptationStrength * 0.5f;
        neuron->addToMembranePotential(ahpCurrent);
    } else {
        // No spike, update voltage normally
        neuron->setMembranePotential(V_new);
    }
    
    // Clear injected current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    float dtFloat = static_cast<float>(dt);
    
    // More sophisticated synaptic dynamics
    // Synaptic current follows alpha function: I_syn(t) = (t/tau) * exp(1 - t/tau)
    
    // Get synaptic parameters
    float weight = synapse->getWeight();
    float timeConstant = pImpl->synapticTimeConstant;
    
    // Update synaptic state (simplified alpha function)
    float synaptic efficacy = std::exp(-dtFloat / timeConstant);
    float weightChange = weight * (1.0f - efficacy);
    
    // Update synapse with new efficacy
    // This would require access to internal synapse state
    // For now, just step the synapse
    synapse->step(dtFloat);
    
    // Check for spike transmission
    if (synapse->isFiring()) {
        Timestamp spikeTime = static_cast<Timestamp>(dtFloat);
        pImpl->lastSynapseSpikes[synapse->getId()] = spikeTime;
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // Get spike time from synapse
    Timestamp spikeTime = pImpl->lastSynapseSpikes[synapse->getId()];
    
    // Calculate synaptic conductance based on spike timing
    float weight = synapse->getWeight();
    float timeSinceSpike = static_cast<float>(spikeTime); // Simplified
    
    // Exponential decay of synaptic efficacy
    float synapticEfficacy = weight * std::exp(-timeSinceSpike / pImpl->synapticTimeConstant);
    
    // Get reversal potential based on synapse type
    float reversalPotential = synapse->isExcitatory() ? 0.0f : -80.0f;
    
    // Calculate conductance-based current: I_syn = g_syn * (V - E_syn)
    float V = neuron->getMembranePotential();
    float conductance = synapticEfficacy * pImpl->synapticWeightScale;
    
    // Ensure conductance doesn't exceed maximum
    conductance = std::min(conductance, pImpl->maxConductance);
    
    // Apply synaptic current
    float synapticCurrent = conductance * (V - reversalPotential);
    
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(synapticCurrent);
    } else {
        neuron->receiveInhibitoryInput(std::abs(synapticCurrent));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    const auto& state = neuron->getState();
    
    // Check absolute refractory
    if (state.refractoryRemaining > 0.0f) {
        return false;
    }
    
    // Check threshold with some noise
    float V = neuron->getMembranePotential();
    float threshold = state.threshold;
    
    // Add some noise to threshold for biological realism
    float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f;
    threshold += noise;
    
    return V >= threshold;
}

void IntegrateAndFireDynamics::reset() {
    // Clear spike history
    pImpl->lastSpikeTimes.clear();
    pImpl->lastSynapseSpikes.clear();
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    if (tau > 0.0f) {
        pImpl->membraneTimeConstant = tau;
    }
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

// Additional parameters and methods for enhanced dynamics
void IntegrateAndFireDynamics::setMembraneResistance(float R) {
    if (R > 0.0f) {
        pImpl->membraneResistance = R;
    }
}

float IntegrateAndFireDynamics::getMembraneResistance() const {
    return pImpl->membraneResistance;
}

void IntegrateAndFireDynamics::setCapacitance(float C) {
    if (C > 0.0f) {
        pImpl->capacitance = C;
    }
}

float IntegrateAndFireDynamics::getCapacitance() const {
    return pImpl->capacitance;
}

void IntegrateAndFireDynamics::setRestingPotential(float V_rest) {
    pImpl->restingPotential = V_rest;
}

float IntegrateAndFireDynamics::getRestingPotential() const {
    return pImpl->restingPotential;
}

void IntegrateAndFireDynamics::setThresholdPotential(float V_th) {
    pImpl->thresholdPotential = V_th;
}

float IntegrateAndFireDynamics::getThresholdPotential() const {
    return pImpl->thresholdPotential;
}

void IntegrateAndFireDynamics::setResetPotential(float V_reset) {
    pImpl->resetPotential = V_reset;
}

float IntegrateAndFireDynamics::getResetPotential() const {
    return pImpl->resetPotential;
}

void IntegrateAndFireDynamics::setSynapticTimeConstant(float tau) {
    if (tau > 0.0f) {
        pImpl->synapticTimeConstant = tau;
    }
}

float IntegrateAndFireDynamics::getSynapticTimeConstant() const {
    return pImpl->synapticTimeConstant;
}

void IntegrateAndFireDynamics::setRefractoryPeriod(float period) {
    if (period >= 0.0f) {
        pImpl->refractoryPeriod = period;
    }
}

float IntegrateAndFireDynamics::getRefractoryPeriod() const {
    return pImpl->refractoryPeriod;
}

Timestamp IntegrateAndFireDynamics::getLastSpikeTime(const NeuronId& neuronId) const {
    auto it = pImpl->lastSpikeTimes.find(neuronId);
    if (it != pImpl->lastSpikeTimes.end()) {
        return it->second;
    }
    return 0;
}

float IntegrateAndFireDynamics::getSynapticEfficacy(const SynapseId& synapseId, Timestamp currentTime) const {
    auto it = pImpl->lastSynapseSpikes.find(synapseId);
    if (it != pImpl->lastSynapseSpikes.end()) {
        float timeSinceSpike = static_cast<float>(currentTime - it->second);
        return std::exp(-timeSinceSpike / pImpl->synapticTimeConstant);
    }
    return 0.0f;
}

} // namespace nlm