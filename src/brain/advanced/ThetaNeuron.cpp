#include "ThetaNeuron.hpp"
#include "AdvancedNeuronModels.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <any>

namespace nlm {

struct ThetaNeuron::Impl {
    NeuronId id;
    ThetaNeuronParameters params;
    
    // State variables
    float V;                    // Membrane potential (mV)
    float omega;                // Firing frequency (Hz)
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
    
    Impl(NeuronId neuronId, const ThetaNeuronParameters& p)
        : id(neuronId), params(p), V(p.V_reset), omega(p.omega_0),
          I_ext(0.0f), I_syn(0.0f), firing(false),
          lastSpikeTime(-1.0f), lastSpikeTimestamp(0.0),
          refractory(false), refractoryTimeRemaining(0.0f) {}
};

ThetaNeuron::ThetaNeuron(NeuronId id, const ThetaNeuronParameters& params)
    : pImpl(std::make_unique<Impl>(id, params)) {}

bool ThetaNeuron::step(Timestamp currentTime) {
    if (pImpl->refractory) {
        pImpl->refractoryTimeRemaining -= pImpl->params.dt;
        if (pImpl->refractoryTimeRemaining <= 0.0f) {
            pImpl->refractory = false;
            pImpl->refractoryTimeRemaining = 0.0f;
        }
        return false;
    }
    
    // Update firing frequency
    pImpl->omega = pImpl->params.omega_0 + pImpl->params.delta * pImpl->I_ext;
    
    // Calculate membrane potential using theta model
    // V = V_th - V_reset * exp((omega - omega_0) / delta)
    pImpl->V = pImpl->params.V_th - pImpl->params.V_reset * 
               exp((pImpl->omega - pImpl->params.omega_0) / pImpl->params.delta);
    
    // Add synaptic input
    pImpl->V += pImpl->I_syn;
    
    // Check for spike threshold
    if (pImpl->V >= pImpl->params.V_th && !pImpl->firing) {
        pImpl->firing = true;
        pImpl->lastSpikeTime = currentTime;
        pImpl->lastSpikeTimestamp = currentTime;
        recordSpike(currentTime);
        
        // Reset membrane potential
        pImpl->V = pImpl->params.V_reset;
        
        // Set refractory period (1-2 ms typical for theta neurons)
        pImpl->refractory = true;
        pImpl->refractoryTimeRemaining = 1.5f;
    }
    
    // Clamp membrane potential
    pImpl->V = std::clamp(pImpl->V, -100.0f, 50.0f);
    
    return pImpl->firing;
}

MembranePotential ThetaNeuron::getMembranePotential() const {
    return pImpl->V;
}

bool ThetaNeuron::isFiring() const {
    return pImpl->firing;
}

float ThetaNeuron::getLastSpikeTime() const {
    return pImpl->lastSpikeTime;
}

void ThetaNeuron::reset() {
    pImpl->V = pImpl->params.V_reset;
    pImpl->omega = pImpl->params.omega_0;
    pImpl->I_ext = 0.0f;
    pImpl->I_syn = 0.0f;
    pImpl->firing = false;
    pImpl->lastSpikeTime = -1.0f;
    pImpl->lastSpikeTimestamp = 0.0;
    pImpl->refractory = false;
    pImpl->refractoryTimeRemaining = 0.0f;
    pImpl->spikeHistory.clear();
}

AdvancedNeuronModelType ThetaNeuron::getModelType() const {
    return AdvancedNeuronModelType::Theta;
}

const std::any& ThetaNeuron::getParameters() const {
    static std::any params = pImpl->params;
    return params;
}

void ThetaNeuron::injectCurrent(MembranePotential current) {
    pImpl->I_ext = current;
}

void ThetaNeuron::receiveSynapticInput(MembranePotential amplitude) {
    pImpl->I_syn += amplitude;
}

void ThetaNeuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > 100) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& ThetaNeuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void ThetaNeuron::setParameters(const ThetaNeuronParameters& params) {
    pImpl->params = params;
}

} // namespace nlm
