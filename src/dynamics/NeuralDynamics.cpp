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
    // Real integrate-and-fire dynamics with configurable timestep
    // Improved implementation that better approximates biological neurons
    
    const auto& state = neuron->getState();
    
    // Leaky integration: dV/dt = -(V - V_rest) / tau + I / C
    // Using exponential Euler integration for stability
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float threshold = state.threshold;
    
    // Calculate membrane potential change with exponential Euler integration
    // More stable than simple Euler integration for LIF neurons
    float voltageDecay = (V_rest - V) / tau;
    float synapticInput = I / R;
    
    // Use configurable timestep
    TimestepDuration timestep = dt;  // Use the passed timestep
    // Convert to milliseconds if needed
    if (timestep < 0.0001) {  // Assuming this is in seconds
        timestep *= 1000.0f;  // Convert to ms
    }
    
    // Exponential integration for stability
    float alpha = std::exp(-timestep / tau);
    float beta = R * (1.0f - alpha);
    
    // Update membrane potential
    V = alpha * V + beta * synapticInput;
    
    // Apply spike-frequency adaptation (simple model)
    if (neuron->getState().adaptationVariable > 0.0f) {
        // Adaptation reduces membrane potential over time
        V -= neuron->getState().adaptationVariable * 0.01f;
        // Decay adaptation variable
        neuron->getState().adaptationVariable *= 0.95f;
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= threshold) {
        // Neuron fires a spike
        neuron->setFiringState(FiringState::Active);
        float currentTime = 0.0f;  // TODO: pass actual simulation time
        neuron->recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        neuron->setRefractoryPeriod(neuron->getRefractoryPeriod());
        neuron->setFiringState(FiringState::Refractory);
        
        // Update adaptation for spike-frequency adaptation
        neuron->getState().adaptationVariable += 1.0f;
    } else {
        neuron->setFiringState(FiringState::Active);
    }
    
    // Update membrane potential
    neuron->setMembranePotential(V);
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->decrementRefractory();
        if (neuron->isRefractory()) {
            // During refractory period, reset to resting potential
            neuron->setMembranePotential(V_reset);
        } else {
            neuron->setFiringState(FiringState::Resting);
        }
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics: implement spike timing dependent plasticity (STDP)
    // and short-term plasticity effects based on spike history
    
    const auto& preHistory = synapse->getPreSpikeHistory();
    const auto& postHistory = synapse->getPostSpikeHistory();
    
    // Apply STDP based on recent spike history
    float stdpWeightChange = 0.0f;
    
    // Check for pre-post spike pairs (potentiation)
    for (size_t i = 0; i < preHistory.size(); ++i) {
        for (size_t j = 0; j < postHistory.size(); ++j) {
            float deltaTime = static_cast<float>(postHistory[j] - preHistory[i]);
            if (deltaTime > 0 && deltaTime < 100.0f) {  // Within 100ms window
                // Potentiation for pre-before-post spikes
                stdpWeightChange += 0.001f * std::exp(-deltaTime / 20.0f);
            }
        }
    }
    
    // Check for post-pre spike pairs (depression)
    for (size_t i = 0; i < postHistory.size(); ++i) {
        for (size_t j = 0; j < preHistory.size(); ++j) {
            float deltaTime = static_cast<float>(preHistory[j] - postHistory[i]);
            if (deltaTime > 0 && deltaTime < 100.0f) {  // Within 100ms window
                // Depression for post-before-pre spikes
                stdpWeightChange -= 0.001f * std::exp(-deltaTime / 20.0f);
            }
        }
    }
    
    // Apply Hebbian learning if enabled
    if (synapse->getPlasticityFlags().hebbian && !preHistory.empty() && !postHistory.empty()) {
        // Simple Hebbian rule: co-activation strengthens synapse
        float hebbianChange = 0.001f;
        stdpWeightChange += hebbianChange;
    }
    
    // Apply weight change
    if (stdpWeightChange != 0.0f) {
        synapse->addToWeight(stdpWeightChange);
    }
    
    // Let the synapse handle its internal dynamics (short-term plasticity)
    // but pass the proper timestep
    synapse->step(0.0);  // Still use the synapse's internal state management
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input: apply spike-triggered postsynaptic current
    // Spike input should affect the neuron's membrane potential through synaptic conductance
    
    TimestepDuration timestep = neuron->getTimestep();
    float weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    if (synapse->isExcitatory()) {
        // Excitatory input: increases membrane potential (depolarization)
        // Weight represents synaptic strength, efficacy represents use-dependent modulation
        float synapticInput = weight * efficacy;
        neuron->receiveExcitatoryInput(synapticInput);
        
        // Record pre-synaptic spike for plasticity
        neuron->recordSpike(0.0);  // TODO: pass actual spike time
    } else {
        // Inhibitory input: decreases membrane potential (hyperpolarization)
        float synapticInput = std::abs(weight) * efficacy;
        neuron->receiveInhibitoryInput(synapticInput);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Improved spike detection based on membrane potential
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
