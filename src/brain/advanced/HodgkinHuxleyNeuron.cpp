#include "HodgkinHuxleyNeuron.hpp"
#include "AdvancedNeuronModels.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <any>

namespace nlm {

struct HodgkinHuxleyNeuron::Impl {
    NeuronId id;
    HodgkinHuxleyParameters params;
    
    // State variables
    MembranePotential V;                    // Membrane potential (mV)
    MembranePotential V_rest;                // Resting potential (mV)
    float m;                                 // Sodium activation gating variable
    float h;                                 // Sodium inactivation gating variable
    float n;                                 // Potassium activation gating variable
    float I_injected;                        // Injected current (μA/cm²)
    float I_syn;                             // Synaptic current (μA/cm²)
    
    // Spike state
    bool firing;                             // Whether neuron is currently firing
    float lastSpikeTime;                     // Time of last spike (ms)
    Timestamp lastSpikeTimestamp;            // Exact timestamp of last spike
    std::vector<Timestamp> spikeHistory;     // History of spike times
    
    // Simulation state
    bool refractory;                         // Whether in refractory period
    float refractoryTimeRemaining;            // Time remaining in refractory period
    float adaptationCurrent;                 // Adaptation current (mV)
    
    // Constants
    static constexpr float V_Na = 115.0f;     // Sodium reversal potential (mV)
    static constexpr float V_K = -12.0f;      // Potassium reversal potential (mV)
    static constexpr float V_L = 10.7f;       // Leak reversal potential (mV)
    static constexpr float C_m = 1.0f;        // Membrane capacitance (μF/cm²)
    
    Impl(NeuronId neuronId, const HodgkinHuxleyParameters& p)
        : id(neuronId), params(p), V(p.V_reset), V_rest(p.V_reset),
          m(0.0f), h(0.0f), n(0.0f), I_injected(0.0f), I_syn(0.0f),
          firing(false), lastSpikeTime(-1.0f), lastSpikeTimestamp(0.0),
          refractory(false), refractoryTimeRemaining(0.0f), adaptationCurrent(0.0f) {}
};

HodgkinHuxleyNeuron::HodgkinHuxleyNeuron(NeuronId id, const HodgkinHuxleyParameters& params)
    : pImpl(std::make_unique<Impl>(id, params)) {}

bool HodgkinHuxleyNeuron::step(Timestamp currentTime) {
    if (pImpl->refractory) {
        pImpl->refractoryTimeRemaining -= pImpl->params.dt;
        if (pImpl->refractoryTimeRemaining <= 0.0f) {
            pImpl->refractory = false;
            pImpl->refractoryTimeRemaining = 0.0f;
        }
        return false;
    }
    
    // Update gating variables
    updateGatingVariables(currentTime, pImpl->params.dt);
    
    // Calculate ionic currents
    float I_Na = pImpl->params.g_Na_max * std::pow(pImpl->m, 3) * pImpl->h * 
                 (pImpl->V - pImpl->params.E_Na);
    float I_K = pImpl->params.g_K_max * std::pow(pImpl->n, 4) * 
                 (pImpl->V - pImpl->params.E_K);
    float I_L = pImpl->params.g_L_max * (pImpl->V - pImpl->params.E_L);
    
    // Total membrane current
    float I_total = pImpl->I_injected + pImpl->I_syn - I_Na - I_K - I_L;
    
    // Update membrane potential using forward Euler
    float dVdt = I_total / pImpl->C_m;
    pImpl->V += dVdt * pImpl->params.dt;
    
    // Apply adaptation current
    pImpl->V -= pImpl->adaptationCurrent;
    pImpl->adaptationCurrent *= 0.9f; // Decay adaptation current
    
    // Check for spike threshold
    if (pImpl->V >= pImpl->params.V_th && !pImpl->firing) {
        pImpl->firing = true;
        pImpl->lastSpikeTime = currentTime;
        pImpl->lastSpikeTimestamp = currentTime;
        recordSpike(currentTime);
        
        // Reset membrane potential
        pImpl->V = pImpl->params.V_reset;
        
        // Set refractory period (3 ms typical for HH)
        pImpl->refractory = true;
        pImpl->refractoryTimeRemaining = 3.0f;
        
        // Update adaptation current (spike-triggered)
        pImpl->adaptationCurrent += 10.0f;
        
        // Enter refractory period
        pImpl->m = 0.05f; // Fixed values during refractory period
        pImpl->h = 0.6f;
        pImpl->n = 0.32f;
    }
    
    // Clamp voltage to reasonable range
    pImpl->V = std::clamp(pImpl->V, -100.0f, 50.0f);
    
    // Decay gating variables if not firing
    if (!pImpl->firing) {
        pImpl->m = std::clamp(pImpl->m, 0.0f, 1.0f);
        pImpl->h = std::clamp(pImpl->h, 0.0f, 1.0f);
        pImpl->n = std::clamp(pImpl->n, 0.0f, 1.0f);
    }
    
    return pImpl->firing;
}

MembranePotential HodgkinHuxleyNeuron::getMembranePotential() const {
    return pImpl->V;
}

bool HodgkinHuxleyNeuron::isFiring() const {
    return pImpl->firing;
}

float HodgkinHuxleyNeuron::getLastSpikeTime() const {
    return pImpl->lastSpikeTime;
}

void HodgkinHuxleyNeuron::reset() {
    pImpl->V = pImpl->params.V_reset;
    pImpl->V_rest = pImpl->params.V_reset;
    pImpl->m = 0.0f;
    pImpl->h = 0.0f;
    pImpl->n = 0.0f;
    pImpl->I_injected = 0.0f;
    pImpl->I_syn = 0.0f;
    pImpl->firing = false;
    pImpl->lastSpikeTime = -1.0f;
    pImpl->lastSpikeTimestamp = 0.0;
    pImpl->refractory = false;
    pImpl->refractoryTimeRemaining = 0.0f;
    pImpl->adaptationCurrent = 0.0f;
    pImpl->spikeHistory.clear();
}

AdvancedNeuronModelType HodgkinHuxleyNeuron::getModelType() const {
    return AdvancedNeuronModelType::HodgkinHuxley;
}

const std::any& HodgkinHuxleyNeuron::getParameters() const {
    static std::any params = pImpl->params;
    return params;
}

void HodgkinHuxleyNeuron::injectCurrent(MembranePotential current) {
    pImpl->I_injected = current;
}

void HodgkinHuxleyNeuron::receiveSynapticInput(MembranePotential amplitude) {
    pImpl->I_syn += amplitude;
}

void HodgkinHuxleyNeuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > 100) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& HodgkinHuxleyNeuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void HodgkinHuxleyNeuron::setParameters(const HodgkinHuxleyParameters& params) {
    pImpl->params = params;
}

void HodgkinHuxleyNeuron::updateGatingVariables(Timestamp currentTime, TimestepDuration dt) {
    // Calculate steady-state values and time constants
    float alpha_m = (2.5 - 0.1 * pImpl->V) / (std::exp(0.1 * (pImpl->V - 2.5)) - 1.0);
    float beta_m = 4.0 * std::exp(pImpl->V / 18.0);
    float alpha_h = 0.07 * std::exp(pImpl->V / 20.0);
    float beta_h = 1.0 / (std::exp(0.1 * (pImpl->V - 3.0)) + 1.0);
    float alpha_n = (0.1 - 0.01 * pImpl->V) / (std::exp(0.1 * (pImpl->V - 1.0)) - 1.0);
    float beta_n = 0.125 * std::exp(pImpl->V / 80.0);
    
    // Update gating variables using Euler integration
    if (!pImpl->refractory) {
        pImpl->m += (alpha_m * (1.0 - pImpl->m) - beta_m * pImpl->m) * dt;
        pImpl->h += (alpha_h * (1.0 - pImpl->h) - beta_h * pImpl->h) * dt;
        pImpl->n += (alpha_n * (1.0 - pImpl->n) - beta_n * pImpl->n) * dt;
    }
    
    // Clamp values to [0, 1] range
    pImpl->m = std::clamp(pImpl->m, 0.0f, 1.0f);
    pImpl->h = std::clamp(pImpl->h, 0.0f, 1.0f);
    pImpl->n = std::clamp(pImpl->n, 0.0f, 1.0f);
}

} // namespace nlm
