#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    
    // Coupling with SpikeSystem for event-driven synaptic transmission
    SpikeSystem* spikeSystem;
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), spikeSystem(nullptr) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real integrate-and-fire dynamics using exponential Euler integration
    // dV/dt = (V_rest - V)/tau + I/C
    // Exponential Euler: V_new = V_rest + (V_old - V_rest) * exp(-dt/tau) + (I/C) * (1 - exp(-dt/tau))
    
    auto& state = neuron->getState();
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        neuron->decrementRefractory();
        // During refractory period, clear synaptic input
        neuron->clearTotalCurrent();
        return;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire with exponential Euler integration
    float V = state.membranePotential;
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float I = neuron->getTotalCurrent();
    float threshold = state.threshold;
    
    // Membrane parameters from Neuron.cpp (consistent values)
    float tau = 20.0f;  // ms - membrane time constant
    float C = 0.25f;    // nF - membrane capacitance
    
    // Exponential Euler integration
    // V_new = V_rest + (V - V_rest) * exp(-dt/tau) + (I/C) * (1 - exp(-dt/tau))
    float expFactor = std::exp(-static_cast<float>(dt) / tau);
    float inputFactor = (1.0f - expFactor) / C;
    
    // Apply synaptic input (converted to potential change)
    float synapticContribution = I * inputFactor;
    
    // Apply leak (exponential decay toward resting potential)
    float leakContribution = (V - V_rest) * expFactor;
    
    // Update membrane potential
    float V_new = V_rest + leakContribution + synapticContribution;
    
    // Apply spike-frequency adaptation (as in Neuron.cpp)
    if (state.adaptationVariable > 0.0f) {
        V_new -= state.adaptationVariable * 0.01f;
        state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V_new = std::clamp(V_new, -100.0f, 50.0f);
    
    state.membranePotential = V_new;
    
    // Check for spike
    if (V_new >= threshold) {
        bool fired = true;
        state.firingState = FiringState::Active;
        state.lastSpikeTime = static_cast<float>(neuron->getCurrentTime());
        
        // Record spike
        neuron->recordSpike(state.lastSpikeTime);
        
        // Reset membrane potential
        state.membranePotential = V_reset;
        
        // Enter refractory period
        state.refractoryRemaining = state.refractoryPeriod;
        state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        state.adaptationVariable += 1.0f;
    } else {
        state.firingState = FiringState::Active;
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics using SpikeSystem for event-driven transmission
    // Call the synapse's internal step method with proper timing
    
    // For exponential integration, use the current time (since we have dt)
    // Note: Spike delivery happens through the SpikeSystem's delayed spike mechanism
    // Use the current simulation step as the time reference
    synapse->step(static_cast<float>(dt));
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input using SpikeSystem for delayed spike delivery
    // This method should be called when a spike arrives at the synapse,
    // and it should queue a delayed spike event to deliver input to the neuron
    
    if (pImpl->spikeSystem) {
        // Use real spike timing and synaptic properties for accurate transmission
        Timestamp currentTime = static_cast<Timestamp>(neuron->getCurrentTime());
        SimulationStep currentStep = neuron->getCurrentStep();
        
        // Create a delayed spike event with real timing
        DelayedSpikeEvent event;
        event.source_neuron = synapse->getSourceNeuron();
        event.destination_neuron = neuron->getId();
        event.synapse_id = synapse->getId();
        event.weight = synapse->getWeight();
        event.synapse_type = synapse->getType();
        event.timestamp = currentTime;
        event.delivery_time = currentTime + static_cast<Timestamp>(synapse->getDelay());
        event.step = currentStep;
        event.delivery_step = currentStep + synapse->getDelay();
        event.is_excitatory = synapse->isExcitatory();
        
        // Queue the delayed spike for delivery via SpikeSystem
        pImpl->spikeSystem->queueDelayedSpike(event);
        
        // Record spike history in synapse for plasticity
        synapse->recordPostSpike(currentTime);
    } else {
        // Fallback to direct synaptic input if no SpikeSystem available
        // TODO: Consider using delayed spike mechanism without SpikeSystem
        SynapticWeight weight = synapse->getWeight();
        if (synapse->isExcitatory()) {
            neuron->receiveExcitatoryInput(weight);
        } else {
            neuron->receiveInhibitoryInput(std::abs(weight));
        }
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

void IntegrateAndFireDynamics::setSpikeSystem(SpikeSystem* spikeSystem) {
    pImpl->spikeSystem = spikeSystem;
}

SpikeSystem* IntegrateAndFireDynamics::getSpikeSystem() {
    return pImpl->spikeSystem;
}

const SpikeSystem* IntegrateAndFireDynamics::getSpikeSystem() const {
    return pImpl->spikeSystem;
}

} // namespace nlm
