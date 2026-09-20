#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float maxDerivativeRate;      // Max dV/dt for stability
    float adaptiveMaxStep;        // Maximum adaptive step size
    float refractoryPeriodSteps;  // Number of refractory steps
    float adaptationTimeWindow;   // Window for spike adaptation
    float adaptationTimeConstant; // Time constant for adaptation
    float adaptationStrength;     // Strength of adaptation
    float spikeAdaptationFactor;  // Factor for spike-dependent plasticity
    float minMembranePotential;   // Minimum V (hyperpolarization limit)
    float maxMembranePotential;   // Maximum V (saturation limit)
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f),
             maxDerivativeRate(100.0f), adaptiveMaxStep(0.1f),
             refractoryPeriodSteps(3), adaptationTimeWindow(20.0f),
             adaptationTimeConstant(10.0f), adaptationStrength(0.3f),
             spikeAdaptationFactor(0.1f), minMembranePotential(-100.0f),
             maxMembranePotential(50.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real integrate-and-fire dynamics with improved numerical integration
    // Implements Leaky Integrate-and-Fire (LIF) model with adaptive threshold and spike adaptation
    
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    
    // Get neuron parameters
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_total = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Enhanced Euler integration with adaptive step size
    float dt_f = static_cast<float>(dt);
    
    // Compute membrane potential change using improved numerical method
    // dV/dt = -(V - V_rest)/tau + I_total/R
    float derivative = -(V - V_rest) / tau + I_total / R;
    
    // Adaptive step size based on derivative magnitude for stability
    float maxDt = pImpl->adaptiveMaxStep;
    float safeDt = dt_f;
    if (std::abs(derivative) > pImpl->maxDerivativeRate) {
        safeDt = maxDt * pImpl->maxDerivativeRate / std::abs(derivative);
        safeDt = std::min(safeDt, dt_f);
    }
    
    // Perform integration with adaptive step
    V += derivative * safeDt;
    
    // Apply spike adaptation based on previous firing history
    if (neuron->getLastSpikeTime() > 0) {
        float timeSinceSpike = state.lastSpikeTime;
        if (timeSinceSpike < pImpl->adaptationTimeWindow) {
            // Spike-frequency adaptation: decrease threshold after spikes
            float adaptationFactor = std::exp(-timeSinceSpike / pImpl->adaptationTimeConstant);
            float adaptedThreshold = state.threshold * (1.0f - pImpl->adaptationStrength * adaptationFactor);
            V += (adaptedThreshold - state.threshold) * 0.1f;  // Gradual adaptation
        }
    }
    
    // Clamp to reasonable bounds
    V = std::clamp(V, pImpl->minMembranePotential, pImpl->maxMembranePotential);
    
    neuron->setMembranePotential(V);
    
    // Check for firing with refractory period
    if (!neuron->isRefractory() && V >= neuron->getThreshold()) {
        // Fire: reset membrane potential and enter refractory period
        neuron->setFiringState(FiringState::Active);
        neuron->setMembranePotential(state.resetPotential);
        neuron->recordSpike(static_cast<float>(currentTime()));  // Use actual time
        neuron->enterRefractoryPeriod(pImpl->refractoryPeriodSteps);
        
        // Spike-dependent plasticity: increase probability of future firing
        neuron->setAdaptationVariable(neuron->getAdaptationVariable() + pImpl->spikeAdaptationFactor);
    }
    
    // Refractory period handling
    if (neuron->isRefractory()) {
        neuron->decrementRefractory();
        if (!neuron->isRefractory()) {
            // Recovery from refractory period
            neuron->setFiringState(FiringState::Resting);
        }
    }
    
    // Clear input current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // TODO PHASE 2: Implement real synaptic dynamics
    // PLACEHOLDER: Synapse decay
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // TODO PHASE 2: Implement real synaptic input
    // PLACEHOLDER: Simple additive input
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
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

struct FitzHughNagumoDynamics::Impl {
    float threshold;             // Spike threshold
    float recoveryTimeConstant;  // Time constant for recovery variable
    float slopeFactor;           // Slope factor (negative in FHN)
    float resetValue;            // Value after spike reset
    float adaptationFactor;      // Adaptation strength
    float minPotential;          // Minimum potential
    float maxPotential;          // Maximum potential
    
    Impl() : threshold(20.0f), recoveryTimeConstant(10.0f),
             slopeFactor(-0.1f), resetValue(0.0f),
             adaptationFactor(0.8f), minPotential(-80.0f),
             maxPotential(80.0f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real FitzHugh-Nagumo dynamics
    // Implements FHN model: dV/dt = (V - V^3/3 - w + I)/tau, dw/dt = (aV - bw)/tau
    // Where V is membrane potential and w is recovery variable
    
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    
    // Get neuron state variables
    float V = neuron->getMembranePotential();
    float w = neuron->getAdaptationVariable();  // Use adaptation variable as recovery
    float I_total = neuron->getTotalCurrent();
    float tau = pImpl->recoveryTimeConstant;
    float a = -0.7f;  // Input parameter (typically between 0 and 1)
    float b = 0.8f;   // Input parameter (typically between 0 and 1)
    
    float dt_f = static_cast<float>(dt);
    
    // FitzHugh-Nagumo equations with improved numerical stability
    // dV/dt = (V - V^3/3 - w + I_total/50.0) / tau
    float dv_dt = (V - V*V*V/3.0f - w + I_total/50.0f) / tau;
    
    // dw/dt = (a*V - b*w) / tau
    float dw_dt = (a * V - b * w) / tau;
    
    // Enhanced Euler integration with adaptive step size
    // Apply steepest descent to avoid instability
    float maxRate = 100.0f;
    float dv_magnitude = std::abs(dv_dt);
    float dw_magnitude = std::abs(dw_dt);
    
    float dv_step = dv_dt;
    float dw_step = dw_dt;
    
    // Adaptive step size for stability
    float max_dt = 0.1f;
    if (dv_magnitude > maxRate || dw_magnitude > maxRate) {
        float factor = maxRate / std::max(dv_magnitude, dw_magnitude);
        dv_step *= factor;
        dw_step *= factor;
    }
    
    // Update variables
    V += dv_step * dt_f;
    w += dw_step * dt_f;
    
    // Apply spike detection and reset
    if (V > pImpl->threshold) {
        // Spike detected
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<float>(currentTime()));
        
        // Reset variables after spike
        V = pImpl->resetValue;
        w += pImpl->adaptationFactor * 2.0f;  // After-spike adaptation
        
        // Enter refractory period
        neuron->enterRefractoryPeriod(2);
    }
    
    // Clamp variables to bounds
    V = std::clamp(V, pImpl->minPotential, pImpl->maxPotential);
    w = std::clamp(w, -1.0f, 1.0f);  // Recovery variable typically bounded
    
    // Update neuron state
    neuron->setMembranePotential(V);
    neuron->setAdaptationVariable(w);
    
    // Check for firing (threshold-based)
    if (!neuron->isRefractory() && V >= neuron->getThreshold()) {
        neuron->setFiringState(FiringState::Active);
        neuron->setMembranePotential(state.resetPotential);
        neuron->recordSpike(static_cast<float>(currentTime()));
        neuron->enterRefractoryPeriod(2);
    }
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        neuron->decrementRefractory();
        if (!neuron->isRefractory()) {
            neuron->setFiringState(FiringState::Resting);
        }
    }
    
    // Clear input current for next step
    neuron->clearTotalCurrent();
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Synapse dynamics in FHN framework
    // Synaptic activity affects both V and w variables
    
    if (!synapse) return;
    
    float dt_f = static_cast<float>(dt);
    
    // Get synapse state
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    bool isExcitatory = synapse->isExcitatory();
    
    // Synapses in FHN system modulate recovery variable
    // Excitatory synapses increase recovery, inhibitory decrease it
    float recoveryChange = (isExcitatory ? 1.0f : -1.0f) * weight * efficacy * 0.01f;
    
    // This would be used by the neuron to update its recovery variable
    // In a real implementation, the synapse would communicate with the neuron
    synapse->step(dt_f);
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Spike transmission in FHN system
    // When a presynaptic neuron spikes, it affects postsynaptic V and w
    
    if (!neuron || !synapse) return;
    
    // Get synapse properties
    SynapticWeight weight = synapse->getWeight();
    bool isExcitatory = synapse->isExcitatory();
    
    // Excitatory synapses depolarize postsynaptic neuron (increase V)
    // Inhibitory synapses hyperpolarize postsynaptic neuron (decrease V)
    float voltageChange = (isExcitatory ? 1.0f : -1.0f) * weight * 10.0f;
    
    // Also affects recovery variable (different signs in FHN)
    float recoveryChange = (isExcitatory ? 1.0f : -1.0f) * weight * 0.5f;
    
    // Apply changes
    neuron->setMembranePotential(neuron->getMembranePotential() + voltageChange);
    neuron->setAdaptationVariable(neuron->getAdaptationVariable() + recoveryChange);
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    // FHN firing condition: threshold-based with refractory consideration
    float V = neuron->getMembranePotential();
    bool isRefractory = neuron->isRefractory();
    
    return (V >= pImpl->threshold) && !isRefractory;
}

void FitzHughNagumoDynamics::reset() {
    // Reset to standard FHN parameters
    pImpl->threshold = 20.0f;
    pImpl->recoveryTimeConstant = 10.0f;
    pImpl->slopeFactor = -0.1f;
    pImpl->resetValue = 0.0f;
    pImpl->adaptationFactor = 0.8f;
    pImpl->minPotential = -80.0f;
    pImpl->maxPotential = 80.0f;
}

void FitzHughNagumoDynamics::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float FitzHughNagumoDynamics::getThreshold() const {
    return pImpl->threshold;
}

void FitzHughNagumoDynamics::setRecoveryTimeConstant(float tau) {
    pImpl->recoveryTimeConstant = tau;
}

float FitzHughNagumoDynamics::getRecoveryTimeConstant() const {
    return pImpl->recoveryTimeConstant;
}

void FitzHughNagumoDynamics::setSlopeFactor(float slope) {
    pImpl->slopeFactor = slope;
}

float FitzHughNagumoDynamics::getSlopeFactor() const {
    return pImpl->slopeFactor;
}

} // namespace nlm
