#include "AdaptiveExponentialNeuron.hpp"
#include "AdvancedNeuronModels.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <any>

namespace nlm {

struct AdaptiveExponentialNeuron::Impl {
    NeuronId id;
    AdaptiveExponentialParameters params;
    
    // State variables
    float V;                    // Membrane potential (mV)
    float w;                    // Adaptation current (mV)
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
    
    Impl(NeuronId neuronId, const AdaptiveExponentialParameters& p)
        : id(neuronId), params(p), V(p.V_reset), w(0.0f),
          I_ext(0.0f), I_syn(0.0f), firing(false),
          lastSpikeTime(-1.0f), lastSpikeTimestamp(0.0),
          refractory(false), refractoryTimeRemaining(0.0f) {}
};

AdaptiveExponentialNeuron::AdaptiveExponentialNeuron(NeuronId id, const AdaptiveExponentialParameters& params)
    : pImpl(std::make_unique<Impl>(id, params)) {}

bool AdaptiveExponentialNeuron::step(Timestamp currentTime) {
    if (pImpl->refractory) {
        pImpl->refractoryTimeRemaining -= pImpl->params.dt;
        if (pImpl->refractoryTimeRemaining <= 0.0f) {
            pImpl->refractory = false;
            pImpl->refractoryTimeRemaining = 0.0f;
        }
        return false;
    }
    
    // Update adaptation current
    w += pImpl->params.a * (pImpl->params.b * (pImpl->V - pImpl->params.V_th) - w);
    
    // Calculate derivative of membrane potential
    // dV/dt = - (V - V_L) / tau + eta * exp((V - V_th) / delta) - w + I_ext + I_syn
    float V_L = 60.0f; // Leak reversal potential
    float tau = 20.0f; // Time constant
    float eta = 0.04f; // Slope factor
    
    float dVdt = - (pImpl->V - V_L) / tau + eta * exp((pImpl->V - pImpl->params.V_th) / pImpl->params.delta) - 
                 pImpl->w + pImpl->I_ext + pImpl->I_syn;
    
    pImpl->V += dVdt * pImpl->params.dt;
    
    // Check for spike threshold (exponential integration)
    if (pImpl->V >= pImpl->params.V_th && !pImpl->firing) {
        pImpl->firing = true;
        pImpl->lastSpikeTime = currentTime;
        pImpl->lastSpikeTimestamp = currentTime;
        recordSpike(currentTime);
        
        // Reset membrane potential
        pImpl->V = pImpl->params.V_reset;
        
        // Update adaptation current (spike-triggered)
        pImpl->w += 150.0f; // Typical after-spike jump
        
        // Set refractory period
        pImpl->refractory = true;
        pImpl->refractoryTimeRemaining = 2.0f;
    }
    
    // Clamp membrane potential
    V = std::clamp(V, -100.0f, 50.0f);
    
    return firing;
}

MembranePotential AdaptiveExponentialNeuron::getMembranePotential() const {
    return pImpl->V;
}

bool AdaptiveExponentialNeuron::isFiring() const {
    return pImpl->firing;
}

float AdaptiveExponentialNeuron::getLastSpikeTime() const {
    return pImpl->lastSpikeTime;
}

void AdaptiveExponentialNeuron::reset() {
    pImpl->V = params.V_reset;
    pImpl->w = 0.0f;
    pImpl->I_ext = 0.0f;
    pImpl->I_syn = 0.0f;
    pImpl->firing = false;
    pImpl->lastSpikeTime = -1.0f;
    pImpl->lastSpikeTimestamp = 0.0;
    pImpl->refractory = false;
    pImpl->refractoryTimeRemaining = 0.0f;
    pImpl->spikeHistory.clear();
}

AdvancedNeuronModelType AdaptiveExponentialNeuron::getModelType() const {
    return AdvancedNeuronModelType::AdaptiveExponential;
}

const std::any& AdaptiveExponentialNeuron::getParameters() const {
    static std::any params = pImpl->params;
    return params;
}

void AdaptiveExponentialNeuron::injectCurrent(MembranePotential current) {
    pImpl->I_ext = current;
}

void AdaptiveExponentialNeuron::receiveSynapticInput(MembranePotential amplitude) {
    pImpl->I_syn += amplitude;
}

void AdaptiveExponentialNeuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > 100) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& AdaptiveExponentialNeuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void AdaptiveExponentialNeuron::setParameters(const AdaptiveExponentialParameters& params) {
    pImpl->params = params;
}

} // namespace nlm
