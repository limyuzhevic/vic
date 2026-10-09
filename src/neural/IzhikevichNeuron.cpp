#include "IzhikevichNeuron.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <random>

namespace nlm {

IzhikevichNeuron::IzhikevichNeuron(NeuronId id, const Parameters& params)
    : Neuron(id)
    , params_(params)
    , u_(0.0f) {
    // Set neuron properties based on Izhikevich model
    setRestingPotential(params.vRest);
    setThreshold(params.vPeak);
    setResetPotential(params.c);
    setLeakConductance(params.gL);
    
    // Set initial recovery variable
    u_ = params.b * (getMembranePotential() - params.vRest);
}

bool IzhikevichNeuron::stepIzhikevich(float dt, 
                                     std::function<void(const IzhikevichNeuron&, float)> spikeCallback) {
    // Izhikevich model equations:
    // dv/dt = (0.04v^2 + 5v + 140 - u + I) / Cm
    // du/dt = a(b(v - vr) - u)
    
    // Get total current (from synaptic inputs + external injections)
    float totalCurrent = getTotalCurrent() / params_.cm;
    
    // Update recovery variable
    float dv = (0.04f * getMembranePotential() * getMembranePotential() +
                5.0f * getMembranePotential() + 140.0f -
                u_ + totalCurrent);
    
    float du = params_.a * (params_.b * (getMembranePotential() - params_.vRest) - u_);
    
    // Update variables
    float newV = getMembranePotential() + dv * dt;
    float newU = u_ + du * dt;
    
    // Check for spike
    bool fired = false;
    if (newV >= params_.vPeak) {
        // Spike occurred
        fired = true;
        newV = params_.c; // Reset voltage
        newU += params_.d; // Increment recovery variable
        
        // Record spike
        recordSpike(static_cast<float>(getId()));
        
        // Call spike callback if provided
        if (spikeCallback) {
            spikeCallback(*this, static_cast<float>(getId()));
        }
        
        NLM_LOG_DEBUG("IzhikevichNeuron " + std::to_string(getId()) + 
                     " fired at V=" + std::to_string(getMembranePotential()) +
                     ", u=" + std::to_string(u_));
    }
    
    // Update state
    setMembranePotential(newV);
    u_ = newU;
    
    return fired;
}

Parameters IzhikevichNeuron::getRegularSpikingParameters() {
    Parameters p;
    p.a = -0.02f;
    p.b = 0.2f;
    p.c = -65.0f;
    p.d = 8.0f;
    p.vPeak = 30.0f;
    p.vRest = -70.0f;
    p.cm = 1.0f;
    p.gL = 0.05f;
    p.EL = -70.0f;
    return p;
}

Parameters IzhikevichNeuron::getIntrinsicBurstingParameters() {
    Parameters p;
    p.a = -0.02f;
    p.b = 0.2f;
    p.c = -50.0f;
    p.d = 150.0f;
    p.vPeak = 30.0f;
    p.vRest = -65.0f;
    p.cm = 1.0f;
    p.gL = 0.05f;
    p.EL = -65.0f;
    return p;
}

Parameters IzhikevichNeuron::getChediumSpikingParameters() {
    Parameters p;
    p.a = 0.025f;
    p.b = -0.2f;
    p.c = -60.0f;
    p.d = 0.0f;
    p.vPeak = 30.0f;
    p.vRest = -70.0f;
    p.cm = 1.0f;
    p.gL = 0.05f;
    p.EL = -70.0f;
    return p;
}

Parameters IzhikevichNeuron::getFastSpikingParameters() {
    Parameters p;
    p.a = 0.1f;
    p.b = 0.2f;
    p.c = -65.0f;
    p.d = 8.0f;
    p.vPeak = 30.0f;
    p.vRest = -70.0f;
    p.cm = 1.0f;
    p.gL = 0.05f;
    p.EL = -70.0f;
    return p;
}

Parameters IzhikevichNeuron::getSlowSpikingParameters() {
    Parameters p;
    p.a = -0.02f;
    p.b = 0.05f;
    p.c = -60.0f;
    p.d = 2.0f;
    p.vPeak = 30.0f;
    p.vRest = -70.0f;
    p.cm = 1.0f;
    p.gL = 0.05f;
    p.EL = -70.0f;
    return p;
}

} // namespace nlm
