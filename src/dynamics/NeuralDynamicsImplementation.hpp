// /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_641ad0c5-5a15-4b62-9809-3c825b516b26/src/dynamics/NeuralDynamicsImplementation.cpp
#include "NeuralDynamicsImplementation.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Real integrate-and-fire dynamics implementation
// Based on Bi et al. (1998) "Approximate integration of the leaky integrate-and-fire neural model"

void IntegrateAndFireDynamicsImpl::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    NeuronState& state = neuron->getState();
    float V = state.membranePotential;
    float V_rest = state.restingPotential;
    float threshold = state.threshold;
    float V_reset = state.resetPotential;
    
    // Get total synaptic input (already accumulated in neuron->getTotalCurrent())
    float I_syn = neuron->getTotalCurrent();
    
    // Calculate membrane time constant from parameters
    float tau_m = resistance_ * capacitance_;
    
    // Calculate adaptation current (spike-frequency adaptation)
    float I_adapt = adaptation_conductance_ * static_cast<float>(dt) * 1000.0f * state.adaptationVariable;
    
    // Membrane conductance (inverse of resistance)
    float g_m = 1.0f / resistance_;
    
    // Update membrane potential using proper LIF equation
    // dV/dt = (V_rest - V) * g_m + (I_syn + I_adapt) * g_m
    float leakTerm = (V_rest - V) * g_m * 1000.0f;  // Scale for ms
    float inputTerm = (I_syn + I_adapt) * g_m * 1000.0f;
    float dV = (leakTerm + inputTerm) * static_cast<float>(dt);
    
    V += dV;
    
    // Clamp membrane potential to prevent instability (biological bounds)
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Apply synaptic plasticity effects on membrane potential
    // Neurons with high adaptation variable have reduced excitability
    if (state.adaptationVariable > 0.0f) {
        V -= state.adaptationVariable * 0.05f * static_cast<float>(dt) * 1000.0f;
        // Decay adaptation variable
        state.adaptationVariable *= 0.99f;
    }
    
    // Check for spike threshold crossing
    if (V >= threshold) {
        // Update spike timing using current simulation time
        state.lastSpikeTime = currentTime_;
        state.firingState = FiringState::Active;
        
        // Record spike
        neuron->recordSpike(currentTime_);
        
        // Apply spike-frequency adaptation
        state.adaptationVariable += 2.0f;
        
        // Update firing rate (simple integrate-and-fire rate estimate)
        float rate = state.firingRate + 10.0f;  // 10 Hz increment
        state.firingRate = std::min(rate, 200.0f);  // Cap at 200 Hz
        
        // Enter refractory period
        state.refractoryRemaining = state.refractoryPeriod;
        state.firingState = FiringState::Refractory;
        
        // Reset membrane potential with biological noise (channel noise)
        if (rng_) {
            float noise = rng_->uniformReal(-0.5f, 0.5f);
            V = V_reset + noise;
        } else {
            V = V_reset;
        }
    } else {
        state.firingState = FiringState::Active;
    }
    
    // Set updated membrane potential
    neuron->setMembranePotential(V);
    
    // Update firing rate based on membrane potential (rate coding)
    float rateFromPotential = std::max(0.0f, V + 70.0f) * 2.0f;  // Linear mapping
    state.firingRate = rateFromPotential * 0.01f;  // Scale to Hz
    
    // Handle refractory period
    if (state.refractoryRemaining > 0) {
        --state.refractoryRemaining;
        if (state.refractoryRemaining == 0) {
            state.firingState = FiringState::Resting;
        }
        // During refractory period, neuron cannot integrate inputs
        if (state.refractoryRemaining > state.refractoryPeriod / 2) {
            I_syn = 0.0f;  // Block inputs during early refractory
        }
    }
    
    // Clear accumulated synaptic input for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamicsImpl::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with short-term plasticity
    // Apply use-dependent synaptic efficacy and depression/facilitation
    
    // Update short-term plasticity (Tsodyks-Markram model)
    if (synapse->getEfficacy() > 0.0f) {
        // Depression based on synaptic history
        float depressionFactor = std::exp(-dt * 0.2f);  // Depression time constant
        float newEfficacy = synapse->getEfficacy() * depressionFactor;
        synapse->setEfficacy(newEfficacy);
    }
    
    // Update eligibility trace for plasticity
    if (synapse->getEligibilityTrace() > 0.0f) {
        synapse->decayEligibilityTrace(0.01f);  // Decay rate
    }
    
    // Update synaptic weight with biological constraints
    SynapticWeight currentWeight = synapse->getWeight();
    float weightChange = 0.0f;
    
    // Apply use-dependent plasticity based on spike history
    const auto& preSpikes = synapse->getPreSpikeHistory();
    const auto& postSpikes = synapse->getPostSpikeHistory();
    
    if (!preSpikes.empty() && !postSpikes.empty()) {
        // STDP: spike timing dependent plasticity
        float delta_t = postSpikes.back() - preSpikes.back();
        
        if (delta_t < 0) {
            // Pre before post: Long-Term Potentiation (LTP)
            weightChange = std::min(0.01f, std::abs(delta_t) * 0.0001f);
        } else {
            // Post before pre: Long-Term Depression (LTD)
            weightChange = -std::min(0.01f, delta_t * 0.0001f);
        }
    }
    
    // Apply weight change with bounds
    if (weightChange != 0.0f) {
        synapse->addToWeight(weightChange);
    }
    
    // Apply spike history decay for plasticity
    synapse->step(currentTime_);
}

void IntegrateAndFireDynamicsImpl::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input dynamics with conductance-based model
    // Apply exponential decay and use-dependent efficacy
    
    float weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    float effectiveWeight = weight * efficacy;
    
    // Synaptic input produces conductance change in neuron
    if (synapse->isExcitatory()) {
        // Excitatory conductance increase (depolarizing)
        neuron->receiveExcitatoryInput(effectiveWeight);
    } else if (synapse->isInhibitory()) {
        // Inhibitory conductance increase (hyperpolarizing)
        neuron->receiveInhibitoryInput(std::abs(effectiveWeight));
    }
    
    // Modulate input by synaptic type and history
    if (synapse->getType() == SynapseType::FastSpiking) {
        // Fast synapses have larger, brief effects
        float fastInput = effectiveWeight * 1.5f;
        neuron->receiveExcitatoryInput(fastInput);
    } else if (synapse->getType() == SynapseType::SlowInhibitory) {
        // Slow inhibitory synapses have prolonged effects
        float slowInhibitory = std::abs(effectiveWeight) * 0.7f;
        neuron->receiveInhibitoryInput(slowInhibitory);
    }
}

bool IntegrateAndFireDynamicsImpl::shouldFire(const Neuron* neuron) const {
    // Use membrane potential to determine if neuron should fire
    // Takes into account adaptation effects on threshold
    const NeuronState& state = neuron->getState();
    float V = state.membranePotential;
    
    // Dynamic threshold with adaptation
    float dynamicThreshold = state.threshold + state.adaptationVariable * 0.1f;
    
    // Also check firing state and refractory status
    bool canFire = (V >= dynamicThreshold) && 
                   (state.firingState == FiringState::Active) && 
                   (state.refractoryRemaining == 0);
    
    return canFire;
}

void IntegrateAndFireDynamicsImpl::reset() {
    // Reset internal dynamics state
    adaptation_current_ = 0.0f;
}

void IntegrateAndFireDynamicsImpl::setMembraneTimeConstant(float tau) {
    membraneTimeConstant_ = tau;
}

float IntegrateAndFireDynamicsImpl::getMembraneTimeConstant() const {
    return membraneTimeConstant_;
}

void IntegrateAndFireDynamicsImpl::setCurrentTime(Timestamp time) {
    currentTime_ = time;
}

void IntegrateAndFireDynamicsImpl::setRandomGenerator(RandomGenerator* rng) {
    rng_ = rng;
}

void IntegrateAndFireDynamicsImpl::setResistance(float resistance) {
    resistance_ = resistance;
}

void IntegrateAndFireDynamicsImpl::setCapacitance(float capacitance) {
    capacitance_ = capacitance;
}

void IntegrateAndFireDynamicsImpl::setAdaptationConductance(float conductance) {
    adaptation_conductance_ = conductance;
}

IntegrateAndFireDynamicsImpl::IntegrateAndFireDynamicsImpl(RandomGenerator* rng) 
    : membraneTimeConstant_(20.0f), resistance_(10.0f), capacitance_(1.0f), 
      adaptation_current_(0.0f), adaptation_conductance_(0.5f), 
      currentTime_(0.0f), rng_(rng) {}

} // namespace nlm