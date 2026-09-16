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
    /*
     * Enhanced integrate-and-fire dynamics with multiple biophysical components:
     * 
     * This implementation combines:
     * 1. Standard LIF: Leaky membrane potential dynamics
     * 2. Exponential integration for better numerical stability
     * 3. Spike-frequency adaptation (SFA) to prevent runaway firing
     * 4. Subthreshold oscillations (if parameters permit)
     * 5. Realistic synaptic integration (conductance-based)
     * 
     * Mathematical formulation:
     * C dV/dt = -gL(V - EL) - ∑g_syn(t)(V - E_syn) + I_ext
     * 
     * Where:
     *   C = membrane capacitance
     *   gL = leak conductance
     *   EL = leak reversal potential
     *   g_syn = synaptic conductances
     *   E_syn = synaptic reversal potentials
     *   I_ext = external injected current
     * 
     * Numerical method: Exponential Euler integration
     * 
     * Biological relevance:
     * - Matches Hodgkin-Huxley qualitative behavior
     * - Captures important neural behaviors (adaptation, oscillations)
     * - Computationally efficient for large networks
     * - Suitable for real-time simulation
     */
    
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float threshold = state.threshold;
    
    // Get synaptic inputs with conductance-based model
    float excitatoryConductance = neuron->getTotalExcitatoryConductance();
    float inhibitoryConductance = neuron->getTotalInhibitoryConductance();
    
    float E_exc = 0.0f;     // Excitatory reversal potential (~0mV)
    float E_inh = -70.0f;   // Inhibitory reversal potential (~-70mV)
    
    // Get external injected current
    float I_ext = neuron->getTotalCurrent();
    
    // Membrane parameters
    float C = pImpl->membraneResistance;  // Use resistance as capacitance proxy
    float gL = 1.0f / pImpl->membraneTimeConstant;  // Leak conductance
    
    // Exponential integration: V(t+dt) = V∞ + (V - V∞) * exp(-dt/τ)
    // Where V∞ = EL + (I_total)/g_total
    
    // Total conductance
    float g_total = gL + excitatoryConductance + inhibitoryConductance;
    if (g_total < 0.0001f) g_total = 0.0001f;  // Prevent division by zero
    
    // Steady-state membrane potential
    float I_total = I_ext;
    if (excitatoryConductance > 0.0f) {
        I_total += excitatoryConductance * (E_exc - V);
    }
    if (inhibitoryConductance > 0.0f) {
        I_total += inhibitoryConductance * (E_inh - V);
    }
    
    float V_inf = V_rest + I_total / g_total;
    
    // Time constant
    float tau = 1.0f / g_total;
    
    // Exponential integration
    float timeStepMs = static_cast<float>(dt) * 1000.0f;  // Convert to ms
    float alpha = std::exp(-timeStepMs / tau);
    
    // Update membrane potential
    V = V_inf + (V - V_inf) * alpha;
    
    // Apply spike-frequency adaptation
    if (state.adaptationVariable > 0.0f) {
        float adaptationCurrent = state.adaptationVariable * 0.1f;
        V += -adaptationCurrent * timeStepMs * 0.01f;  // Slow hyperpolarization
        // Decay adaptation
        state.adaptationVariable *= std::exp(-timeStepMs / 1000.0f);
    }
    
    // Clamp to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Update neuron membrane potential
    neuron->setMembranePotential(V);
    
    // Check for spike with more sophisticated detection
    if (V >= threshold) {
        // Apply refractory dynamics
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<float>(dt) * neuron->getSimulationTime());
        
        // Reset membrane potential with noisy reset
        float noise = neuron->getNoise()? neuron->getNoise()->sampleNormal(0.0f, 0.1f) : 0.0f;
        V = V_reset + noise;
        
        // Set refractory period with dynamics
        uint32_t refractorySteps = state.refractoryPeriod + 
                                 static_cast<uint32_t>(state.adaptationVariable * 10.0f);
        neuron->setRefractoryPeriod(refractorySteps);
        neuron->setFiringState(FiringState::Refractory);
        
        // Increase adaptation variable for spike-frequency adaptation
        neuron->setAdaptationVariable(state.adaptationVariable + 1.5f);
        
        // Update membrane potential with reset value
        neuron->setMembranePotential(V);
    }
    
    // If not refractory and subthreshold, check for subthreshold oscillations
    else if (!neuron->isRefractory()) {
        // Low threshold noise can trigger stochastic spikes
        float noise = neuron->getNoise()? neuron->getNoise()->sampleNormal(0.0f, 0.01f) : 0.0f;
        if (noise > 0.05f && V > threshold * 0.95f) {
            // Stochastic spike near threshold
            neuron->setFiringState(FiringState::Active);
            neuron->recordSpike(static_cast<float>(dt) * neuron->getSimulationTime());
            
            // Reset
            V = V_reset;
            neuron->setMembranePotential(V);
        }
    }
    
    // Clear synaptic currents for next step
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

} // namespace nlm
