#include "MorrisLecarNeuron.hpp"
#include "AdvancedNeuronModels.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <any>

namespace nlm {

struct MorrisLecarNeuron::Impl {
    NeuronId id;
    MorrisLecarParameters params;
    
    // State variables
    float V;                    // Membrane potential (mV)
    float w;                    // Potassium activation variable
    float I_ext;                // External current input (μA/cm²)
    float I_syn;                // Synaptic current (μA/cm²)
    
    // Spike state
    bool firing;                // Whether neuron is currently firing
    float lastSpikeTime;        // Time of last spike (ms)
    Timestamp lastSpikeTimestamp; // Exact timestamp of last spike
    std::vector<Timestamp> spikeHistory;     // History of spike times
    
    // Simulation state
    bool refractory;            // Whether in refractory period
    float refractoryTimeRemaining; // Time remaining in refractory period
    
    Impl(NeuronId neuronId, const MorrisLecarParameters& p)
        : id(neuronId), params(p), V(p.V_reset), w(p.w),
          I_ext(0.0f), I_syn(0.0f), firing(false),
          lastSpikeTime(-1.0f), lastSpikeTimestamp(0.0),
          refractory(false), refractoryTimeRemaining(0.0f) {}
};

MorrisLecarNeuron::MorrisLecarNeuron(NeuronId id, const MorrisLecarParameters& params)
    : pImpl(std::make_unique<Impl>(id, params)) {}

bool MorrisLecarNeuron::step(Timestamp currentTime) {
    if (pImpl->refractory) {
        pImpl->refractoryTimeRemaining -= pImpl->params.dt;
        if (pImpl->refractoryTimeRemaining <= 0.0f) {
            pImpl->refractory = false;
            pImpl->refractoryTimeRemaining = 0.0f;
        }
        return false;
    }
    
    // Calculate steady-state functions and time constants
    float I_total = pImpl->I_ext + pImpl->I_syn;
    
    // Steady-state values for voltage-gated currents
    float I_Ca_inf = pImpl->params.g_Ca_inf * 
                     (1.0f / (1.0f + exp((pImpl->V - pImpl->params.V1) / pImpl->params.V2)));
    float I_K_inf = pImpl->params.g_K_inf * 
                    (1.0f / (1.0f + exp((pImpl->V - pImpl->params.V3) / pImpl->params.V4)));
    
    // Time constants for gating variables
    float tau_Ca = pImpl->params.tau_Ca * (1.0f / (cosh(0.5 * (pImpl->V - pImpl->params.V1) / pImpl->params.V2)));
    float tau_K = pImpl->params.tau_K * (1.0f / (cosh(0.5 * (pImpl->V - pImpl->params.V3) / pImpl->params.V4)));
    
    // Update gating variable w
    if (tau_K > 0.0f) {
        w += (pImpl->params.g_K_inf - pImpl->w) / tau_K * pImpl->params.dt;
    } else {
        w = pImpl->params.g_K_inf;
    }
    
    // Calculate derivatives using forward Euler
    float dVdt = (I_total - pImpl->params.g_L * (V - pImpl->params.E_L) - 
                  w * (V - pImpl->params.E_K)) / pImpl->params.C_m;
    
    // Integrate voltage
    V += dVdt * pImpl->params.dt;
    
    // Check for spike threshold
    if (V >= pImpl->params.V_th && !pImpl->firing) {
        pImpl->firing = true;
        pImpl->lastSpikeTime = currentTime;
        pImpl->lastSpikeTimestamp = currentTime;
        recordSpike(currentTime);
        
        // Reset voltage to reset potential
        V = pImpl->params.V_reset;
        
        // Set refractory period (5-15 ms typical for Morris-Lecar)
        pImpl->refractory = true;
        pImpl->refractoryTimeRemaining = 8.0f;
        
        // Reset w to its steady-state value at reset potential
        w = pImpl->params.g_K_inf * (1.0f / (1.0f + exp((V - pImpl->params.V3) / pImpl->params.V4)));
    }
    
    // Clamp voltage to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    return pImpl->firing;
}

MembranePotential MorrisLecarNeuron::getMembranePotential() const {
    return pImpl->V;
}

bool MorrisLecarNeuron::isFiring() const {
    return pImpl->firing;
}

float MorrisLecarNeuron::getLastSpikeTime() const {
    return pImpl->lastSpikeTime;
}

void MorrisLecarNeuron::reset() {
    pImpl->V = pImpl->params.V_reset;
    pImpl->w = pImpl->params.g_K_inf;
    pImpl->I_ext = 0.0f;
    pImpl->I_syn = 0.0f;
    pImpl->firing = false;
    pImpl->lastSpikeTime = -1.0f;
    pImpl->lastSpikeTimestamp = 0.0;
    pImpl->refractory = false;
    pImpl->refractoryTimeRemaining = 0.0f;
    pImpl->spikeHistory.clear();
}

AdvancedNeuronModelType MorrisLecarNeuron::getModelType() const {
    return AdvancedNeuronModelType::MorrisLecar;
}

const std::any& MorrisLecarNeuron::getParameters() const {
    static std::any params = pImpl->params;
    return params;
}

void MorrisLecarNeuron::injectCurrent(MembranePotential current) {
    pImpl->I_ext = current;
}

void MorrisLecarNeuron::receiveSynapticInput(MembranePotential amplitude) {
    pImpl->I_syn += amplitude;
}

void MorrisLecarNeuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > 100) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& MorrisLecarNeuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void MorrisLecarNeuron::setParameters(const MorrisLecarParameters& params) {
    pImpl->params = params;
}

} // namespace nlm
