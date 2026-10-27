#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float capacitance;           // pF
    float sodiumConductance;     // nS
    float potassiumConductance;   // nS
    float leakConductance;        // nS
    float restingPotential;      // mV
    float thresholdPotential;    // mV
    float resetPotential;        // mV
    float sodiumReversal;         // mV
    float potassiumReversal;      // mV
    float sodiumActivationTimeConstant;  // ms
    float sodiumInactivationTimeConstant; // ms
    float potassiumActivationTimeConstant; // ms
    
    // State variables for Hodgkin-Huxley
    float sodiumActivation;       // mV
    float sodiumInactivation;     // mV
    float potassiumActivation;    // mV
    
    Impl() : 
        membraneTimeConstant(20.0f),
        membraneResistance(10.0f),
        capacitance(1.0f),
        sodiumConductance(120.0f),
        potassiumConductance(36.0f),
        leakConductance(0.3f),
        restingPotential(-70.0f),
        thresholdPotential(-55.0f),
        resetPotential(-70.0f),
        sodiumReversal(50.0f),
        potassiumReversal(-77.0f),
        sodiumActivationTimeConstant(1.0f),
        sodiumInactivationTimeConstant(5.0f),
        potassiumActivationTimeConstant(5.0f),
        sodiumActivation(alpha_n(0.0f, 1.0f)),
        sodiumInactivation(alpha_n(0.0f, 1.0f)),
        potassiumActivation(alpha_n(0.0f, 1.0f)) {}
};

float IntegrateAndFireDynamics::alpha_n(float V, float dt) const {
    // Alpha function for gating variable dynamics
    return 0.01f * (V + 55.0f) / (1.0f - std::exp(-0.1f * (V + 55.0f)));
}

float IntegrateAndFireDynamics::beta_n(float V, float dt) const {
    return 0.125f * std::exp(-0.0125 * (V + 65.0f));
}

float IntegrateAndFireDynamics::alpha_m(float V, float dt) const {
    return 0.1f * (V + 40.0f) / (1.0f - std::exp(-0.1f * (V + 40.0f)));
}

float IntegrateAndFireDynamics::beta_m(float V, float dt) const {
    return 4.0f * std::exp(-0.0556666667 * (V + 65.0f));
}

float IntegrateAndFireDynamics::alpha_h(float V, float dt) const {
    return 0.07f * std::exp(-0.05 * (V + 65.0f));
}

float IntegrateAndFireDynamics::beta_h(float V, float dt) const {
    return 1.0f / (1.0f + std::exp(-0.1f * (V + 35.0f)));
}

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real Hodgkin-Huxley integrate-and-fire dynamics
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float I_total = neuron->getTotalCurrent();
    
    // Update gating variables using exponential Euler method
    float V_clamped = std::max(-100.0f, std::min(100.0f, V)); // Prevent overflow
    float m = state.adaptationVariable;  // Reusing adaptationVariable as m for HH
    float h = state.firingRate;         // Reusing firingRate as h for HH
    float n = state.threshold;          // Reusing threshold as n for HH
    
    // Calculate gating variable derivatives
    float m_dot = alpha_m(V_clamped, dt) * (1.0f - m) - beta_m(V_clamped, dt) * m;
    float h_dot = alpha_h(V_clamped, dt) * (1.0f - h) - beta_h(V_clamped, dt) * h;
    float n_dot = alpha_n(V_clamped, dt) * (1.0f - n) - beta_n(V_clamped, dt) * n;
    
    // Update gating variables
    m += m_dot * static_cast<float>(dt);
    h += h_dot * static_cast<float>(dt);
    n += n_dot * static_cast<float>(dt);
    
    // Clamp gating variables
    m = std::clamp(m, 0.0f, 1.0f);
    h = std::clamp(h, 0.0f, 1.0f);
    n = std::clamp(n, 0.0f, 1.0f);
    
    // Calculate ionic currents
    float I_Na = pImpl->sodiumConductance * m * m * m * h * (V_clamped - pImpl->sodiumReversal);
    float I_K = pImpl->potassiumConductance * n * n * n * n * (V_clamped - pImpl->potassiumReversal);
    float I_L = pImpl->leakConductance * (V_clamped - pImpl->restingPotential);
    
    // Calculate total membrane current
    float I_membrane = I_total - I_Na - I_K - I_L;
    
    // Update membrane potential using membrane time constant
    float dV = I_membrane / pImpl->capacitance * static_cast<float>(dt);
    V += dV;
    
    // Update neuron state
    neuron->setMembranePotential(V);
    
    // Update adaptation variable and firing rate
    state.adaptationVariable = m;  // Store m in adaptation variable
    state.firingRate = 1000.0f / (1.0f + std::exp(-(V - 50.0f) / 10.0f));  // Firing rate function
    
    // Check for spike generation
    if (neuron->checkThreshold()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
        // Reset membrane potential to reset value
        neuron->setMembranePotential(state.resetPotential);
        
        // Update gating variables after spike
        m = 0.05f;   // Alpha activation
        h = 0.35f;   // Beta inactivation
        n = 0.32f;   // Beta activation
    }
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        neuron->decrementRefractory();
        if (neuron->isRefractory()) {
            neuron->setMembranePotential(state.resetPotential);
        }
    }
    
    // Clear external current for next time step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with exponential decay
    SynapticWeight weight = synapse->getWeight();
    
    // Calculate synaptic conductance change
    float G_max = 1.0f;  // Maximum conductance
    float tau_syn = 5.0f;  // Synaptic time constant (ms)
    
    if (synapse->isActive()) {
        // Active synapse: increase conductance
        synapse->step(dt);
    } else {
        // Inactive synapse: decay conductance
        float G = synapse->getConductance();
        G *= std::exp(-static_cast<float>(dt) / tau_syn);
        synapse->setConductance(G);
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input with conductance-based dynamics
    float G_syn = synapse->getConductance();
    float reversalPotential = synapse->isExcitatory() ? pImpl->sodiumReversal : pImpl->potassiumReversal;
    
    // Calculate synaptic current
    float I_syn = G_syn * (neuron->getMembranePotential() - reversalPotential);
    
    // Apply to neuron based on synapse type
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(std::abs(I_syn));
    } else {
        neuron->receiveInhibitoryInput(std::abs(I_syn));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // Reset internal HH state
    pImpl->sodiumActivation = alpha_n(0.0f, 0.0f);
    pImpl->sodiumInactivation = alpha_n(0.0f, 0.0f);
    pImpl->potassiumActivation = alpha_n(0.0f, 0.0f);
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
