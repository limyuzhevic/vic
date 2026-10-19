#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Enhanced integrate-and-fire dynamics with biological accuracy
    // Uses the neuron's internal stepLIF method for consistency
    const auto& state = neuron->getState();
    
    // Update neuron using the enhanced stepLIF method
    bool fired = neuron->stepLIF(state.lastSpikeTime, dt);
    
    // Additional synaptic dynamics integration
    float synapticInput = neuron->getTotalCurrent();
    float membranePotential = neuron->getMembranePotential();
    float restingPotential = state.restingPotential;
    float threshold = state.threshold;
    float refractoryPeriod = static_cast<float>(state.refractoryPeriod);
    
    // Adaptive threshold dynamics based on recent activity
    float adaptationFactor = 1.0f;
    if (neuron->getLastSpikeTime() > 0) {
        float timeSinceLastSpike = static_cast<float>(state.lastSpikeTime);
        adaptationFactor = std::exp(-timeSinceLastSpike / 100.0f);  // Decay over 100ms
    }
    
    // Dynamic threshold based on adaptation
    float dynamicThreshold = threshold * adaptationFactor;
    
    // Update neuron state based on synaptic input
    if (synapticInput != 0.0f) {
        // Synaptic integration with conductance-based model
        float conductance = std::abs(synapticInput) / 10.0f;  // Normalized conductance
        float reversalPotential = synapticInput > 0.0f ? 50.0f : -80.0f;  // Excitatory/inhibitory
        
        // Calculate synaptic current
        float synapticCurrent = conductance * (membranePotential - reversalPotential);
        
        // Apply synaptic current to membrane potential
        float deltaV = synapticCurrent * static_cast<float>(dt) * 0.001f;  // Scale factor
        neuron->addToMembranePotential(deltaV);
    }
    
    // Metabolic regulation of excitability
    float metabolicRate = neuron->getMetabolicCost();
    float regulationFactor = std::clamp(1.0f - metabolicRate * 0.1f, 0.5f, 2.0f);
    
    // Apply metabolic regulation to leak conductance
    float regulatedLeakConductance = state.leakConductance * regulationFactor;
    
    // Store regulated values back to neuron state
    const_cast<NeuronState&>(state).leakConductance = regulatedLeakConductance;
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Advanced synaptic dynamics with short-term plasticity
    const auto& preSynState = synapse->getPreState();
    const auto& postSynState = synapse->getPostState();
    
    SynapticWeight weight = synapse->getWeight();
    float timeConstant = synapse->getTimeConstant();
    
    // Short-term plasticity based on recent activity
    float utilization = synapse->getUtilization();
    float recoveryRate = synapse->getRecoveryRate();
    
    // Update utilization based on pre-synaptic spike activity
    if (preSynState.spikeCount > 0) {
        utilization = std::min(1.0f, utilization + (1.0f - utilization) * (1.0f - std::exp(-dt / 5.0f)));
    }
    
    // Recover utilization when not active
    utilization *= std::exp(-dt / recoveryRate);
    
    // Update synaptic efficacy
    float efficacy = utilization * weight;
    
    // Apply depression/facilitation based on activity history
    float activityHistory = synapse->getActivityHistory();
    if (activityHistory > 0) {
        // Activity-dependent modification
        float modulation = 1.0f + 0.1f * std::log(1.0f + activityHistory);
        efficacy *= modulation;
    }
    
    // Update synaptic weight with homeostasis
    if (std::abs(efficacy - weight) > 0.001f) {
        float homeostaticFactor = 1.0f - dt * 0.01f;  // Slow homeostatic regulation
        weight = weight * homeostaticFactor + efficacy * (1.0f - homeostaticFactor);
        synapse->setWeight(weight);
    }
    
    // Update activity history
    synapse->updateActivityHistory(dt);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Realistic synaptic input with conductance-based dynamics
    SynapticWeight weight = synapse->getWeight();
    float reversalPotential = synapse->isExcitatory() ? 50.0f : -80.0f;
    float conductance = std::abs(weight) / 10.0f;  // Synaptic conductance
    
    // Calculate synaptic current
    MembranePotential membranePotential = neuron->getMembranePotential();
    float synapticCurrent = conductance * (membranePotential - reversalPotential);
    
    // Apply to neuron with realistic dynamics
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(std::abs(synapticCurrent));
    } else {
        neuron->receiveInhibitoryInput(std::abs(synapticCurrent));
    }
    
    // Record synaptic event for plasticity
    neuron->recordSpike(0.0f);  // Spike timestamp will be handled by main simulation loop
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Enhanced firing detection with refractory considerations
    float membranePotential = neuron->getMembranePotential();
    float threshold = neuron->getThreshold();
    
    // Check if neuron is in refractory period
    if (neuron->isRefractory()) {
        return false;
    }
    
    // Dynamic threshold based on recent activity and adaptation
    float adaptationFactor = 1.0f;
    if (neuron->getLastSpikeTime() > 0) {
        adaptationFactor = std::exp(-neuron->getLastSpikeTime() / 50.0f);
    }
    
    float dynamicThreshold = threshold * adaptationFactor;
    
    return membranePotential >= dynamicThreshold;
}

void IntegrateAndFireDynamics::reset() {
    // Reset dynamics state
    pImpl->membraneTimeConstant = 20.0f;
    pImpl->membraneResistance = 10.0f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

// Hodgkin-Huxley dynamics implementation
namespace HH {
    // Hodgkin-Huxley rate constants
    float alpha_m(float V) { return 0.1f * (V + 40.0f) / (1.0f - std::exp(-(V + 40.0f) / 10.0f)); }
    float beta_m(float V) { return 4.0f * std::exp(-(V + 65.0f) / 18.0f); }
    float alpha_h(float V) { return 0.07f * std::exp(-(V + 65.0f) / 20.0f); }
    float beta_h(float V) { return 1.0f / (1.0f + std::exp(-(V + 35.0f) / 10.0f)); }
    float alpha_n(float V) { return 0.01f * (V + 55.0f) / (1.0f - std::exp(-(V + 55.0f) / 10.0f)); }
    float beta_n(float V) { return 0.125f * std::exp(-(V + 65.0f) / 80.0f); }
    
    // Steady-state values and time constants
    float m_inf(float V) { return alpha_m(V) / (alpha_m(V) + beta_m(V)); }
    float h_inf(float V) { return alpha_h(V) / (alpha_h(V) + beta_h(V)); }
    float n_inf(float V) { return alpha_n(V) / (alpha_n(V) + beta_n(V)); }
    
    float tau_m(float V) { return 1.0f / (alpha_m(V) + beta_m(V)); }
    float tau_h(float V) { return 1.0f / (alpha_h(V) + beta_h(V)); }
    float tau_n(float V) { return 1.0f / (alpha_n(V) + beta_n(V)); }
}

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    // Hodgkin-Huxley model parameters
    const float C_m = 1.0f;  // Membrane capacitance (μF/cm²)
    const float g_Na = 120.0f;  // Sodium conductance (mS/cm²)
    const float g_K = 36.0f;  // Potassium conductance (mS/cm²)
    const float g_L = 0.3f;  // Leak conductance (mS/cm²)
    const float E_Na = 50.0f;  // Sodium reversal potential (mV)
    const float E_K = -77.0f;  // Potassium reversal potential (mV)
    const float E_L = -54.387f;  // Leak reversal potential (mV)
    
    // Get current gating variables
    float m = state.gatingVariable1;  // Sodium activation
    float h = state.gatingVariable2;  // Sodium inactivation
    float n = state.gatingVariable3;  // Potassium activation
    
    float V = neuron->getMembranePotential();
    
    // Calculate ionic currents
    float I_Na = g_Na * m * m * m * h * (V - E_Na);
    float I_K = g_K * n * n * n * n * (V - E_K);
    float I_L = g_L * (V - E_L);
    
    // Total synaptic current
    float I_syn = neuron->getTotalCurrent();
    
    // Calculate derivatives
    float dVdt = -(I_Na + I_K + I_L + I_syn) / C_m;
    
    float dmdt = (HH::m_inf(V) - m) / HH::tau_m(V);
    float dhdt = (HH::h_inf(V) - h) / HH::tau_h(V);
    float dndt = (HH::n_inf(V) - n) / HH::tau_n(V);
    
    // Update state using forward Euler
    float V_new = V + dVdt * static_cast<float>(dt);
    float m_new = m + dmdt * static_cast<float>(dt);
    float h_new = h + dhdt * static_cast<float>(dt);
    float n_new = n + dndt * static_cast<float>(dt);
    
    // Clamp gating variables to [0, 1]
    m_new = std::clamp(m_new, 0.0f, 1.0f);
    h_new = std::clamp(h_new, 0.0f, 1.0f);
    n_new = std::clamp(n_new, 0.0f, 1.0f);
    
    // Update neuron state
    neuron->setMembranePotential(V_new);
    const_cast<NeuronState&>(state).gatingVariable1 = m_new;
    const_cast<NeuronState&>(state).gatingVariable2 = h_new;
    const_cast<NeuronState&>(state).gatingVariable3 = n_new;
    
    // Check for spike threshold crossing
    if (V_new > 0.0f) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0f);  // Will be updated by main loop
        
        // Reset after spike (simple threshold reset)
        neuron->setMembranePotential(state.resetPotential);
        
        // Reset gating variables after spike
        const_cast<NeuronState&>(state).gatingVariable1 = HH::m_inf(state.resetPotential);
        const_cast<NeuronState&>(state).gatingVariable2 = HH::h_inf(state.resetPotential);
        const_cast<NeuronState&>(state).gatingVariable3 = HH::n_inf(state.resetPotential);
    }
    
    // Update metabolic state
    neuron->updateMetabolicState();
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // HH dynamics with advanced synaptic modeling
    // Include NMDA and AMPA conductance dynamics
    
    SynapticWeight weight = synapse->getWeight();
    float reversalPotential = synapse->isExcitatory() ? 0.0f : -80.0f;
    
    // Calculate synaptic conductance with dynamics
    float conductance = synapse->getConductance();
    float activation = synapse->getActivation();
    float inactivation = synapse->getInactivation();
    
    // Update conductance based on presynaptic activity
    float dConductance = (activation * (1.0f - conductance) - conductance / synapse->getTimeConstant()) * dt;
    conductance += dConductance;
    
    // Calculate synaptic current
    float V = synapse->getPostNeuron()->getMembranePotential();
    float synapticCurrent = conductance * (V - reversalPotential);
    
    // Apply current to postsynaptic neuron
    if (synapse->isExcitatory()) {
        synapse->getPostNeuron()->receiveExcitatoryInput(std::abs(synapticCurrent));
    } else {
        synapse->getPostNeuron()->receiveInhibitoryInput(std::abs(synapticCurrent));
    }
    
    // Update synaptic efficacy with activity-dependent plasticity
    if (synapse->getPreSpikeCount() > 0) {
        float potentiation = std::exp(-synapse->getTimeSinceLastPreSpike() / 10.0f);
        float currentWeight = synapse->getWeight();
        synapse->setWeight(currentWeight * (1.0f + potentiation * 0.05f));
    }
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Implement spike timing dependent plasticity for HH dynamics
    if (synapse && neuron) {
        // Calculate STDP weight change
        float deltaT = calculateSpikeTimingDifference(synapse, neuron);
        
        if (deltaT > 0) {
            // Pre before post: LTP
            float weight = synapse->getWeight();
            synapse->setWeight(weight + 0.01f * std::exp(-deltaT / 20.0f));
        } else if (deltaT < 0) {
            // Post before pre: LTD
            float weight = synapse->getWeight();
            synapse->setWeight(weight - 0.012f * std::exp(deltaT / 20.0f));
        }
        
        // Apply synaptic input
        IntegrateAndFireDynamics::applySpikeInput(neuron, synapse);
    }
}

float HodgkinHuxleyDynamics::calculateSpikeTimingDifference(const Synapse* synapse, const Neuron* neuron) const {
    // Calculate time difference between pre- and post-synaptic spikes
    float preSpikeTime = synapse->getLastPreSpikeTime();
    float postSpikeTime = neuron->getLastSpikeTime();
    
    if (preSpikeTime > 0 && postSpikeTime > 0) {
        return postSpikeTime - preSpikeTime;
    }
    
    return 0.0f;
}

void HodgkinHuxleyDynamics::reset() {
    // Reset HH dynamics state
}

void HodgkinHuxleyDynamics::setMembraneTimeConstant(float tau) {
    // HH has specific time constant constraints
    // The main parameter is the membrane time constant, but HH uses specific rates
}

float HodgkinHuxleyDynamics::getMembraneTimeConstant() const {
    return 20.0f;  // Standard HH membrane time constant
}

// FitzHugh-Nagumo dynamics implementation
namespace FHN {
    // FitzHugh-Nagumo rate functions
    float alpha(float V) { return V * (V - 0.1f) * (1.0f - V); }
    float beta(float V) { return 0.1f * (V - 0.5f); }
    
    // Cubic nullcline
    float nullcline_C(float V) { return V * (V - 0.1f) * (1.0f - V); }
    
    // Linear nullcline
    float nullcline_L(float W) { return 0.1f * (W - 0.5f); }
}

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    // FitzHugh-Nagumo model parameters
    const float a = 0.7f;
    const float b = 0.8f;
    const float I_ext = neuron->getTotalCurrent();  // External input
    
    float V = neuron->getMembranePotential();
    float W = state.gatingVariable1;  // Recovery variable
    
    // FHN dynamics equations
    float dVdt = a * (V - V * V * V / 3.0f - W + I_ext);
    float dWdt = b * (V - a * W - 0.5f);
    
    // Update using forward Euler
    float V_new = V + dVdt * static_cast<float>(dt);
    float W_new = W + dWdt * static_cast<float>(dt);
    
    // Update neuron state
    neuron->setMembranePotential(V_new);
    const_cast<NeuronState&>(state).gatingVariable1 = W_new;
    
    // Enhanced firing detection with FHN characteristics
    if (V_new > 0.5f) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0f);
        
        // FHN has different reset characteristics
        float resetVoltage = -1.2f;
        neuron->setMembranePotential(resetVoltage);
        
        // Reset recovery variable
        const_cast<NeuronState&>(state).gatingVariable1 = -0.8f;
    }
    
    // Metabolic constraints for FHN
    float metabolicCost = neuron->getMetabolicCost();
    float energyAvailable = std::max(0.0f, 1.0f - metabolicCost);
    
    // Limit firing rate based on energy availability
    if (energyAvailable < 0.3f && neuron->isFiring()) {
        // Reduce firing likelihood when energy is low
        float energyModulation = energyAvailable / 0.3f;
        neuron->setMembranePotential(V_new * energyModulation);
    }
    
    neuron->updateMetabolicState();
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // FHN dynamics with simplified synaptic modeling
    
    // Calculate synaptic conductance with FHN-appropriate dynamics
    float conductance = synapse->getConductance();
    float activation = synapse->getActivation();
    
    // FHN-appropriate synaptic dynamics
    float dConductance = (activation * (1.0f - conductance) - conductance / synapse->getTimeConstant()) * dt;
    conductance += dConductance;
    
    // Apply to postsynaptic neuron
    if (synapse->isExcitatory()) {
        IntegrateAndFireDynamics::applySpikeInput(synapse->getPostNeuron(), synapse);
    }
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Simplified STDP for FHN
    if (synapse && neuron) {
        float deltaT = calculateSpikeTimingDifference(synapse, neuron);
        
        if (std::abs(deltaT) < 10.0f) {  // Window for STDP
            float weight = synapse->getWeight();
            if (deltaT > 0) {
                synapse->setWeight(weight + 0.005f * std::exp(-deltaT / 5.0f));
            } else {
                synapse->setWeight(weight - 0.006f * std::exp(deltaT / 5.0f));
            }
        }
    }
}

float FitzHughNagumoDynamics::calculateSpikeTimingDifference(const Synapse* synapse, const Neuron* neuron) const {
    float preSpikeTime = synapse->getLastPreSpikeTime();
    float postSpikeTime = neuron->getLastSpikeTime();
    
    if (preSpikeTime > 0 && postSpikeTime > 0) {
        return postSpikeTime - preSpikeTime;
    }
    
    return 0.0f;
}

void FitzHughNagumoDynamics::reset() {
    // Reset FHN dynamics state
}

void FitzHughNagumoDynamics::setMembraneTimeConstant(float tau) {
    // FHN doesn't use traditional membrane time constant
    // Parameters are adjusted for the cubic nullcline
}

float FitzHughNagumoDynamics::getMembraneTimeConstant() const {
    return 10.0f;  // Characteristic timescale for FHN
}

} // namespace nlm
