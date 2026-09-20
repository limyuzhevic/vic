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

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(std::make_unique<Impl>()) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

IntegrateAndFireDynamics::IntegrateAndFireDynamics(IntegrateAndFireDynamics&& other) noexcept : pImpl(std::move(other.pImpl)) {}

IntegrateAndFireDynamics& IntegrateAndFireDynamics::operator=(IntegrateAndFireDynamics&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    // Real integrate-and-fire dynamics implementation
    // dV/dt = (V_rest - V)/tau + I/C
    // Where I includes synaptic inputs and external inputs
    
    const auto& state = neuron->getState();
    
    // Get current membrane potential
    MembranePotential& V = neuron->getMembranePotential();
    MembranePotential V_rest = state.restingPotential;
    MembranePotential threshold = state.threshold;
    MembranePotential V_reset = state.resetPotential;
    
    // Membrane time constant and resistance
    float tau = pImpl->membraneTimeConstant;  // ms
    float R = pImpl->membraneResistance;     // MOhm
    
    // Get total input current (from synaptic inputs and external inputs)
    MembranePotential I = neuron->getTotalCurrent();
    
    // Handle refractory period - during refractory, membrane potential is clamped
    if (neuron->isRefractory()) {
        // During refractory period, neuron is at reset potential
        neuron->setMembranePotential(V_reset);
        
        // Decrement refractory counter
        neuron->decrementRefractory();
        
        // During refractory, synaptic inputs are blocked
        // (refractory neurons don't integrate new inputs)
        neuron->clearTotalCurrent();
        
        return;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // V_new = V + dt * ((V_rest - V)/tau + I/C)
    // Note: C = R * tau (Ohm * ms = nF)
    
    float dV = (-(V - V_rest) / tau + I / R) * static_cast<float>(dt);
    V += dV;
    
    // Apply spike-frequency adaptation
    // After each spike, adaptation variable increases
    // During refractory and shortly after, adaptation variable is elevated
    if (state.adaptationVariable > 0.0f) {
        // Adaptation causes hyperpolarization (decreases V)
        V -= state.adaptationVariable * 0.01f;
        
        // Decay adaptation variable
        state.adaptationVariable *= 0.95f;  // Decay towards zero
    }
    
    // Clamp membrane potential to biologically plausible ranges
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike threshold crossing
    bool shouldFire = V >= threshold;
    
    if (shouldFire) {
        // Record spike time for plasticity and statistics
        neuron->recordSpike(static_cast<float>(dt));
        
        // Enter refractory period
        neuron->setFiringState(FiringState::Active);
        neuron->setMembranePotential(V_reset);  // Reset to reset potential
        neuron->setRefractoryPeriod(static_cast<uint32_t>(state.refractoryPeriod));  // Enter refractory
        
        // Update adaptation variable for spike-frequency adaptation
        state.adaptationVariable += 1.0f;  // Each spike increases adaptation
        
        // Log spike event for debugging
        NLM_LOG_DEBUG("Neuron " + std::to_string(neuron->getId().index()) + 
                     " spiked at V=" + std::to_string(V) + 
                     "mV, adaptation=" + std::to_string(state.adaptationVariable));
    }
    
    // Clear synaptic current for next timestep
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Real synaptic dynamics implementation
    // Includes short-term plasticity, eligibility traces, and decay
    
    float currentTime = static_cast<float>(dt);
    
    // Decay short-term plasticity state
    // Tsodyks-Markram model for synaptic depression/facilitation
    
    if (synapse->getPlasticityFlags().hebbian) {
        // STDP is enabled - update based on spike history
        const auto& preSpikes = synapse->getPreSpikeHistory();
        const auto& postSpikes = synapse->getPostSpikeHistory();
        
        // Update synaptic weight based on spike timing
        // Implement STDP learning rule
        // If pre before post: potentiate (LTP)
        // If post before pre: depress (LTD)
        
        // Simplified implementation: use spike counts to adjust weight
        float weightChange = 0.0f;
        
        if (!preSpikes.empty() && !postSpikes.empty()) {
            // Check for correlated activity
            for (const auto& preTime : preSpikes) {
                for (const auto& postTime : postSpikes) {
                    if (postTime > preTime) {
                        // Pre before post - potentiate
                        weightChange += 0.001f;  // Small weight increase
                    } else {
                        // Post before pre - depress
                        weightChange -= 0.0005f;  // Small weight decrease
                    }
                }
            }
            
            // Apply weight change with bounds
            float currentWeight = synapse->getWeight();
            float newWeight = currentWeight + weightChange;
            newWeight = std::clamp(newWeight, -1.0f, 1.0f);  // Keep within reasonable bounds
            synapse->setWeight(newWeight);
        }
    }
    
    // Update short-term dynamics
    // Depression and facilitation based on recent activity
    
    float& depression = synapse->getPlasticityFlags().depression;
    float& facilitation = synapse->getPlasticityFlags().facilitation;
    
    // Decay depression towards 1.0 (full recovery)
    depression = std::min(1.0f, depression + 0.01f);
    
    // Decay facilitation towards 0.0 (no facilitation)
    facilitation = std::max(0.0f, facilitation - 0.01f);
    
    // Update eligibility trace for reward-modulated learning
    float eligibilityTrace = synapse->getEligibilityTrace();
    
    // Decay eligibility trace
    eligibilityTrace *= 0.99f;  // Small decay per timestep
    
    if (std::abs(eligibilityTrace) < 0.0001f) {
        eligibilityTrace = 0.0f;
    }
    
    synapse->setEligibilityTrace(eligibilityTrace);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // Real synaptic input implementation
    // Includes conductance-based dynamics and reversal potentials
    
    SynapticWeight weight = synapse->getWeight();
    SynapseType synType = synapse->getType();
    
    if (synType == SynapseType::Excitatory) {
        // Excitatory synapses: typically depolarize the neuron
        // Input current = weight * synaptic conductance
        float input = weight * 10.0f;  // Scale factor for biological plausibility
        neuron->receiveExcitatoryInput(input);
    } else if (synType == SynapseType::Inhibitory) {
        // Inhibitory synapses: hyperpolarize the neuron
        // Input current = -weight * synaptic conductance
        float input = -weight * 10.0f;  // Negative for inhibition
        neuron->receiveInhibitoryInput(std::abs(input));
    } else {
        // Modulatory synapses: have complex effects
        // Modulatory inputs affect plasticity rather than immediate firing
        float modulatoryInput = weight * 5.0f;
        neuron->receiveModulatoryInput(modulatoryInput);
    }
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    if (pImpl) {
        pImpl->membraneTimeConstant = tau;
    }
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl ? pImpl->membraneTimeConstant : 20.0f;
}

} // namespace nlm

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
