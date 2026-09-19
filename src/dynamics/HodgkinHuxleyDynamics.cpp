#include "HodgkinHuxleyDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct HodgkinHuxleyDynamics::Impl {
    // HH model constants
    const float V_Na = 50.0f;    // Sodium reversal potential
    const float V_K = -77.0f;    // Potassium reversal potential
    const float V_L = -54.387f;  // Leak reversal potential
    
    // Simulation parameters
    float dt;                    // Time step
    float cm;                    // Membrane capacitance
    float g_Na_max_;             // Max sodium conductance
    float g_K_max_;              // Max potassium conductance
    float g_L_;                  // Leak conductance
    
    // Gating variable rates
    float alpha_m(float V) {
        float Vhalf = 25.0f;
        float k = 10.0f;
        return 0.1f * (Vhalf - V) / (std::exp((Vhalf - V) / k) - 1.0f);
    }
    
    float beta_m(float V) {
        float Vhalf = -10.0f;
        float k = 80.0f;
        return 4.0f * std::exp(-V / k);
    }
    
    float alpha_h(float V) {
        return 0.07f * std::exp(-V / 20.0f);
    }
    
    float beta_h(float V) {
        float Vhalf = -50.0f;
        float k = 10.0f;
        return 1.0f / (std::exp((Vhalf - V) / k) + 1.0f);
    }
    
    float alpha_n(float V) {
        float Vhalf = 10.0f;
        float k = 10.0f;
        return 0.01f * (Vhalf - V) / (std::exp((Vhalf - V) / k) - 1.0f);
    }
    
    float beta_n(float V) {
        return 0.125f * std::exp(-V / 80.0f);
    }
    
    Impl() : dt(0.001f), cm(1.0f), g_Na_max_(120.0f), g_K_max_(36.0f), g_L_(0.3f) {}
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {
    reset();
}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() {
    delete pImpl;
}

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    // Update time step
    pImpl->dt = static_cast<float>(dt);
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    
    // Get injected current
    float I_inj = neuron->getTotalCurrent();
    neuron->clearTotalCurrent();
    
    // Compute gating variable rates
    float alpha_m = pImpl->alpha_m(V);
    float beta_m = pImpl->beta_m(V);
    float alpha_h = pImpl->alpha_h(V);
    float beta_h = pImpl->beta_h(V);
    float alpha_n = pImpl->alpha_n(V);
    float beta_n = pImpl->beta_n(V);
    
    // Update gating variables using Euler integration
    float m = pImpl->m_;
    float h = pImpl->h_;
    float n = pImpl->n_;
    
    m += (alpha_m * (1.0f - m) - beta_m * m) * pImpl->dt;
    h += (alpha_h * (1.0f - h) - beta_h * h) * pImpl->dt;
    n += (alpha_n * (1.0f - n) - beta_n * n) * pImpl->dt;
    
    // Clamp to [0, 1]
    m = std::max(0.0f, std::min(1.0f, m));
    h = std::max(0.0f, std::min(1.0f, h));
    n = std::max(0.0f, std::min(1.0f, n));
    
    pImpl->m_ = m;
    pImpl->h_ = h;
    pImpl->n_ = n;
    
    // Compute ionic currents
    float I_Na = pImpl->g_Na_max_ * m * m * m * h * (V - pImpl->V_Na);
    float I_K = pImpl->g_K_max_ * n * n * n * (V - pImpl->V_K);
    float I_L = pImpl->g_L_ * (V - pImpl->V_L);
    
    // Total membrane current
    float I_total = I_Na + I_K + I_L - I_inj;
    
    // Update membrane potential
    float dV_dt = I_total / pImpl->cm;
    float V_new = V + dV_dt * pImpl->dt;
    neuron->setMembranePotential(V_new);
    
    // Check for refractory and set firing state
    if (shouldFire(neuron) && !neuron->isRefractory()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: use actual time
    }
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // HH synapses can have voltage-dependent properties
    // For now, use standard synaptic dynamics
    synapse->step(static_cast<double>(dt));
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // HH neurons can have complex spike integration
    // For now, use standard excitatory/inhibitory input
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    float V = neuron->getMembranePotential();
    return V >= thresholdVoltage;
}

void HodgkinHuxleyDynamics::reset() {
    pImpl->m_ = 0.05f;  // Steady-state at resting potential
    pImpl->h_ = 0.6f;
    pImpl->n_ = 0.32f;
}

void HodgkinHuxleyDynamics::setMembraneTimeConstant(float tau) {
    pImpl->cm = 1.0f / tau;  // Inverse relationship
}

float HodgkinHuxleyDynamics::getMembraneTimeConstant() const {
    return 1.0f / std::max(0.001f, pImpl->cm);
}

} // namespace nlm