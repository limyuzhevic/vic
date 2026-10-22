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
    // Real integrate-and-fire dynamics implementation
    if (!neuron) {
        NLM_LOG_ERROR("Cannot update null neuron");
        return;
    }
    
    // Check if neuron is in refractory period
    if (neuron->isRefractory()) {
        // During refractory period, clamp to reset potential
        neuron->setMembranePotential(neuron->getState().resetPotential);
        return;
    }
    
    // LIF dynamics: dV/dt = (V_rest - V) / tau + I / C
    // Euler integration: V_new = V_old + dt * (-(V_old - V_rest) / tau + I / C)
    
    float V = neuron->getMembranePotential();
    float V_rest = neuron->getState().restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;  // ms
    float R = pImpl->membraneResistance;      // MOhm
    
    // Calculate derivative using Euler method
    // Convert dt from seconds to milliseconds
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    float dV = (-(V - V_rest) / tau + I / R) * dt_ms;
    
    // Update membrane potential
    float newV = V + dV;
    
    // Apply spike-frequency adaptation if present
    if (neuron->getState().adaptationVariable > 0.0f) {
        // Slow adaptation: subtract adaptation variable
        newV -= neuron->getState().adaptationVariable * 0.01f;
        // Decay adaptation variable
        neuron->setAdaptationVariable(neuron->getState().adaptationVariable * 0.95f);
    }
    
    // Clamp to reasonable bounds
    newV = std::clamp(newV, -100.0f, 50.0f);
    
    // Check for threshold crossing
    if (newV >= neuron->getThreshold()) {
        // Spike detected
        neuron->setFiringState(FiringState::Active);
        
        // Record spike with current time (0.0 is placeholder, should be actual timestep)
        neuron->recordSpike(0.0);
        
        // Reset membrane potential
        newV = neuron->getState().resetPotential;
        
        // Enter refractory period
        neuron->setRefractoryPeriod(neuron->getState().refractoryPeriod);
        neuron->setFiringState(FiringState::Refractory);
        
        // Increase adaptation for next spike
        neuron->setAdaptationVariable(neuron->getState().adaptationVariable + 1.0f);
    } else {
        neuron->setFiringState(FiringState::Active);
    }
    
    neuron->setMembranePotential(newV);
    
    // Clear total current for next timestep
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics implementation
    if (!synapse) {
        NLM_LOG_ERROR("Cannot update null synapse");
        return;
    }
    
    // Convert dt from seconds to milliseconds for consistency with other parts of the system
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    
    // Update short-term synaptic plasticity using Tsodyks-Markram model
    float& depression = pImpl->shortTermDepression;  // Utilization factor (0-1)
    float& facilitation = pImpl->shortTermFacilitation;  // Facilitation factor
    float& lastPre = pImpl->lastPreSpikeTime;  // Last presynaptic spike time
    float& lastPost = pImpl->lastPostSpikeTime;  // Last postsynaptic spike time
    
    // Decay facilitation (towards 1.0)
    if (lastPre >= 0.0f) {
        float timeSincePre = static_cast<float>(dt_ms - lastPre);
        facilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // Decay depression (towards 1.0, recovery from depression)
    if (lastPost >= 0.0f || lastPre >= 0.0f) {
        float timeSinceActivity = std::max(
            (lastPost >= 0.0f ? static_cast<float>(dt_ms - lastPost) : 0.0f),
            (lastPre >= 0.0f ? static_cast<float>(dt_ms - lastPre) : 0.0f)
        );
        // Recovery from depression
        depression += (1.0f - depression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
    }
    
    // Update synaptic efficacy based on recent activity
    synapse->step(dt_ms);
    
    // Clamp weight bounds to prevent instability
    synapse->setWeight(synapse->getWeight());  // This will clamp within Impl::MIN_WEIGHT/Impl::MAX_WEIGHT
    
    // Update eligibility trace for reward-modulated learning
    synapse->decayEligibilityTrace(0.001f * dt_ms);  // Decay based on timestep
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real spike input application with proper synaptic dynamics
    if (!neuron) {
        NLM_LOG_ERROR("Cannot apply spike input to null neuron");
        return;
    }
    
    if (!synapse) {
        NLM_LOG_ERROR("Cannot apply input from null synapse");
        return;
    }
    
    SynapticWeight weight = synapse->getWeight();
    
    // Apply synaptic input with short-term plasticity modulation
    float depression = pImpl->shortTermDepression;
    float facilitation = pImpl->shortTermFacilitation;
    
    // Effective synaptic weight: base weight * depression * facilitation
    float effectiveWeight = weight * depression * facilitation;
    
    // Apply to neuron based on synapse type
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(effectiveWeight);
    } else if (synapse->isInhibitory()) {
        neuron->receiveInhibitoryInput(std::abs(effectiveWeight));
    } else {
        // For other synapse types, use modulatory input
        neuron->receiveModulatoryInput(effectiveWeight);
    }
    
    // Update synapse spike history
    if (synapse) {
        // Update last spike times for this synapse
        synapse->recordPreSpike(0.0);  // TODO: pass actual time
        synapse->setEligibilityTrace(synapse->getEligibilityTrace() + 1.0f);
    }
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
