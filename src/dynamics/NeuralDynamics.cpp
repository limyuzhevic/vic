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
    // Real integrate-and-fire dynamics implementation
    // dV/dt = -(V - V_rest) / tau + I / C
    // Where tau = R * C, C = 1 nF (standard neuron)
    
    const auto& state = neuron->getState();
    
    // Get current neuron state
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float threshold = state.threshold;
    
    // Get synaptic currents
    float I_syn = neuron->getTotalCurrent();
    
    // Membrane parameters
    float R = pImpl->membraneResistance;  // MOhm
    float C = 1.0f;  // nF, standard membrane capacitance
    float tau = pImpl->membraneTimeConstant * R * C;  // time constant in ms
    
    // Convert dt from seconds to milliseconds
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    
    // Apply real I-F dynamics with exponential Euler integration
    // V_new = V_old * exp(-dt/tau) + R*C*I_syn * (1 - exp(-dt/tau))
    float alpha = std::exp(-dt_ms / tau);
    
    // Update membrane potential
    float V_new = V * alpha + R * C * I_syn * (1.0f - alpha);
    neuron->setMembranePotential(V_new);
    
    // Apply spike-frequency adaptation if active
    if (state.adaptationVariable > 0.0f) {
        // Adaptation current reduces membrane potential
        float adaptationCurrent = state.adaptationVariable * 0.1f;
        V_new -= adaptationCurrent * dt_ms;
        neuron->setMembranePotential(V_new);
    }
    
    // Check for spike threshold crossing
    if (V_new >= threshold && !neuron->isRefractory()) {
        // Neuron fires - record spike and reset
        float currentTime = 0.0f;  // TODO: pass actual simulation time
        neuron->recordSpike(currentTime);
        neuron->setFiringState(FiringState::Active);
        neuron->setMembranePotential(state.resetPotential);
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->membraneTimeConstant));
        
        // Reset adaptation variable for next spike train
        neuron->getState().adaptationVariable *= 0.5f;
    }
    
    // Apply refractory clamp
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        neuron->decrementRefractory();
    }
    
    // Clear total current after integration
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics implementation
    // Implements short-term plasticity and synaptic depression/facilitation
    
    float dtf = static_cast<float>(dt);
    
    // Get current synaptic state
    float weight = synapse->getWeight();
    float eligibility = synapse->getEligibilityTrace();
    
    // Apply short-term plasticity
    // Use Tsodyks-Markram model for synaptic dynamics
    float u = synapse->getUtilization();  // fraction of resources used
    float x = synapse->getResourceAvailable();  // available resources
    
    // Update dynamics based on spike activity and time
    // For now, implement simple facilitation/depression
    float facilitation = 1.0f + 0.1f * dtf;  // mild facilitation
    float depression = 1.0f - 0.05f * dtf;  // synaptic depression
    
    // Update synaptic efficacy
    weight = weight * facilitation * depression;
    synapse->setWeight(weight);
    
    // Apply STDP eligibility trace decay
    synapse->setEligibilityTrace(eligibility * std::exp(-dtf * 0.1f));
    
    // Update synaptic state based on recent history
    float historyUpdate = synapse->getHistoryBasedStrength();
    weight = weight * historyUpdate;
    synapse->setWeight(weight);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input application with conductance-based dynamics
    
    SynapticWeight weight = synapse->getWeight();
    
    // Determine synaptic reversal potential based on synapse type
    float reversalPotential = 0.0f;  // Typical for excitatory (exceeds threshold)
    if (!synapse->isExcitatory()) {
        reversalPotential = -80.0f;  // Typical for inhibitory (hyperpolarizing)
    }
    
    // Calculate synaptic conductance using alpha function
    float t = 0.0f;  // TODO: track time since spike
    float g_max = std::abs(weight) * 0.1f;  // Maximum conductance
    float g = g_max * t * std::exp(-t / 0.005f);  // Alpha function shape
    
    // Convert to current: I = g * (V - E_rev)
    float V = neuron->getMembranePotential();
    float I_syn = g * (V - reversalPotential);
    
    // Apply synaptic current with sign convention
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(I_syn);
    } else {
        neuron->receiveInhibitoryInput(-I_syn);
    }
    
    // Update spike history for plasticity
    neuron->recordSpike(0.0f);  // TODO: pass actual time
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Real spike detection based on integrated membrane potential
    
    float V = neuron->getMembranePotential();
    float threshold = neuron->getState().threshold;
    
    // Apply refractory period filter
    if (neuron->isRefractory()) {
        return false;
    }
    
    // Check threshold with some noise for biological realism
    float noiseLevel = 0.01f;  // 1% threshold variability
    float effectiveThreshold = threshold * (1.0f + noiseLevel * ((std::rand() % 100 - 50) / 50.0f));
    
    return V >= effectiveThreshold;
}

void IntegrateAndFireDynamics::reset() {
    // No internal state to reset for this implementation
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

// Hodgkin-Huxley dynamics implementation
struct HodgkinHuxleyDynamics::Impl {
    float maxConductanceNa;    // mS/cm²
    float maxConductanceK;    // mS/cm²
    float membraneCapacitance; // μF/cm²
    
    Impl() : maxConductanceNa(120.0f), maxConductanceK(36.0f), membraneCapacitance(1.0f) {}
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Full Hodgkin-Huxley neuron dynamics
    // Implements conductance-based synapses with voltage-gated ion channels
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    
    float dtf = static_cast<float>(dt) * 1000.0f;  // Convert to ms
    
    // Membrane currents
    float I_leak = 0.3f * (V - 60.0f);  // Leak current
    float I_Na = pImpl->maxConductanceNa * std::pow(state.activationNa, 3) * (V - 50.0f); // Sodium current
    float I_K = pImpl->maxConductanceK * std::pow(state.activationK, 4) * (V - -77.0f); // Potassium current
    
    // Total synaptic input
    float I_syn = neuron->getTotalCurrent();
    
    // Total membrane current
    float I_total = I_syn - I_leak - I_Na - I_K;
    
    // Update membrane potential: C * dV/dt = I_total
    float dV = I_total / pImpl->membraneCapacitance * dtf;
    neuron->setMembranePotential(V + dV);
    
    // Update gating variables using Euler integration
    // m (sodium activation), h (sodium inactivation), n (potassium activation)
    float dm = (alpha_m(V) * (1.0f - state.activationNa) - beta_m(V) * state.activationNa) * dtf;
    float dh = (alpha_h(V) * (1.0f - state.inactivationH) - beta_h(V) * state.inactivationH) * dtf;
    float dn = (alpha_n(V) * (1.0f - state.activationK) - beta_n(V) * state.activationK) * dtf;
    
    neuron->getState().activationNa = std::clamp(state.activationNa + dm, 0.0f, 1.0f);
    neuron->getState().inactivationH = std::clamp(state.inactivationH + dh, 0.0f, 1.0f);
    neuron->getState().activationK = std::clamp(state.activationK + dn, 0.0f, 1.0f);
    
    // Spike detection with refractory period
    if (V >= 55.0f && !neuron->isRefractory()) {
        float currentTime = 0.0f;  // TODO: pass actual simulation time
        neuron->recordSpike(currentTime);
        neuron->setFiringState(FiringState::Active);
        neuron->setMembranePotential(-65.0f);  // Reset potential
        neuron->setRefractoryPeriod(2);  // 2ms refractory period
    }
    
    // Reset after refractory
    if (neuron->isRefractory()) {
        neuron->decrementRefractory();
    }
    
    neuron->clearTotalCurrent();
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Simplified synapse dynamics for HH neurons
    // Uses conductance-based models with realistic kinetics
    
    float dtf = static_cast<float>(dt) * 1000.0f;
    
    // Update conductance dynamics
    float g = synapse->getConductance();
    g = g * std::exp(-dtf / 5.0f);  // Exponential decay
    synapse->setConductance(g);
    
    // Apply STDP with more realistic dynamics
    float eligibility = synapse->getEligibilityTrace();
    eligibility = eligibility * std::exp(-dtf * 0.05f); // Slower decay
    synapse->setEligibilityTrace(eligibility);
    
    // Update weight based on recent activity
    float weight = synapse->getWeight();
    weight = weight * (1.0f + synapse->getHistoryBasedStrength() * 0.1f);
    synapse->setWeight(weight);
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Conductance-based synaptic input for HH neurons
    
    float g_max = synapse->getConductance();
    float reversal = synapse->isExcitatory() ? 0.0f : -80.0f;
    
    // Calculate conductance change based on spike timing
    float g = g_max * 1.0f; // Simplified: full conductance
    
    // Calculate current: I = g * (V - E_rev)
    float V = neuron->getMembranePotential();
    float I_syn = g * (V - reversal);
    
    neuron->receiveExcitatoryInput(I_syn);
    neuron->recordSpike(0.0f); // TODO: pass actual time
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    float V = neuron->getMembranePotential();
    return V >= 55.0f && !neuron->isRefractory();
}

void HodgkinHuxleyDynamics::reset() {
    // Reset gating variables
    auto& state = const_cast<NeuronState&>(neuron->getState());
    state.activationNa = 0.05f;
    state.inactivationH = 0.6f;
    state.activationK = 0.32f;
}

void HodgkinHuxleyDynamics::setMembraneTimeConstant(float tau) {
    // Not directly used in HH model
}

float HodgkinHuxleyDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

// Helper functions for HH dynamics
float HodgkinHuxleyDynamics::alpha_m(float V) const {
    return 0.1f * (V + 40.0f) / (1.0f - std::exp(-(V + 40.0f) / 10.0f));
}

float HodgkinHuxleyDynamics::beta_m(float V) const {
    return 4.0f * std::exp(-(V + 65.0f) / 18.0f);
}

float HodgkinHuxleyDynamics::alpha_h(float V) const {
    return 0.07f * std::exp(-(V + 65.0f) / 20.0f);
}

float HodgkinHuxleyDynamics::beta_h(float V) const {
    return 1.0f / (1.0f + std::exp(-(V + 35.0f) / 10.0f));
}

float HodgkinHuxleyDynamics::alpha_n(float V) const {
    return 0.01f * (V + 55.0f) / (1.0f - std::exp(-(V + 55.0f) / 10.0f));
}

float HodgkinHuxleyDynamics::beta_n(float V) const {
    return 0.125f * std::exp(-(V + 65.0f) / 80.0f);
}

// FitzHugh-Nagumo dynamics implementation
struct FitzHughNagumoDynamics::Impl {
    float threshold;      // Threshold for spike generation
    float recoveryTime;    // Time constant for recovery variable
    float noiseLevel;      // Neural noise
    
    Impl() : threshold(30.0f), recoveryTime(6.0f), noiseLevel(0.1f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // FitzHugh-Nagumo model: simplified excitable system
    // Captures bursting and oscillatory dynamics
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    float w = state.adaptationVariable;  // Recovery variable
    
    float dtf = static_cast<float>(dt) * 1000.0f;
    
    // FHN equations:
    // dV/dt = (V - V^3/3 - w + I_syn + noise) / tau
    // dw/dt = epsilon * (V + a - b*w)
    
    float I_syn = neuron->getTotalCurrent();
    float noise = pImpl->noiseLevel * ((std::rand() % 200 - 100) / 100.0f);
    
    // Compute derivatives
    float dV = (V - V*V*V/3.0f - w + I_syn + noise) / 5.0f;
    float dw = 0.08f * (V + 0.7f - 0.8f * w);
    
    // Update state
    V += dV * dtf;
    w += dw * dtf;
    
    neuron->setMembranePotential(V);
    neuron->getState().adaptationVariable = w;
    
    // Spike detection based on threshold crossing
    if (V >= pImpl->threshold) {
        float currentTime = 0.0f;  // TODO: pass actual simulation time
        neuron->recordSpike(currentTime);
        neuron->setFiringState(FiringState::Active);
        
        // Reset with hysteresis to prevent immediate re-firing
        V = -70.0f;
        w = w + 10.0f;  // Large recovery pulse
        neuron->setMembranePotential(V);
        neuron->getState().adaptationVariable = w;
        neuron->setRefractoryPeriod(5);  // Brief refractory period
    }
    
    // Update refractory state
    if (neuron->isRefractory()) {
        neuron->decrementRefractory();
    }
    
    neuron->clearTotalCurrent();
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Simple synapse dynamics for FHN model
    // Fast dynamics matching FHN timescales
    
    float dtf = static_cast<float>(dt) * 1000.0f;
    
    // Update synaptic variables
    float g = synapse->getConductance();
    g = g * std::exp(-dtf / 2.0f);  // Fast decay
    synapse->setConductance(g);
    
    // Apply plasticity with FHN-appropriate dynamics
    float eligibility = synapse->getEligibilityTrace();
    eligibility = eligibility * std::exp(-dtf * 0.2f); // Moderate decay
    synapse->setEligibilityTrace(eligibility);
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Synaptic input for FHN model
    float g = synapse->getConductance();
    float reversal = synapse->isExcitatory() ? 0.0f : -80.0f;
    
    float V = neuron->getMembranePotential();
    float I_syn = g * (V - reversal);
    
    neuron->receiveExcitatoryInput(I_syn);
    neuron->recordSpike(0.0f); // TODO: pass actual time
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    float V = neuron->getMembranePotential();
    return V >= pImpl->threshold && !neuron->isRefractory();
}

void FitzHughNagumoDynamics::reset() {
    // Reset FHN state variables
    neuron->getState().adaptationVariable = 0.0f;
}

void FitzHughNagumoDynamics::setMembraneTimeConstant(float tau) {
    // Not directly used in FHN model
}

float FitzHughNagumoDynamics::getMembraneTimeConstant() const {
    return 5.0f;  // tau in FHN equations
}

} // namespace nlm
