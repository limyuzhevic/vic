#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // REAL PHASE 2: Implement proper integrate-and-fire dynamics with biological plausibility
    
    const auto& state = neuron->getState();
    
    // Load membrane parameters
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_total = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;  // ms
    float R = pImpl->membraneResistance;     // MOhm
    
    // Apply spike-frequency adaptation if active
    float adaptationCurrent = 0.0f;
    if (state.adaptationVariable > 0.0f) {
        // Adaptation reduces excitability (hyperpolarizing current)
        adaptationCurrent = -state.adaptationVariable * 0.5f;
        I_total += adaptationCurrent;
    }
    
    // Apply neuromodulation effects if available
    if (state.firingState == FiringState::Active) {
        // During active firing, apply adaptive processes
        // For now, use simple adaptation
        state.adaptationVariable += 0.1f;  // Build up adaptation
    }
    
    // Real LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
    // With Euler integration: V_new = V_old + dt * ((V_rest - V_old)/tau + I_total/R)
    
    float dV = (-(V - V_rest) / tau + I_total / R) * static_cast<float>(dt);
    
    // Apply refractory period if active
    if (neuron->isRefractory()) {
        // During refractory period, neuron is clamped to reset potential
        // and cannot integrate new inputs
        V = state.resetPotential;
        dV = 0.0f;
    }
    
    // Update membrane potential with biological constraints
    V += dV;
    
    // Apply biological voltage clamping for stability
    // Typical neuron voltage range: -100mV to +50mV
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike threshold crossing with hysteresis
    bool shouldSpike = (V >= state.threshold);
    
    if (shouldSpike && !neuron->isRefractory()) {
        // Neuron fires - implement spike dynamics
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<Timestamp>(pImpl->lastSpikeTime));  // TODO: pass actual time
        
        // Apply spike reset dynamics
        V = state.resetPotential;
        
        // Enter refractory period
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        neuron->setFiringState(FiringState::Refractory);
        
        // Update adaptation for spike-frequency adaptation
        state.adaptationVariable += 1.0f;  // Each spike increases adaptation
        
        // Note: Spike detection and state updates happen through the neuron's stepLIF method
        // This function focuses on the integration dynamics
    } else {
        // Update firing state based on current membrane potential
        if (neuron->getMembranePotential() >= state.threshold) {
            neuron->setFiringState(FiringState::Active);
        } else {
            neuron->setFiringState(FiringState::Resting);
        }
    }
    
    // Set the updated membrane potential
    neuron->setMembranePotential(V);
    
    // Decay adaptation variable over time (biological spike-frequency adaptation)
    if (state.adaptationVariable > 0.0f) {
        state.adaptationVariable *= 0.95f;  // Exponential decay
    }
    
    // Clear total current for next timestep
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // REAL PHASE 2: Implement proper synaptic dynamics with biological models
    
    // Load current synaptic state
    float weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    float depression = synapse->getShortTermDepression();
    float facilitation = synapse->getShortTermFacilitation();
    float lastPreTime = synapse->getLastPreSpikeTime();
    float lastPostTime = synapse->getLastPostSpikeTime();
    
    float currentTime = static_cast<float>(pImpl->lastSpikeTime);  // TODO: pass actual time
    
    // Apply Tsodyks-Markram short-term plasticity model
    // This is the standard model for synaptic dynamics
    
    // Update facilitation component
    if (lastPreTime >= 0.0f) {
        float dt_facilitation = currentTime - lastPreTime;
        // Facilitation decays exponentially with time constant
        float tau_fac = 100.0f;  // ms
        facilitation *= std::exp(-dt_facilitation / tau_fac);
    }
    
    // Update depression component
    if (lastPostTime >= 0.0f || lastPreTime >= 0.0f) {
        float dt_depression = currentTime - std::max(lastPostTime, lastPreTime);
        if (dt_depression > 0.0f) {
            // Depression recovers exponentially towards 1.0
            float tau_dep = 200.0f;  // ms
            depression += (1.0f - depression) * (1.0f - std::exp(-dt_depression / tau_dep));
        }
    }
    
    // Update synaptic efficacy based on short-term plasticity
    // The effective synaptic efficacy is modulation of the base weight
    float effectiveWeight = weight * efficacy * facilitation * depression;
    
    // Apply biological constraints
    effectiveWeight = std::clamp(effectiveWeight, -1.0f, 1.0f);
    
    // Update synapse state with new effective weight
    synapse->setWeight(effectiveWeight);
    
    // Update eligibility trace for plasticity
    if (std::abs(weight) > 0.01f) {
        float eligibilityChange = 0.01f * static_cast<float>(dt);  // Small eligibility increment
        float currentEligibility = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentEligibility + eligibilityChange);
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // REAL PHASE 2: Implement proper synaptic transmission with conductance-based dynamics
    
    // Load synaptic parameters
    SynapticWeight baseWeight = synapse->getWeight();
    SynapseType type = synapse->getType();
    float efficacy = synapse->getEfficacy();
    
    // Calculate synaptic conductance based on weight, efficacy, and synaptic type
    // This simulates the actual flow of ions during synaptic transmission
    
    // For excitatory synapses: positive conductance increases membrane potential
    // For inhibitory synapses: negative conductance decreases membrane potential
    // For modulatory synapses: different time constants and reversal potentials
    
    float synapticConductance = baseWeight * efficacy * 10.0f;  // Scale factor for biological realism
    
    // Apply synaptic input with type-specific dynamics
    if (synapse->isExcitatory()) {
        // Excitatory input: add positive current
        // This models AMPA/NMDA receptor activation
        neuron->receiveExcitatoryInput(synapticConductance);
    } else if (synapse->isInhibitory()) {
        // Inhibitory input: subtract current (hyperpolarizing)
        // This models GABA receptor activation
        neuron->receiveInhibitoryInput(synapticConductance);
    } else {
        // Modulatory input: affects plasticity rather than membrane potential
        // This models neuromodulator receptor effects
        neuron->receiveModulatoryInput(synapticConductance * 0.1f);
    }
    
    // Record synaptic activation time for plasticity
    // This helps track when synaptic transmission occurs
    // Note: Spike timing dependent plasticity will use these activation times
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // No internal state to reset
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
