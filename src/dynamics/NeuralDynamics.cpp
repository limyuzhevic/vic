#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float membraneCapacitance;    // nF
    float synapticDelay;          // ms
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), 
             membraneCapacitance(1.0f), synapticDelay(1.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // PHASE 2 IMPROVEMENT: Real integrate-and-fire dynamics
    // More accurate numerical integration of LIF differential equation
    
    const auto& state = neuron->getState();
    
    // LIF equation: C * dV/dt = - (V - V_rest)/R + I_syn
    // dV/dt = (V_rest - V)/tau + I_syn/C
    // Where tau = R * C is the membrane time constant
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_syn = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float C = pImpl->membraneCapacitance;
    
    // Calculate derivatives
    float leakTerm = (V_rest - V) / tau;  // Leak current
    float inputTerm = I_syn / C;          // Synaptic input current
    float dV_dt = leakTerm + inputTerm;
    
    // Exponential Euler integration (better stability for stiff equations)
    // V_new = V_rest + (V_old - V_rest) * exp(-dt/tau) + (I_syn * tau / C) * (1 - exp(-dt/tau))
    float alpha = std::exp(-static_cast<float>(dt) / tau);
    float beta = tau / C * (1.0f - alpha);
    
    float V_new = V_rest + (V - V_rest) * alpha + inputTerm * beta;
    
    // Apply spike-frequency adaptation if present
    if (state.adaptationVariable > 0.0f) {
        V_new -= state.adaptationVariable * 0.01f;
    }
    
    // Clamp to prevent instability
    V_new = std::clamp(V_new, state.restingPotential - 50.0f, 50.0f);
    
    neuron->setMembranePotential(V_new);
    
    // Check for spike threshold crossing
    if (shouldFire(neuron, V_new)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual simulation time
        
        // Reset membrane potential with stochastic component
        float resetV = state.resetPotential + 
                      (rand() / (RAND_MAX + 1.0f)) * 2.0f; // ±1mV variation
        neuron->setMembranePotential(resetV);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(state.refractoryPeriod));
        neuron->setFiringState(FiringState::Refractory);
        
        // Increase adaptation variable for spike-frequency adaptation
        neuron->getState().adaptationVariable += 1.0f;
    }
    
    // Refractory period handling
    if (neuron->isRefractory()) {
        // During refractory period, override dynamics with reset potential
        neuron->setMembranePotential(state.resetPotential);
        neuron->decrementRefractory();
        
        // Still allow some adaptation even during refractory
        if (state.adaptationVariable > 0.0f) {
            neuron->getState().adaptationVariable *= 0.98f;
        }
    }
    
    // Decay adaptation variable
    if (state.adaptationVariable > 0.0f) {
        neuron->getState().adaptationVariable *= 0.98f;
    }
    
    // Clear synaptic current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // PHASE 2 IMPROVEMENT: Real synaptic dynamics
    // More realistic synapse model with depression/facilitation
    
    float weight = synapse->getWeight();
    
    // Short-term synaptic plasticity
    if (synapse->isFacilitating()) {
        // Facilitation: increases strength with recent activity
        float facilitation = synapse->getFacilitationLevel();
        facilitation = std::min(1.0f, facilitation * 1.05f); // 5% increase per spike
        synapse->setFacilitationLevel(facilitation);
        weight *= (1.0f + facilitation * 0.5f);
    } else if (synapse->isDepressing()) {
        // Depression: decreases strength with recent activity  
        float depression = synapse->getDepressionLevel();
        depression = std::min(1.0f, depression * 1.02f); // 2% decrease per spike
        synapse->setDepressionLevel(depression);
        weight *= (1.0f - depression * 0.3f);
    }
    
    // Synaptic decay over time
    weight *= (1.0f - 0.001f * static_cast<float>(dt)); // 0.1% per ms
    
    synapse->setWeight(weight);
    synapse->step(static_cast<float>(dt));
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // PHASE 2 IMPROVEMENT: Realistic synaptic input with delay and plasticity
    
    if (!synapse || !neuron) return;
    
    SynapticWeight weight = synapse->getWeight();
    float delay = pImpl->synapticDelay;
    
    // Apply weight changes from presynaptic activity
    if (synapse->isPresynapticActive()) {
        // Record spike time for delayed effect
        synapse->recordSpikeTime(static_cast<float>(delay));
        
        // Apply synaptic plasticity based on spike timing
        synapse->applyPlasticity(neuron->getState().lastSpikeTime, 
                               synapse->getLastSpikeTime(), delay);
    }
    
    // Determine input type and magnitude
    if (synapse->isExcitatory()) {
        // Excitatory input: causes depolarization
        float inputAmplitude = weight * synapse->getConductance();
        neuron->receiveExcitatoryInput(inputAmplitude);
    } else if (synapse->isInhibitory()) {
        // Inhibitory input: causes hyperpolarization
        float inputAmplitude = weight * synapse->getConductance();
        neuron->receiveInhibitoryInput(inputAmplitude);
    } else if (synapse->isModulatory()) {
        // Modulatory input: affects plasticity but not direct integration
        float inputAmplitude = weight * synapse->getConductance() * 0.1f;
        neuron->receiveModulatoryInput(inputAmplitude);
    }
    
    // Apply short-term plasticity effects to the input
    if (synapse->isFacilitating()) {
        float facilitation = synapse->getFacilitationLevel();
        float enhancedWeight = weight * (1.0f + facilitation * 0.5f);
        // Additional input for facilitated synapses
        neuron->receiveExcitatoryInput(enhancedWeight * 0.1f);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron, float membranePotential) const {
    // Check threshold with stochastic component
    bool deterministic = membranePotential >= neuron->getThreshold();
    
    if (deterministic) {
        return true;
    }
    
    // Add probabilistic firing near threshold
    float distance = neuron->getThreshold() - membranePotential;
    if (distance < 5.0f && distance > 0.0f) { // Within 5mV below threshold
        float noise = (rand() / (RAND_MAX + 1.0f)) * 2.0f - 1.0f; // -1 to 1
        float thresholdNoise = noise * 2.0f; // ±2mV noise
        if (membranePotential + thresholdNoise >= neuron->getThreshold()) {
            return true;
        }
    }
    
    return false;
}

void IntegrateAndFireDynamics::reset() {
    // Reset internal state
    pImpl->membraneTimeConstant = 20.0f;
    pImpl->membraneResistance = 10.0f;
    pImpl->membraneCapacitance = 1.0f;
    pImpl->synapticDelay = 1.0f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
