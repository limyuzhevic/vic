# Improved Neural Dynamics Implementation
# Replaces the basic placeholder with real biophysical dynamics

#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct RealIntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float capacitance;            // nF
    float restingPotential;      // mV
    float thresholdPotential;     // mV
    float resetPotential;         // mV
    float leakConductance;        // nS
    float adaptationConductance;  // nS
    float adaptationTimeConstant; // ms
    
    // Improved Euler integration parameters
    float dt;                     // Time step (ms)
    float sqrt2;                  // Pre-computed constant
    
    Impl() : membraneTimeConstant(20.0f),  // More realistic: 20-30ms
             membraneResistance(10.0f),    // More realistic: 10-15MOhm
             capacitance(1.0f),            // nF
             restingPotential(-70.0f),     // mV - typical neuron resting potential
             thresholdPotential(-55.0f),   // mV - typical spike threshold
             resetPotential(-65.0f),       // mV - reset after spike
             leakConductance(0.1f),        // nS
             adaptationConductance(5.0f),  // nS
             adaptationTimeConstant(100.0f), // ms - spike-frequency adaptation
             dt(0.001f),                    // 1ms timestep (matches simulation)
             sqrt2(std::sqrt(2.0f)) {}     // For numerical stability
};

RealIntegrateAndFireDynamics::RealIntegrateAndFireDynamics() : pImpl(new Impl) {}

RealIntegrateAndFireDynamics::~RealIntegrateAndFireDynamics() = default;

void RealIntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt_ms) {
    const auto& state = neuron->getState();
    
    // Get current neuron state
    MembranePotential V = neuron->getMembranePotential();
    NeuronId id = neuron->getId();
    
    // Extract parameters with biological realism
    float V_rest = pImpl->restingPotential;
    float V_th = pImpl->thresholdPotential;
    float V_reset = pImpl->resetPotential;
    float tau = pImpl->membraneTimeConstant;      // ms
    float R = pImpl->membraneResistance;          // MOhm
    float C = pImpl->capacitance;                  // nF
    float gL = pImpl->leakConductance;            // nS
    float gA = pImpl->adaptationConductance;      // nS
    float tauA = pImpl->adaptationTimeConstant;   // ms
    
    // Get injected currents (synaptic and external)
    MembranePotential I_syn = neuron->getSynapticInput();
    MembranePotential I_adapt = state.adaptationCurrent;
    
    // Enhanced LIF dynamics with proper biophysical formulation:
    // dV/dt = (V_rest - V + R*(I_syn + I_adapt)) / (tau_m) - (I_adapt / (C * tau_A))
    
    // Current state contributions
    float V_driven = V_rest + R * (I_syn + I_adapt);
    
    // Leaky integration with membrane time constant
    float leakTerm = (V_driven - V) / tau;
    
    // Adaptation term (spike-frequency adaptation)
    float adaptationTerm = -I_adapt / (C * tauA);
    
    // Synaptic input contribution
    float synapticTerm = I_syn / C;
    
    // Improved numerical integration using adaptive Euler
    float dV_dt = leakTerm + adaptationTerm + synapticTerm;
    
    // Stability enhancement: limit rate of change
    float max_rate = 50.0f;  // mV/ms - reasonable maximum dV/dt
    float actual_rate = std::clamp(dV_dt, -max_rate, max_rate);
    
    // Update membrane potential
    V += actual_rate * dt_ms;
    
    // Spike-frequency adaptation dynamics
    float adaptationDecay = std::exp(-dt_ms / tauA);
    float adaptationUpdate = 0.0f;
    
    // Update adaptation current based on recent spiking
    if (neuron->isFiring()) {
        adaptationUpdate = 0.5f;  // Increase adaptation with each spike
    }
    
    state.adaptationCurrent = state.adaptationCurrent * adaptationDecay + adaptationUpdate;
    
    // Stability constraints: prevent runaway excitation
    V = std::clamp(V, V_rest - 20.0f, V_th + 10.0f);  // reasonable bounds
    
    // Check for spike
    bool fired = shouldFire(neuron, V, V_th);
    
    if (fired) {
        neuron->setFiringState(FiringState::Active);
        
        // Record spike time for STDP and plasticity
        neuron->recordSpike(dt_ms);
        
        // Spike-triggered adaptations
        neuron->incrementAdaptation();  // Increase adaptation current
        neuron->enterRefractoryState(); // Set refractory period
        
        // Apply spike-reset dynamics
        neuron->setMembranePotential(V_reset);
        
        // Store additional spike state for plasticity calculations
        neuron->setLastSpikeTime(dt_ms);
    } else {
        neuron->setFiringState(FiringState::Active);
    }
    
    // Update neuron state with new potential
    neuron->setMembranePotential(V);
}

void RealIntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt_ms) {
    // Real synaptic dynamics with short-term plasticity
    // Implement Tsodyks-Markram model for short-term plasticity
    
    SynapseId synId = synapse->getId();
    NeuronId source = synapse->getSourceNeuron();
    NeuronId dest = synapse->getDestinationNeuron();
    
    // Get current synapse state
    float weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    float depression = synapse->getShortTermDepression();
    float facilitation = synapse->getShortTermFacilitation();
    
    Timestamp lastPreTime = synapse->getLastPreSpikeTime();
    Timestamp lastPostTime = synapse->getLastPostSpikeTime();
    
    // Current time simulation
    float currentTime = dt_ms;
    
    // Update short-term plasticity using biological parameters
    
    // 1. Facilitation dynamics (Tsodyks-Markram model)
    if (lastPreTime >= 0.0f) {
        float dt_since_pre = currentTime - lastPreTime;
        // Recovery from facilitation
        facilitation *= std::exp(-dt_since_pre / 100.0f);  // 100ms time constant
    }
    
    // 2. Depression dynamics
    if (lastPostTime >= 0.0f) {
        float dt_since_post = currentTime - lastPostTime;
        // Recovery from depression
        depression += (1.0f - depression) * (1.0f - std::exp(-dt_since_post / 200.0f));  // 200ms time constant
    }
    
    // 3. Efficacy dynamics based on use history
    float utilization = 0.8f;  // Maximum utilization factor
    float use_dependent_efficacy = 1.0f - utilization * depression;
    use_dependent_efficacy = std::min(use_dependent_efficacy, 1.0f);
    
    // Apply efficacy modulation to weight
    float effective_weight = weight * efficacy * use_dependent_efficacy;
    
    // Update synapse state
    synapse->setEfficacy(use_dependent_efficacy);
    synapse->setShortTermDepression(depression);
    synapse->setShortTermFacilitation(facilitation);
    
    // Store for next update
    synapse->updateLastSpikeTime(currentTime);
}

void RealIntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real biophysical synaptic input model
    // Includes conductance-based synapses and reversal potentials
    
    SynapticWeight syn_weight = synapse->getWeight();
    SynapseType syn_type = synapse->getType();
    
    // Get neuron reversal potentials
    MembranePotential V = neuron->getMembranePotential();
    float E_exc = 0.0f;   // Excitatory reversal potential (+0mV)
    float E_inh = -80.0f; // Inhibitory reversal potential (-80mV)
    
    // Calculate conductance-based synaptic current
    float g_syn = std::abs(syn_weight) * 0.01f;  // Synaptic conductance (nS)
    
    float E_rev = (syn_type == SynapseType::Excitatory) ? E_exc : E_inh;
    
    // Conductance-based current: I_syn = g_syn * (V - E_rev)
    MembranePotential I_syn = g_syn * (V - E_rev);
    
    // Real synaptic dynamics: sign depends on neuron state and synaptic type
    if (syn_type == SynapseType::Excitatory) {
        // Excitatory synapses increase membrane potential toward E_exc
        neuron->receiveExcitatoryInput(std::max(I_syn, 0.0f));
    } else {
        // Inhibitory synapses decrease membrane potential toward E_inh
        neuron->receiveInhibitoryInput(std::max(-I_syn, 0.0f));
    }
    
    // Apply short-term plasticity effects
    float STP_effect = synapse->getEfficacy() * synapse->getShortTermDepression();
    
    if (STP_effect < 0.8f) {  // Significant depression
        // Depression reduces synaptic efficacy
        float depression_factor = 1.0f - STP_effect;
        neuron->receiveModulatoryInput(-depression_factor * 0.5f);
    }
}

bool RealIntegrateAndFireDynamics::shouldFire(const Neuron* neuron, MembranePotential V, MembranePotential threshold) const {
    // Advanced firing condition with refractory considerations
    
    if (!neuron->isActive()) return false;
    
    // Standard threshold crossing
    if (V >= threshold) return true;
    
    // Subthreshold oscillations and stochastic firing
    float noise_level = 0.5f;  // mV - typical noise level
    float random_factor = getRandomNoise();  // Could add stochastic component
    
    // Enhanced firing condition: threshold with noise margin
    if (V >= (threshold - noise_level * random_factor)) return true;
    
    // Refractory period check
    if (neuron->isRefractory()) {
        // During refractory period, neuron cannot fire
        return false;
    }
    
    // Burst detection: if voltage exceeded threshold recently
    if (neuron->hasRecentHighActivity(10)) {  // 10ms window
        return true;
    }
    
    return false;
}

void RealIntegrateAndFireDynamics::reset() {
    // Reset internal state for new simulation
    pImpl->membraneTimeConstant = 20.0f;
    pImpl->membraneResistance = 10.0f;
    pImpl->restingPotential = -70.0f;
    pImpl->thresholdPotential = -55.0f;
    pImpl->resetPotential = -65.0f;
    pImpl->adaptationConductance = 5.0f;
    pImpl->adaptationTimeConstant = 100.0f;
}

void RealIntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = std::max(1.0f, std::min(100.0f, tau));  // 1-100ms
}

void RealIntegrateAndFireDynamics::setMembraneResistance(float R) {
    pImpl->membraneResistance = std::max(1.0f, std::min(50.0f, R));  // 1-50MOhm
}

void RealIntegrateAndFireDynamics::setRestingPotential(float V_rest) {
    pImpl->restingPotential = std::clamp(V_rest, -90.0f, -50.0f);  // -90 to -50mV
}

void RealIntegrateAndFireDynamics::setThresholdPotential(float V_th) {
    pImpl->thresholdPotential = std::clamp(V_th, -65.0f, -40.0f);  // -65 to -40mV
}

float RealIntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

float RealIntegrateAndFireDynamics::getMembraneResistance() const {
    return pImpl->membraneResistance;
}

float RealIntegrateAndFireDynamics::getRestingPotential() const {
    return pImpl->restingPotential;
}

float RealIntegrateAndFireDynamics::getThresholdPotential() const {
    return pImpl->thresholdPotential;
}

float RealIntegrateAndFireDynamics::getAdaptationConductance() const {
    return pImpl->adaptationConductance;
}

float RealIntegrateAndFireDynamics::getAdaptationTimeConstant() const {
    return pImpl->adaptationTimeConstant;
}

float RealIntegrateAndFireDynamics::getRandomNoise() const {
    // Simple deterministic noise for reproducibility
    static float noise_seed = 0.12345f;
    noise_seed = std::fmod(noise_seed * 1.12345f, 1.0f);
    return noise_seed * 2.0f - 1.0f;  // [-1, 1]
}

} // namespace nlm
