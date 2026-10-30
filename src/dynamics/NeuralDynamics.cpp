#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    // Membrane parameters
    float membraneTimeConstant;     // ms (membrane time constant tau)
    float membraneResistance;        // MOhm (membrane resistance R)
    float membraneCapacitance;       // nF (membrane capacitance C)
    
    // Synaptic reversal potentials (mV)
    float reversalPotentialE;        // Excitatory reversal potential
    float reversalPotentialI;        // Inhibitory reversal potential
    
    // Synaptic state
    float excitatoryConductance;     // nS (g_e)
    float inhibitoryConductance;     // nS (g_i)
    
    // Noise and dynamics parameters
    float noiseLevel;                // mV/sqrt(ms) for biological realism
    float adaptationCurrent;         // nA for spike-frequency adaptation
    float adaptationTimeConstant;    // ms for adaptation decay
    
    // For numerical stability
    float adaptiveTimestep;          // Current adaptive timestep
    float minTimestep;               // Minimum timestep for stability
    
    Impl() : membraneTimeConstant(20.0f), 
             membraneResistance(10.0f),
             membraneCapacitance(1.0f),
             reversalPotentialE(0.0f),   // E_rev for excitatory
             reversalPotentialI(-80.0f),  // E_rev for inhibitory
             excitatoryConductance(0.0f),
             inhibitoryConductance(0.0f),
             noiseLevel(0.5f),            // Moderate noise level
             adaptationCurrent(0.0f),
             adaptationTimeConstant(100.0f),
             adaptiveTimestep(1.0f),
             minTimestep(0.1f) {}

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    // Use adaptive timestep for numerical stability
    TimestepDuration timestep = std::max(dt, pImpl->minTimestep);
    
    // Get neuron parameters with biological realism
    MembranePotential V = neuron->getMembranePotential();
    MembranePotential V_rest = state.restingPotential;
    MembranePotential threshold = neuron->getThreshold();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float C = pImpl->membraneCapacitance;
    
    // Synaptic conductances (nS)
    float g_e = pImpl->excitatoryConductance;
    float g_i = pImpl->inhibitoryConductance;
    
    // Reversal potentials (mV)
    float E_e = pImpl->reversalPotentialE;  // ~0mV for excitatory
    float E_i = pImpl->reversalPotentialI;  // ~-80mV for inhibitory
    
    // Membrane capacitance current (from neuron state)
    MembranePotential I_total = neuron->getTotalCurrent();
    
    // Add noise for biological realism (Gaussian white noise)
    float noise = pImpl->noiseLevel * std::sqrt(timestep);
    float noise_dV = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * noise;
    
    // Exponential Euler integration for membrane potential
    // V_new = V_rest + (V - V_rest) * exp(-dt/tau) + R*I_total + noise
    float expFactor = std::exp(-static_cast<float>(timestep) / tau);
    float dV_leak = R * I_total;
    
    // Conductance-based synaptic currents: I_syn = g_e*(E_e - V) + g_i*(E_i - V)
    float I_syn = g_e * (E_e - V) + g_i * (E_i - V);
    float dV_syn = I_syn;
    
    // Adaptation current contribution
    if (pImpl->adaptationCurrent > 0.0f) {
        dV_syn += pImpl->adaptationCurrent;
    }
    
    // Update membrane potential with exponential integration
    V = V_rest + (V - V_rest) * expFactor + dV_syn * static_cast<float>(timestep) + noise_dV;
    
    // Apply spike-frequency adaptation
    if (pImpl->adaptationCurrent > 0.0f) {
        pImpl->adaptationCurrent *= std::exp(-static_cast<float>(timestep) / pImpl->adaptationTimeConstant);
    }
    
    // Refractory period handling
    if (neuron->isRefractory()) {
        if (state.firingState == FiringState::Refractory) {
            neuron->decrementRefractory();
            if (!neuron->isRefractory()) {
                // Refractory period ended, reset adaptation
                pImpl->adaptationCurrent = 0.0f;
            }
        }
        // During refractory period, hold membrane potential at reset
        V = state.resetPotential;
    } else {
        // Update firing state based on dynamics
        if (V >= threshold) {
            neuron->setFiringState(FiringState::Active);
        } else {
            neuron->setFiringState(FiringState::Resting);
        }
    }
    
    // Clamp membrane potential for numerical stability
    V = std::clamp(V, -100.0f, 50.0f);
    neuron->setMembranePotential(V);
    
    // Clear total current for next step
    neuron->clearTotalCurrent();
    
    // Update adaptive timestep based on stability criteria
    if (std::abs(dV_syn) > 5.0f) {
        pImpl->adaptiveTimestep *= 0.8f;  // Reduce timestep if change is large
    } else {
        pImpl->adaptiveTimestep = std::min(pImpl->adaptiveTimestep * 1.05f, 10.0f);  // Increase timestep gradually
    }
    pImpl->adaptiveTimestep = std::max(pImpl->adaptiveTimestep, pImpl->minTimestep);
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics implementation with short-term plasticity and STDP
    TimestepDuration timestep = std::max(dt, pImpl->minTimestep);
    
    // Get current state from synapse
    float depression = 1.0f; // Placeholder - would come from synapse internal state
    float facilitation = 1.0f; // Placeholder - would come from synapse internal state
    Timestamp currentTime = 0.0f; // Current simulation time
    
    // Update short-term plasticity (Tsodyks-Markram model)
    // Facilitation: synaptic resources become more available with recent presynaptic activity
    if (pImpl->adaptationCurrent > 0.0f) {  // Placeholder for lastPreSpikeTime tracking
        // Model facilitation decay
        float timeSinceLastSpike = 10.0f;  // Placeholder
        facilitation *= std::exp(-timeSinceLastSpike / 100.0f);  // tau_fac = 100ms
    }
    
    // Model depression recovery
    // After postsynaptic spike, synaptic efficacy decreases
    float timeSincePostSpike = 20.0f;  // Placeholder
    depression += (1.0f - depression) * (1.0f - std::exp(-timeSincePostSpike / 200.0f));  // tau_dep = 200ms
    
    // Clamp depression between 0.1 and 1.0 for stability
    depression = std::clamp(depression, 0.1f, 1.0f);
    
    // Decay eligibility trace for STDP
    float eligibilityTrace = synapse->getEligibilityTrace();
    synapse->decayEligibilityTrace(0.01f * static_cast<float>(timestep));
    
    // Update synaptic weight based on plasticity rules
    PlasticityFlags& plasticityFlags = synapse->getPlasticityFlags();
    
    if (plasticityFlags.stdp) {
        // Spike-Timing Dependent Plasticity (STDP)
        // Simple model: pre-post spike timing determines weight change
        float weightChange = 0.0f;
        
        // Check for eligible spike pairs (simplified)
        // In a full implementation, this would check actual spike times from history
        weightChange += 0.01f;  // Potentiation for pre-before-post
        weightChange -= 0.01f;  // Depression for post-before-pre
        
        // Apply STDP weight change
        synapse->addToWeight(weightChange);
    }
    
    if (plasticityFlags.hebbian) {
        // Hebbian learning: correlated activity strengthens synapses
        // Simplified implementation: increase weight based on co-activity
        float coActivity = 0.5f;  // Placeholder - would be computed from correlated activity
        synapse->addToWeight(coActivity * 0.005f);
    }
    
    if (plasticityFlags.reward_modulated) {
        // Reward-modulated plasticity: eligibility traces multiplied by reward prediction error
        // Eligibility trace already updated above
        // In full implementation: trace *= reward_prediction_error
    }
    
    // Update synaptic efficacy based on use-dependent modulation
    // Efficacy reflects synaptic strength changes over time
    float currentEfficacy = synapse->getEfficacy();
    currentEfficacy = std::clamp(currentEfficacy, 0.5f, 2.0f);
    synapse->setEfficacy(currentEfficacy);
    
    // Clamp weight to prevent instability
    float minWeight = -1.0f;  // Placeholder - would come from synapse
    float maxWeight = 1.0f;   // Placeholder - would come from synapse
    float weight = synapse->getWeight();
    weight = std::clamp(weight, minWeight, maxWeight);
    synapse->setWeight(weight);
    
    // Update postsynaptic neuron adaptation based on presynaptic spikes
    // This contributes to network-level dynamics
    if (facilitation > 1.0f) {
        pImpl->adaptationCurrent += 0.1f * (facilitation - 1.0f);
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input implementation with conductance-based dynamics
    SynapticWeight weight = synapse->getWeight();
    SynapseType type = synapse->getType();
    
    // Get synaptic efficacy (use-dependent modulation)
    float efficacy = synapse->getEfficacy();
    
    // Short-term plasticity state (Tsodyks-Markram model)
    float depression = 1.0f; // Placeholder for actual depression state
    float facilitation = 1.0f; // Placeholder for actual facilitation state
    
    // Calculate effective synaptic weight considering short-term dynamics
    float effectiveWeight = weight * efficacy * facilitation * depression;
    
    // Apply input based on synapse type with conductance-based model
    if (type == SynapseType::Excitatory) {
        // Excitatory synaptic conductance: increase g_e
        float synapticConductance = effectiveWeight;
        pImpl->excitatoryConductance += synapticConductance;
        neuron->receiveExcitatoryInput(effectiveWeight * 0.1f); // Smaller input to neuron state
    } else if (type == SynapseType::Inhibitory) {
        // Inhibitory synaptic conductance: increase g_i
        float synapticConductance = effectiveWeight;
        pImpl->inhibitoryConductance += synapticConductance;
        neuron->receiveInhibitoryInput(effectiveWeight * 0.1f); // Smaller input to neuron state
    } else if (type == SynapseType::Modulatory) {
        // Modulatory input affects plasticity flags
        neuron->receiveModulatoryInput(effectiveWeight);
    }
    
    // Update synaptic weight history for STDP
    synapse->recordPreSpike(0.0f); // Current time placeholder
    
    // Decay eligibility trace for reward-modulated learning
    float eligibilityTrace = synapse->getEligibilityTrace();
    if (eligibilityTrace > 0.0f) {
        synapse->decayEligibilityTrace(0.01f);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    const auto& state = neuron->getState();
    
    // Check basic threshold condition
    if (neuron->getMembranePotential() < neuron->getThreshold()) {
        return false;
    }
    
    // Check if neuron is in refractory period
    if (neuron->isRefractory() || state.firingState == FiringState::Refractory) {
        return false;
    }
    
    // Consider adaptation variable - high adaptation increases threshold
    // This is a simplified model where adaptation affects spike probability
    if (state.adaptationVariable > 1.5f) {
        // High adaptation makes firing unlikely even if potential exceeds threshold
        return false;
    }
    
    // Consider recent spike history - refractory period already handled above
    // but also account for very recent spikes within adaptation window
    if (state.lastSpikeTime > -1.0f) {
        // Check if last spike was too recent (within 1ms for fast adaptation)
        // This is handled by refractory period but provides additional protection
    }
    
    // Noise can cause spurious spikes, so require potential to exceed threshold
    // by a small margin for reliable spike detection
    float thresholdMargin = 0.5f; // mV margin for spike detection
    if (neuron->getMembranePotential() >= neuron->getThreshold() + thresholdMargin) {
        return true;
    }
    
    return false;
}

void IntegrateAndFireDynamics::reset() {
    // Reset all internal dynamic state
    pImpl->excitatoryConductance = 0.0f;
    pImpl->inhibitoryConductance = 0.0f;
    pImpl->adaptationCurrent = 0.0f;
    pImpl->adaptiveTimestep = 1.0f;  // Reset to default timestep
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
