#include "IzhikevichNeuron.hpp"
#include "AdvancedNeuronModels.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <any>

namespace nlm {

struct IzhikevichNeuron::Impl {
    NeuronId id;
    IzhikevichParameters params;
    
    // State variables
    float V;                    // Membrane potential (mV)
    float u;                    // Recovery variable (mV)
    float I_ext;                // External current input (μA/cm²)
    float I_syn;                // Synaptic current (μA/cm²)
    
    // Spike state
    bool firing;                // Whether neuron is currently firing
    float lastSpikeTime;        // Time of last spike (ms)
    Timestamp lastSpikeTimestamp;// Exact timestamp of last spike
    std::vector<Timestamp> spikeHistory;     // History of spike times
    
    // Simulation state
    bool refractory;            // Whether in refractory period
    float refractoryTimeRemaining; // Time remaining in refractory period
    
    Impl(NeuronId neuronId, const IzhikevichParameters& p)
        : id(neuronId), params(p), V(p.V_init), u(p.u_init), 
          I_ext(0.0f), I_syn(0.0f), firing(false), 
          lastSpikeTime(-1.0f), lastSpikeTimestamp(0.0),
          refractory(false), refractoryTimeRemaining(0.0f) {}
};

IzhikevichNeuron::IzhikevichNeuron(NeuronId id, const IzhikevichParameters& params)
    : pImpl(std::make_unique<Impl>(id, params)) {}

bool IzhikevichNeuron::step(Timestamp currentTime) {
    if (pImpl->refractory) {
        pImpl->refractoryTimeRemaining -= pImpl->params.dt;
        if (pImpl->refractoryTimeRemaining <= 0.0f) {
            pImpl->refractory = false;
            pImpl->refractoryTimeRemaining = 0.0f;
        }
        return false;
    }
    
    // Update recovery variable
    pImpl->u += pImpl->params.a * (pImpl->V - 65.0f - pImpl->u);
    
    // Update membrane potential using Izhikevich model equations
    // dV/dt = 0.04*V^2 + 5*V + 140 - u + I_ext + I_syn
    // du/dt = a*(b*V - u)
    float dVdt = 0.04f * pImpl->V * pImpl->V + 5.0f * pImpl->V + 140.0f - 
                 pImpl->u + pImpl->I_ext + pImpl->I_syn;
    
    pImpl->V += dVdt * pImpl->params.dt;
    
    // Check for spike threshold
    if (pImpl->V >= pImpl->params.V_th && !pImpl->firing) {
        pImpl->firing = true;
        pImpl->lastSpikeTime = currentTime;
        pImpl->lastSpikeTimestamp = currentTime;
        recordSpike(currentTime);
        
        // Reset membrane potential and recovery variable based on model type
        pImpl->V = pImpl->params.c;
        pImpl->u += pImpl->params.d;
        
        // Set refractory period (2-5 ms typical)
        pImpl->refractory = true;
        pImpl->refractoryTimeRemaining = 2.0f + (pImpl->params.d / 10.0f);
    }
    
    // Clamp membrane potential to prevent instability
    pImpl->V = std::clamp(pImpl->V, -100.0f, 50.0f);
    
    return pImpl->firing;
}

MembranePotential IzhikevichNeuron::getMembranePotential() const {
    return pImpl->V;
}

bool IzhikevichNeuron::isFiring() const {
    return pImpl->firing;
}

float IzhikevichNeuron::getLastSpikeTime() const {
    return pImpl->lastSpikeTime;
}

void IzhikevichNeuron::reset() {
    pImpl->V = pImpl->params.V_init;
    pImpl->u = pImpl->params.u_init;
    pImpl->I_ext = 0.0f;
    pImpl->I_syn = 0.0f;
    pImpl->firing = false;
    pImpl->lastSpikeTime = -1.0f;
    pImpl->lastSpikeTimestamp = 0.0;
    pImpl->refractory = false;
    pImpl->refractoryTimeRemaining = 0.0f;
    pImpl->spikeHistory.clear();
}

AdvancedNeuronModelType IzhikevichNeuron::getModelType() const {
    return AdvancedNeuronModelType::Izhikevich;
}

const std::any& IzhikevichNeuron::getParameters() const {
    static std::any params = pImpl->params;
    return params;
}

void IzhikevichNeuron::injectCurrent(MembranePotential current) {
    pImpl->I_ext = current;
}

void IzhikevichNeuron::receiveSynapticInput(MembranePotential amplitude) {
    pImpl->I_syn += amplitude;
}

void IzhikevichNeuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > 100) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& IzhikevichNeuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void IzhikevichNeuron::setParameters(const IzhikevichParameters& params) {
    pImpl->params = params;
}

} // namespace nlm
