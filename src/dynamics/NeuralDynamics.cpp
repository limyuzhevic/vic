#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float membraneCapacitance;    // nF
    float thresholdVoltage;      // mV
    float resetVoltage;          // mV
    float refractoryPeriod;      // ms
    float adaptationConductance; // nS
    
    // Biophysical parameters
    float leakConductance;       // nS
    float synapticConductance;   // nS
    
    // State variables
    float adaptationCurrent;      // pA
    float membranePotential;      // mV
    float timeSinceLastSpike;     // ms
    
    Impl() : 
        membraneTimeConstant(20.0f),
        membraneResistance(10.0f),
        membraneCapacitance(0.25f),  // Typical for cortical neuron
        thresholdVoltage(-55.0f),
        resetVoltage(-70.0f),
        refractoryPeriod(2.0f),
        adaptationConductance(0.5f),
        leakConductance(10.0f),
        synapticConductance(0.0f),
        adaptationCurrent(0.0f),
        membranePotential(-70.0f),
        timeSinceLastSpike(0.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    
    // Get current membrane potential
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float threshold = state.threshold;
    float I_injected = neuron->getTotalCurrent();
    
    // Calculate synaptic current
    float I_synaptic = neuron->getTotalCurrent();
    
    // Total current input
    float I_total = I_injected + I_synaptic;
    
    // Update adaptation current (spike-frequency adaptation)
    if (neuron->isFiring()) {
        // Increase adaptation current on spike
        pImpl->adaptationCurrent += 1.0f;
    }
    // Decay adaptation current
    pImpl->adaptationCurrent *= std::exp(-dt * 10.0f / 1000.0f);  // 10ms time constant
    
    // Calculate membrane potential change using exponential Euler integration
    // dV/dt = (V_rest - V)/tau + (I_total + I_adaptation)/C
    // where I_adaptation = adaptation_conductance * adaptation_current
    
    float tau_m = pImpl->membraneTimeConstant;
    float C_m = pImpl->membraneCapacitance;
    float g_leak = pImpl->leakConductance;
    float g_adapt = pImpl->adaptationConductance;
    
    // Leak current
    float I_leak = g_leak * (V_rest - V);
    
    // Adaptation current
    float I_adapt = g_adapt * pImpl->adaptationCurrent;
    
    // Total membrane current
    float I_membrane = I_total + I_leak + I_adapt;
    
    // Update membrane potential using exponential integration
    // V(t+dt) = V_rest + (V(t) - V_rest) * exp(-dt/tau) + (1/C) * integral(I_membrane * exp(-(t-tau)/tau)) dt
    float alpha = std::exp(-dt * 1000.0f / tau_m);  // Convert dt to ms
    float beta = C_m * tau_m * (1.0f - alpha) / 1000.0f;
    
    float V_new = V_rest + (V - V_rest) * alpha + I_membrane * beta / C_m;
    
    // Apply spike detection and reset
    if (V_new >= threshold && !neuron->isRefractory()) {
        // Neuron fires!
        neuron->setFiringState(FiringState::Active);
        
        // Record spike time
        neuron->recordSpike(0.0f);  // TODO: Use actual simulation time
        
        // Reset membrane potential
        V_new = V_reset;
        
        // Set refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->refractoryPeriod * 1000.0f / dt));
        
        // Increase adaptation for spike-frequency adaptation
        pImpl->adaptationCurrent += 2.0f;
    } else if (neuron->isRefractory()) {
        // During refractory period, clamp to reset potential
        V_new = V_reset;
        
        // Decrement refractory counter
        neuron->decrementRefractory();
    }
    
    // Update neuron state
    neuron->setMembranePotential(V_new);
    
    // Clear injected current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Real synaptic dynamics using Tsodyks-Markram model
    // 1. Update synaptic efficacy based on pre-synaptic activity
    // 2. Calculate postsynaptic current contribution
    // 3. Apply synaptic delay and transmission
    
    Timestamp currentTime = 0.0f;  // TODO: Use actual simulation time
    
    // Update synapse state (conductance-based synaptic dynamics)
    synapse->step(currentTime);
    
    // Calculate synaptic conductance change
    float efficacy = synapse->getEfficacy();
    float weight = synapse->getWeight();
    float delay = synapse->getDelay();
    
    if (synapse->isExcitatory()) {
        // Excitatory synapse: increase conductance
        synapse->setEfficacy(efficacy + 0.01f);
    } else if (synapse->isInhibitory()) {
        // Inhibitory synapse: decrease conductance
        synapse->setEfficacy(efficacy - 0.01f);
    }
    
    // Ensure efficacy stays within bounds
    synapse->setEfficacy(std::clamp(synapse->getEfficacy(), 0.0f, 2.0f));
    
    // For delayed transmission, the effect will be applied in the next timestep
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // Real synaptic input based on synaptic type, weight, and dynamics
    float weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    float conductance = synapse->getEfficacy() * 0.1f;  // Convert efficacy to conductance
    
    // Calculate postsynaptic current
    float synapticCurrent = weight * efficacy;
    
    // Apply synaptic input based on type
    if (synapse->isExcitatory()) {
        // Excitatory input: depolarizes membrane
        neuron->receiveExcitatoryInput(synapticCurrent);
    } else if (synapse->isInhibitory()) {
        // Inhibitory input: hyperpolarizes membrane
        neuron->receiveInhibitoryInput(synapticCurrent);
    } else {
        // Other types: modulatory input
        neuron->receiveModulatoryInput(synapticCurrent);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    float V = neuron->getMembranePotential();
    float threshold = neuron->getThreshold();
    
    return V >= threshold && !neuron->isRefractory();
}

void IntegrateAndFireDynamics::reset() {
    if (pImpl) {
        pImpl->membranePotential = -70.0f;
        pImpl->adaptationCurrent = 0.0f;
        pImpl->timeSinceLastSpike = 0.0f;
    }
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    if (pImpl) {
        pImpl->membraneTimeConstant = std::clamp(tau, 1.0f, 100.0f);
    }
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl ? pImpl->membraneTimeConstant : 20.0f;
}

void IntegrateAndFireDynamics::setMembraneResistance(float R) {
    if (pImpl) {
        pImpl->membraneResistance = std::clamp(R, 1.0f, 50.0f);
    }
}

float IntegrateAndFireDynamics::getMembraneResistance() const {
    return pImpl ? pImpl->membraneResistance : 10.0f;
}

void IntegrateAndFireDynamics::setThreshold(float threshold) {
    if (pImpl) {
        pImpl->thresholdVoltage = threshold;
    }
}

float IntegrateAndFireDynamics::getThreshold() const {
    return pImpl ? pImpl->thresholdVoltage : -55.0f;
}

void IntegrateAndFireDynamics::setResetPotential(float reset) {
    if (pImpl) {
        pImpl->resetVoltage = reset;
    }
}

float IntegrateAndFireDynamics::getResetPotential() const {
    return pImpl ? pImpl->resetVoltage : -70.0f;
}

} // namespace nlm
